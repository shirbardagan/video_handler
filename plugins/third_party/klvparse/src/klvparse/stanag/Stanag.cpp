#include <klvparse/stanag/Stanag.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;
namespace encoders = klvparse::encoders;
namespace pretty = klvparse::pretty;
namespace ugly = klvparse::ugly;

// std::unordered_map<uint64_t, std::string> Stanag::number_tag;
std::map<uint64_t, std::string> Stanag::number_tag;
std::unordered_map<std::string, uint64_t> Stanag::tag_number;
// std::unordered_map<uint64_t, std::function<bool(KLV *, KLVStorage *)>> Stanag::parsers;
std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *res, std::shared_ptr<Context>)>> Stanag::parsers;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> Stanag::encoders;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, nl::json *, std::shared_ptr<Context>)>> Stanag::prettys;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, nl::json *, std::shared_ptr<Context>)>> Stanag::uglys;

void Stanag::init() {
    init_tags();
    init_parsers();
    init_encoders();
    init_pretty();
    init_ugly();
}

Stanag::Stanag() {
    init();
    this->ctx = std::make_shared<Context>();
}

Stanag::Stanag(std::shared_ptr<Context> ctx) {
    init();
    this->ctx = ctx;
}

Stanag::~Stanag() {
}

bool Stanag::parse(const uint8_t *data, uint64_t size, nl::json *res, uint64_t skip, bool with_key) {
    std::vector<uint8_t> buffer(data, data + size);
    return parse(buffer, res, skip, with_key);
}

bool Stanag::parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip, bool with_key) {
    ByteReader reader(buffer);
    return parse(reader, res, skip, with_key);
}

bool Stanag::parse(ByteReader &reader, nl::json *res, uint64_t skip, bool with_key) {
    try {
        if (is_strict()) {
            auto start = reader.get_index();
            if (!verify_checksum(reader)) {
                return false;
            }
            reader.set_index(start);
        }
        if (skip > 0) {
            reader.skip(skip);
        }
        nl::json _res = nl::json::object();
        KLV klv;
        nl::json temp_key;
        nl::json temp_value;
        if (with_key) {
            klv = KLV::parse(reader, PARSE_FORMATS::_16, PARSE_FORMATS::_BER);
            if (!parsers::bytes_parser(klv.key, &temp_value)) {
                CTX_LOG_PARSER(Level::error, "failed to get key bytes", &klv, 0, _UAS_DATALINK_LS);
                return false;
            }
            _res[_UAS_DATALINK_LS] = temp_value;
            reader = klv.value.copy();
        }
        uint64_t last_remaining = 0;
        uint64_t remaining = 0;
        for (uint64_t i = 0;; i++) {
            remaining = reader.get_remaining();
            if (remaining <= 0) {
                break;
            } else if (remaining == last_remaining) {
                CTX_LOG(Level::error, "stuck");
                break;
            }
            last_remaining = remaining;
            uint64_t tag_value = 0;
            std::string tag_name = "";
            klv = KLV::parse(reader, PARSE_FORMATS::_BER_OID, PARSE_FORMATS::_BER);
            if (!get_tag(klv, &tag_value)) {
                CTX_LOG_PARSER(Level::error, "failed to get tag", &klv, tag_value, tag_name);
                continue;
            }
            if (!get_key_complex(tag_value, &temp_key)) {
                CTX_LOG_PARSER(Level::warn, "not registered", &klv, tag_value, tag_name);
                continue;
            }
            tag_name = temp_key.dump();
            // CTX_LOG_PARSER(Level::debug, klv.debug(), &klv, tag_value, tag_name);
            if (!get_value_complex(klv, tag_value, &temp_value)) {
                CTX_LOG_PARSER(Level::error, "parser failed", &klv, tag_value, tag_name);
                continue;
            }
            CTX_LOG_PARSER(Level::debug, nl::ensure_dump(temp_value), &klv, tag_value, tag_name);
            _res[temp_key.get<std::string>()] = temp_value;
        }
        *res = _res;
        return true;
    } catch (std::exception &e) {

        CTX_LOG(Level::error, e.what());
        return false;
    }
    return false;
}

bool Stanag::pretty(const nl::json &j, nl::json *res) {
    nl::json _res = j;
    for (auto const &[number, tag] : this->number_tag) {
        auto j_pair = j.find(tag);
        if (j_pair == j.end()) {
            continue;
        }
        const nl::json &j_value = j_pair.value();
        auto prettifier = prettys.find(number);
        if (prettifier == prettys.end()) {
            // CTX_LOG_PRETTY(Level::debug, "pretty missing", &j_value, number, tag);
            continue;
        }
        nl::json result = j_value;
        if (!prettifier->second(j_value, &result, this->ctx)) {
            CTX_LOG_PRETTY(Level::warn, "pretty failed", &j_value, number, tag);
            continue;
        };
        {
            if (ctx->verbosity >= Level::debug) {
                std::stringstream sstr;
                sstr << nl::ensure_dump(j_value) << " => " << nl::ensure_dump(result);
                CTX_LOG_PRETTY(Level::debug, sstr.str(), &j_value, number, tag);
            }
        }
        _res[tag] = result;
    }
    *res = _res;
    return true;
};

bool Stanag::verify_checksum(ByteReader &reader) {
    bool checksum_is_valid = false;
    uint16_t calculated = 0;
    uint16_t actual = 0;
    if (!_verify_checksum(reader, &checksum_is_valid, &calculated, &actual)) {
        CTX_LOG(Level::warn, "failed to calculate checksum, ignoring packet")
        return false;
    }
    if (!checksum_is_valid) {
        {
            std::stringstream sstr;
            sstr << "checksum is invalid: "
                 << "calculated: 0x" << std::hex << std::setfill('0') << std::setw(4) << calculated << ", "
                 << "actual: 0x" << std::hex << std::setfill('0') << std::setw(4) << actual;
            CTX_LOG(Level::warn, sstr.str());
        }
        return false;
    }
    return true;
}
static std::vector<uint8_t> uas_datalink_ls_default{0x06, 0x0e, 0x2b, 0x34, 0x02, 0x0b, 0x01, 0x01, 0x0e, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00};
bool Stanag::encode(const nl::json &j, std::vector<uint8_t> *res) {
    // std::vector<uint8_t> _res(__TYPICAL_KLV_SIZE);
    std::vector<uint8_t> _res;
    try {
        {
            const nl::json *uas_datalink_ls_json_ptr = NULL;
            auto uas_datalink_ls = j.find(_UAS_DATALINK_LS);
            if (uas_datalink_ls == j.end()) {
                CTX_LOG_ENCODER(Level::warn, "missing", uas_datalink_ls_json_ptr, 0, _UAS_DATALINK_LS);
                if (is_strict()) {
                    return false;
                }
                _res.insert(_res.end(), uas_datalink_ls_default.begin(), uas_datalink_ls_default.end());
            } else {
                const nl::json &uas_datalink_ls_json = uas_datalink_ls.value();
                uas_datalink_ls_json_ptr = &uas_datalink_ls_json;
                if (!uas_datalink_ls_json.is_string()) {
                    CTX_LOG_ENCODER(Level::error, "not a string", uas_datalink_ls_json_ptr, 0, _UAS_DATALINK_LS);
                    return false;
                }
                std::vector<uint8_t> uas_datalink_ls_bytes;
                if (!encoders::bytes_encoder(uas_datalink_ls_json, &uas_datalink_ls_bytes)) {
                    CTX_LOG_ENCODER(Level::error, "not a string", uas_datalink_ls_json_ptr, 0, _UAS_DATALINK_LS);
                    return false;
                }
                _res.insert(_res.end(), uas_datalink_ls_bytes.begin(), uas_datalink_ls_bytes.end());
            }
        }
        std::vector<uint8_t> tags_bytes;
        for (auto const &[number, tag] : this->number_tag) {
            if (number == 1) {
                continue; // ignore checksum tag, we'll handle this later
            }             // #TODO: tag 2 precision timestamp should be first
            auto j_pair = j.find(tag);
            if (j_pair == j.end()) {
                continue;
            }
            const nl::json &j_value = j_pair.value();
            auto enc = encoders.find(number);
            if (enc == encoders.end()) {
                CTX_LOG_ENCODER(Level::warn, "encoder missing", &j_value, number, tag);
                continue;
            }
            std::vector<uint8_t> value_bytes;
            if (!enc->second(j_value, &value_bytes, ctx)) {
                CTX_LOG_ENCODER(Level::error, "encoder failed", &j_value, number, tag);
                continue;
            }
            std::vector<uint8_t> tag_bytes;
            uint64_t n = number;
            if (!encoders::encode_ber_oid(n, &tag_bytes)) {
                {
                    std::stringstream sstr;
                    sstr << "failed to encode tag's tag (" << number << ") in ber oid";
                    CTX_LOG_ENCODER(Level::error, sstr.str(), &j_value, number, tag);
                }
                continue;
            }
            std::vector<uint8_t> length_bytes;
            uint64_t l = value_bytes.size();
            if (!encoders::encode_ber(l, &length_bytes)) {
                {
                    std::stringstream sstr;
                    sstr << "failed to encode tag's length (" << l << ") in ber";
                    CTX_LOG_ENCODER(Level::error, sstr.str(), &j_value, number, tag);
                }
                continue;
            }
            tags_bytes.insert(tags_bytes.end(), tag_bytes.begin(), tag_bytes.end());
            tags_bytes.insert(tags_bytes.end(), length_bytes.begin(), length_bytes.end());
            tags_bytes.insert(tags_bytes.end(), value_bytes.begin(), value_bytes.end());
        }
        { // checksum related - insert checksum tlv - tag 1, length 2,
            std::vector<uint8_t> checksum_klv_bytes = {0x01, 0x02, 0x00, 0x00};
            tags_bytes.insert(tags_bytes.end(), checksum_klv_bytes.begin(), checksum_klv_bytes.end());
        }
        std::vector<uint8_t> length_bytes;
        uint64_t l = tags_bytes.size();
        if (!encoders::encode_ber(l, &length_bytes)) {
            {
                std::stringstream sstr;
                sstr << "failed length (" << l << ") in ber";
                CTX_LOG(Level::error, sstr.str());
            }
            return false;
        }
        _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
        _res.insert(_res.end(), tags_bytes.begin(), tags_bytes.end());
        { // checksum related - update checksum value
            uint16_t calculated;
            uint16_t actual;
            ByteReader r(_res);
            _verify_checksum(r, NULL, &calculated, &actual);
            _res[_res.size() - 2] = (calculated >> 8) & 0xff;
            _res[_res.size() - 1] = calculated & 0xff;
            {
                std::stringstream sstr;
                sstr << "checksum: calculated:" << calculated << ", actual:" << actual;
                CTX_LOG(Level::debug, sstr.str());
            }
        }
        // { // checksum validation
        //     uint16_t calculated;
        //     uint16_t actual;
        //     ByteReader r(_res);
        //     _verify_checksum(r, NULL, &calculated, &actual);
        //     {
        //         std::stringstream sstr;
        //         sstr << "checksum: calculated:" << calculated << ", actual:" << actual;
        //         CTX_LOG(Level::debug, sstr.str());
        //     }
        // }
        *res = std::move(_res);
        return true;
    } catch (std::exception &e) {
        CTX_LOG(Level::error, e.what());
        return false;
    }
    return false;
}

bool Stanag::get_tag(KLV &klv, uint64_t *res) {
    uint64_t tag;
    klv.key.restart();
    if (!klv.key.get_ber_oid(&tag)) {
        return false;
    }
    *res = tag;
    return true;
}

bool Stanag::get_key_complex(uint64_t tag, nl::json *res) {
    auto pair = this->number_tag.find(tag);
    if (pair != this->number_tag.end()) {
        *res = pair->second;
        return true;
    }
    return false;
}

bool Stanag::get_key(KLV &klv, nl::json *res) {
    uint64_t tag;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_key_complex(tag, res);
}

bool Stanag::get_value_complex(KLV &klv, uint64_t tag, nl::json *res) {
    auto pair = this->parsers.find(tag);
    // klv.value.restart();
    if (pair != this->parsers.end()) {
        return pair->second(klv.value, res, this->ctx);
    }
    return false;
}

bool Stanag::get_value(KLV &klv, nl::json *res) {
    uint64_t tag = 0;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_value_complex(klv, tag, res);
}

void Stanag::init_parsers() {
    if (Stanag::parsers.size() > 0)
        return;

    // 0x01 - 1 - checksum
    Stanag::parsers.insert(std::make_pair(0x01, parsers::stanag_checksum_parser));
    // 0x02 - 2 - precision_time_stamp
    Stanag::parsers.insert(std::make_pair(0x02, parsers::stanag_precision_time_stamp_parser));
    // 0x03 - 3 - mission_id
    Stanag::parsers.insert(std::make_pair(0x03, parsers::stanag_mission_id_parser));
    // 0x04 - 4 - platform_tail_number
    Stanag::parsers.insert(std::make_pair(0x04, parsers::stanag_platform_tail_number_parser));
    // 0x05 - 5 - platform_heading_angle
    Stanag::parsers.insert(std::make_pair(0x05, parsers::stanag_platform_heading_angle_parser));
    // 0x06 - 6 - platform_pitch_angle
    Stanag::parsers.insert(std::make_pair(0x06, parsers::stanag_platform_pitch_angle_parser));
    // 0x07 - 7 - platform_roll_angle
    Stanag::parsers.insert(std::make_pair(0x07, parsers::stanag_platform_roll_angle_parser));
    // 0x08 - 8 - platform_true_airspeed
    Stanag::parsers.insert(std::make_pair(0x08, parsers::stanag_platform_true_airspeed_parser));
    // 0x09 - 9 - platform_indicated_airspeed
    Stanag::parsers.insert(std::make_pair(0x09, parsers::stanag_platform_indicated_airspeed_parser));
    // 0x0A - 10 - platform_designation
    Stanag::parsers.insert(std::make_pair(0x0A, parsers::stanag_platform_designation_parser));
    // 0x0B - 11 - image_source_sensor
    Stanag::parsers.insert(std::make_pair(0x0B, parsers::stanag_image_source_sensor_parser));
    // 0x0C - 12 - image_coordinate_system
    Stanag::parsers.insert(std::make_pair(0x0C, parsers::stanag_image_coordinate_system_parser));
    // 0x0D - 13 - sensor_latitude
    Stanag::parsers.insert(std::make_pair(0x0D, parsers::stanag_sensor_latitude_parser));
    // 0x0E - 14 - sensor_longitude
    Stanag::parsers.insert(std::make_pair(0x0E, parsers::stanag_sensor_longitude_parser));
    // 0x0F - 15 - sensor_true_altitude
    Stanag::parsers.insert(std::make_pair(0x0F, parsers::stanag_sensor_true_altitude_parser));
    // 0x10 - 16 - sensor_horizontal_field_of_view
    Stanag::parsers.insert(std::make_pair(0x10, parsers::stanag_sensor_horizontal_field_of_view_parser));
    // 0x11 - 17 - sensor_vertical_field_of_view
    Stanag::parsers.insert(std::make_pair(0x11, parsers::stanag_sensor_vertical_field_of_view_parser));
    // 0x12 - 18 - sensor_relative_azimuth_angle
    Stanag::parsers.insert(std::make_pair(0x12, parsers::stanag_sensor_relative_azimuth_angle_parser));
    // 0x13 - 19 - sensor_relative_elevation_angle
    Stanag::parsers.insert(std::make_pair(0x13, parsers::stanag_sensor_relative_elevation_angle_parser));
    // 0x14 - 20 - sensor_relative_roll_angle
    Stanag::parsers.insert(std::make_pair(0x14, parsers::stanag_sensor_relative_roll_angle_parser));
    // 0x15 - 21 - slant_range
    Stanag::parsers.insert(std::make_pair(0x15, parsers::stanag_slant_range_parser));
    // 0x16 - 22 - target_width
    Stanag::parsers.insert(std::make_pair(0x16, parsers::stanag_target_width_parser));
    // 0x17 - 23 - frame_center_latitude
    Stanag::parsers.insert(std::make_pair(0x17, parsers::stanag_frame_center_latitude_parser));
    // 0x18 - 24 - frame_center_longitude
    Stanag::parsers.insert(std::make_pair(0x18, parsers::stanag_frame_center_longitude_parser));
    // 0x19 - 25 - frame_center_elevation
    Stanag::parsers.insert(std::make_pair(0x19, parsers::stanag_frame_center_elevation_parser));
    // 0x1A - 26 - offset_corner_latitude_point_1
    Stanag::parsers.insert(std::make_pair(0x1A, parsers::stanag_offset_corner_latitude_point_1_parser));
    // 0x1B - 27 - offset_corner_longitude_point_1
    Stanag::parsers.insert(std::make_pair(0x1B, parsers::stanag_offset_corner_longitude_point_1_parser));
    // 0x1C - 28 - offset_corner_latitude_point_2
    Stanag::parsers.insert(std::make_pair(0x1C, parsers::stanag_offset_corner_latitude_point_2_parser));
    // 0x1D - 29 - offset_corner_longitude_point_2
    Stanag::parsers.insert(std::make_pair(0x1D, parsers::stanag_offset_corner_longitude_point_2_parser));
    // 0x1E - 30 - offset_corner_latitude_point_3
    Stanag::parsers.insert(std::make_pair(0x1E, parsers::stanag_offset_corner_latitude_point_3_parser));
    // 0x1F - 31 - offset_corner_longitude_point_3
    Stanag::parsers.insert(std::make_pair(0x1F, parsers::stanag_offset_corner_longitude_point_3_parser));
    // 0x20 - 32 - offset_corner_latitude_point_4
    Stanag::parsers.insert(std::make_pair(0x20, parsers::stanag_offset_corner_latitude_point_4_parser));
    // 0x21 - 33 - offset_corner_longitude_point_4
    Stanag::parsers.insert(std::make_pair(0x21, parsers::stanag_offset_corner_longitude_point_4_parser));
    // 0x22 - 34 - icing_detected
    Stanag::parsers.insert(std::make_pair(0x22, parsers::stanag_icing_detected_parser));
    // 0x23 - 35 - wind_direction
    Stanag::parsers.insert(std::make_pair(0x23, parsers::stanag_wind_direction_parser));
    // 0x24 - 36 - wind_speed
    Stanag::parsers.insert(std::make_pair(0x24, parsers::stanag_wind_speed_parser));
    // 0x25 - 37 - static_pressure
    Stanag::parsers.insert(std::make_pair(0x25, parsers::stanag_static_pressure_parser));
    // 0x26 - 38 - density_altitude
    Stanag::parsers.insert(std::make_pair(0x26, parsers::stanag_density_altitude_parser));
    // 0x27 - 39 - outside_air_temperature
    Stanag::parsers.insert(std::make_pair(0x27, parsers::stanag_outside_air_temperature_parser));
    // 0x28 - 40 - target_location_latitude
    Stanag::parsers.insert(std::make_pair(0x28, parsers::stanag_target_location_latitude_parser));
    // 0x29 - 41 - target_location_longitude
    Stanag::parsers.insert(std::make_pair(0x29, parsers::stanag_target_location_longitude_parser));
    // 0x2A - 42 - target_location_elevation
    Stanag::parsers.insert(std::make_pair(0x2A, parsers::stanag_target_location_elevation_parser));
    // 0x2B - 43 - target_track_gate_width
    Stanag::parsers.insert(std::make_pair(0x2B, parsers::stanag_target_track_gate_width_parser));
    // 0x2C - 44 - target_track_gate_height
    Stanag::parsers.insert(std::make_pair(0x2C, parsers::stanag_target_track_gate_height_parser));
    // 0x2D - 45 - target_error_estimate_ce90
    Stanag::parsers.insert(std::make_pair(0x2D, parsers::stanag_target_error_estimate_ce90_parser));
    // 0x2E - 46 - target_error_estimate_le90
    Stanag::parsers.insert(std::make_pair(0x2E, parsers::stanag_target_error_estimate_le90_parser));
    // 0x2F - 47 - generic_flag_data
    Stanag::parsers.insert(std::make_pair(0x2F, parsers::stanag_generic_flag_data_parser));
    // 0x30 - 48 - security_local_set
    Stanag::parsers.insert(std::make_pair(0x30, parsers::stanag_security_local_set_parser));
    // 0x31 - 49 - differential_pressure
    Stanag::parsers.insert(std::make_pair(0x31, parsers::stanag_differential_pressure_parser));
    // 0x32 - 50 - platform_angle_of_attack
    Stanag::parsers.insert(std::make_pair(0x32, parsers::stanag_platform_angle_of_attack_parser));
    // 0x33 - 51 - platform_vertical_speed
    Stanag::parsers.insert(std::make_pair(0x33, parsers::stanag_platform_vertical_speed_parser));
    // 0x34 - 52 - platform_sideslip_angle
    Stanag::parsers.insert(std::make_pair(0x34, parsers::stanag_platform_sideslip_angle_parser));
    // 0x35 - 53 - airfield_barometric_pressure
    Stanag::parsers.insert(std::make_pair(0x35, parsers::stanag_airfield_barometric_pressure_parser));
    // 0x36 - 54 - airfield_elevation
    Stanag::parsers.insert(std::make_pair(0x36, parsers::stanag_airfield_elevation_parser));
    // 0x37 - 55 - relative_humidity
    Stanag::parsers.insert(std::make_pair(0x37, parsers::stanag_relative_humidity_parser));
    // 0x38 - 56 - platform_ground_speed
    Stanag::parsers.insert(std::make_pair(0x38, parsers::stanag_platform_ground_speed_parser));
    // 0x39 - 57 - ground_range
    Stanag::parsers.insert(std::make_pair(0x39, parsers::stanag_ground_range_parser));
    // 0x3A - 58 - platform_fuel_remaining
    Stanag::parsers.insert(std::make_pair(0x3A, parsers::stanag_platform_fuel_remaining_parser));
    // 0x3B - 59 - platform_call_sign
    Stanag::parsers.insert(std::make_pair(0x3B, parsers::stanag_platform_call_sign_parser));
    // 0x3C - 60 - weapon_load
    Stanag::parsers.insert(std::make_pair(0x3C, parsers::stanag_weapon_load_parser));
    // 0x3D - 61 - weapon_fired
    Stanag::parsers.insert(std::make_pair(0x3D, parsers::stanag_weapon_fired_parser));
    // 0x3E - 62 - laser_prf_code
    Stanag::parsers.insert(std::make_pair(0x3E, parsers::stanag_laser_prf_code_parser));
    // 0x3F - 63 - sensor_field_of_view_name
    Stanag::parsers.insert(std::make_pair(0x3F, parsers::stanag_sensor_field_of_view_name_parser));
    // 0x40 - 64 - platform_magnetic_heading
    Stanag::parsers.insert(std::make_pair(0x40, parsers::stanag_platform_magnetic_heading_parser));
    // 0x41 - 65 - uas_datalink_ls_version_number
    Stanag::parsers.insert(std::make_pair(0x41, parsers::stanag_uas_datalink_ls_version_number_parser));

    // // 0x42 - 66 - deprecated

    // 0x42 - 66 - target_location_covariance_matrix
    Stanag::parsers.insert(std::make_pair(0x42, parsers::stanag_target_location_covariance_matrix_parser));
    // 0x43 - 67 - alternate_platform_latitude
    Stanag::parsers.insert(std::make_pair(0x43, parsers::stanag_alternate_platform_latitude_parser));
    // 0x44 - 68 - alternate_platform_longitude
    Stanag::parsers.insert(std::make_pair(0x44, parsers::stanag_alternate_platform_longitude_parser));
    // 0x45 - 69 - alternate_platform_altitude
    Stanag::parsers.insert(std::make_pair(0x45, parsers::stanag_alternate_platform_altitude_parser));
    // 0x46 - 70 - alternate_platform_name
    Stanag::parsers.insert(std::make_pair(0x46, parsers::stanag_alternate_platform_name_parser));
    // 0x47 - 71 - alternate_platform_heading
    Stanag::parsers.insert(std::make_pair(0x47, parsers::stanag_alternate_platform_heading_parser));
    // 0x48 - 72 - event_start_time_utc
    Stanag::parsers.insert(std::make_pair(0x48, parsers::stanag_event_start_time_utc_parser));
    // 0x49 - 73 - rvt_local_set
    Stanag::parsers.insert(std::make_pair(0x49, parsers::stanag_rvt_local_set_parser));
    // 0x4A - 74 - vmti_local_set
    Stanag::parsers.insert(std::make_pair(0x4A, parsers::stanag_vmti_local_set_parser));
    // 0x4B - 75 - sensor_ellipsoid_height
    Stanag::parsers.insert(std::make_pair(0x4B, parsers::stanag_sensor_ellipsoid_height_parser));
    // 0x4C - 76 - alternate_platform_ellipsoid_height
    Stanag::parsers.insert(std::make_pair(0x4C, parsers::stanag_alternate_platform_ellipsoid_height_parser));
    // 0x4D - 77 - operational_mode
    Stanag::parsers.insert(std::make_pair(0x4D, parsers::stanag_operational_mode_parser));
    // 0x4E - 78 - frame_center_height_above_ellipsoid
    Stanag::parsers.insert(std::make_pair(0x4E, parsers::stanag_frame_center_height_above_ellipsoid_parser));
    // 0x4F - 79 - sensor_north_velocity
    Stanag::parsers.insert(std::make_pair(0x4F, parsers::stanag_sensor_north_velocity_parser));
    // 0x50 - 80 - sensor_east_velocity
    Stanag::parsers.insert(std::make_pair(0x50, parsers::stanag_sensor_east_velocity_parser));
    // 0x51 - 81 - image_horizon_pixel_pack
    Stanag::parsers.insert(std::make_pair(0x51, parsers::stanag_image_horizon_pixel_pack_parser));
    // 0x52 - 82 - corner_latitude_point_1
    Stanag::parsers.insert(std::make_pair(0x52, parsers::stanag_corner_latitude_point_1_parser));
    // 0x53 - 83 - corner_longitude_point_1
    Stanag::parsers.insert(std::make_pair(0x53, parsers::stanag_corner_longitude_point_1_parser));
    // 0x54 - 84 - corner_latitude_point_2
    Stanag::parsers.insert(std::make_pair(0x54, parsers::stanag_corner_latitude_point_2_parser));
    // 0x55 - 85 - corner_longitude_point_2
    Stanag::parsers.insert(std::make_pair(0x55, parsers::stanag_corner_longitude_point_2_parser));
    // 0x56 - 86 - corner_latitude_point_3
    Stanag::parsers.insert(std::make_pair(0x56, parsers::stanag_corner_latitude_point_3_parser));
    // 0x57 - 87 - corner_longitude_point_3
    Stanag::parsers.insert(std::make_pair(0x57, parsers::stanag_corner_longitude_point_3_parser));
    // 0x58 - 88 - corner_latitude_point_4
    Stanag::parsers.insert(std::make_pair(0x58, parsers::stanag_corner_latitude_point_4_parser));
    // 0x59 - 89 - corner_longitude_point_4
    Stanag::parsers.insert(std::make_pair(0x59, parsers::stanag_corner_longitude_point_4_parser));
    // 0x5A - 90 - platform_pitch_angle_full
    Stanag::parsers.insert(std::make_pair(0x5A, parsers::stanag_platform_pitch_angle_full_parser));
    // 0x5B - 91 - platform_roll_angle_full
    Stanag::parsers.insert(std::make_pair(0x5B, parsers::stanag_platform_roll_angle_full_parser));
    // 0x5C - 92 - platform_angle_of_attack_full
    Stanag::parsers.insert(std::make_pair(0x5C, parsers::stanag_platform_angle_of_attack_full_parser));
    // 0x5D - 93 - platform_sideslip_angle_full
    Stanag::parsers.insert(std::make_pair(0x5D, parsers::stanag_platform_sideslip_angle_full_parser));
    // 0x5E - 94 - miis_core_identifier
    Stanag::parsers.insert(std::make_pair(0x5E, parsers::stanag_miis_core_identifier_parser));
    // 0x5F - 95 - sar_motion_imagery_local_set
    Stanag::parsers.insert(std::make_pair(0x5F, parsers::stanag_sar_motion_imagery_local_set_parser));
    // 0x60 - 96 - target_width_extended
    Stanag::parsers.insert(std::make_pair(0x60, parsers::stanag_target_width_extended_parser));
    // 0x61 - 97 - range_image_local_set
    Stanag::parsers.insert(std::make_pair(0x61, parsers::stanag_range_image_local_set_parser));
    // 0x62 - 98 - geo_registration_local_set
    Stanag::parsers.insert(std::make_pair(0x62, parsers::stanag_geo_registration_local_set_parser));
    // 0x63 - 99 - composite_imaging_local_set
    Stanag::parsers.insert(std::make_pair(0x63, parsers::stanag_composite_imaging_local_set_parser));
    // 0x64 - 100 - segment_local_set
    Stanag::parsers.insert(std::make_pair(0x64, parsers::stanag_segment_local_set_parser));
    // 0x65 - 101 - amend_local_set
    Stanag::parsers.insert(std::make_pair(0x65, parsers::stanag_amend_local_set_parser));
    // 0x66 - 102 - sdcc_flp
    Stanag::parsers.insert(std::make_pair(0x66, parsers::stanag_sdcc_flp_parser));
    // 0x67 - 103 - density_altitude_extended
    Stanag::parsers.insert(std::make_pair(0x67, parsers::stanag_density_altitude_extended_parser));
    // 0x68 - 104 - sensor_ellipsoid_height_extended
    Stanag::parsers.insert(std::make_pair(0x68, parsers::stanag_sensor_ellipsoid_height_extended_parser));
    // 0x69 - 105 - alternate_platform_ellipsoid_height_extended
    Stanag::parsers.insert(std::make_pair(0x69, parsers::stanag_alternate_platform_ellipsoid_height_extended_parser));
    // 0x6A - 106 - stream_designator
    Stanag::parsers.insert(std::make_pair(0x6A, parsers::stanag_stream_designator_parser));
    // 0x6B - 107 - operational_base
    Stanag::parsers.insert(std::make_pair(0x6B, parsers::stanag_operational_base_parser));
    // 0x6C - 108 - broadcast_source
    Stanag::parsers.insert(std::make_pair(0x6C, parsers::stanag_broadcast_source_parser));
    // 0x6D - 109 - range_to_recovery_location
    Stanag::parsers.insert(std::make_pair(0x6D, parsers::stanag_range_to_recovery_location_parser));
    // 0x6E - 110 - time_airborne
    Stanag::parsers.insert(std::make_pair(0x6E, parsers::stanag_time_airborne_parser));
    // 0x6F - 111 - propulsion_unit_speed
    Stanag::parsers.insert(std::make_pair(0x6F, parsers::stanag_propulsion_unit_speed_parser));
    // 0x70 - 112 - platform_course_angle
    Stanag::parsers.insert(std::make_pair(0x70, parsers::stanag_platform_course_angle_parser));
    // 0x71 - 113 - altitude_agl
    Stanag::parsers.insert(std::make_pair(0x71, parsers::stanag_altitude_agl_parser));
    // 0x72 - 114 - radar_altimeter
    Stanag::parsers.insert(std::make_pair(0x72, parsers::stanag_radar_altimeter_parser));
    // 0x73 - 115 - control_command
    Stanag::parsers.insert(std::make_pair(0x73, parsers::stanag_control_command_parser));
    // 0x74 - 116 - control_command_verification_list
    Stanag::parsers.insert(std::make_pair(0x74, parsers::stanag_control_command_verification_list_parser));
    // 0x75 - 117 - sensor_azimuth_rate
    Stanag::parsers.insert(std::make_pair(0x75, parsers::stanag_sensor_azimuth_rate_parser));
    // 0x76 - 118 - sensor_elevation_rate
    Stanag::parsers.insert(std::make_pair(0x76, parsers::stanag_sensor_elevation_rate_parser));
    // 0x77 - 119 - sensor_roll_rate
    Stanag::parsers.insert(std::make_pair(0x77, parsers::stanag_sensor_roll_rate_parser));
    // 0x78 - 120 - onboard_mi_storage_percent_full
    Stanag::parsers.insert(std::make_pair(0x78, parsers::stanag_onboard_mi_storage_percent_full_parser));
    // 0x79 - 121 - active_wavelength_list
    Stanag::parsers.insert(std::make_pair(0x79, parsers::stanag_active_wavelength_list_parser));
    // 0x7A - 122 - country_codes
    Stanag::parsers.insert(std::make_pair(0x7A, parsers::stanag_country_codes_parser));
    // 0x7B - 123 - number_of_navsats_in_view
    Stanag::parsers.insert(std::make_pair(0x7B, parsers::stanag_number_of_navsats_in_view_parser));
    // 0x7C - 124 - positioning_method_source
    Stanag::parsers.insert(std::make_pair(0x7C, parsers::stanag_positioning_method_source_parser));
    // 0x7D - 125 - platform_status
    Stanag::parsers.insert(std::make_pair(0x7D, parsers::stanag_platform_status_parser));
    // 0x7E - 126 - sensor_control_mode
    Stanag::parsers.insert(std::make_pair(0x7E, parsers::stanag_sensor_control_mode_parser));
    // 0x7F - 127 - sensor_frame_rate_pack
    Stanag::parsers.insert(std::make_pair(0x7F, parsers::stanag_sensor_frame_rate_pack_parser));
    // 0x80 - 128 - wavelengths_list
    Stanag::parsers.insert(std::make_pair(0x80, parsers::stanag_wavelengths_list_parser));
    // 0x81 - 129 - target_id
    Stanag::parsers.insert(std::make_pair(0x81, parsers::stanag_target_id_parser));
    // 0x82 - 130 - airbase_locations
    Stanag::parsers.insert(std::make_pair(0x82, parsers::stanag_airbase_locations_parser));
    // 0x83 - 131 - takeoff_time
    Stanag::parsers.insert(std::make_pair(0x83, parsers::stanag_takeoff_time_parser));
    // 0x84 - 132 - transmission_frequency
    Stanag::parsers.insert(std::make_pair(0x84, parsers::stanag_transmission_frequency_parser));
    // 0x85 - 133 - onboard_mi_storage_capacity
    Stanag::parsers.insert(std::make_pair(0x85, parsers::stanag_onboard_mi_storage_capacity_parser));
    // 0x86 - 134 - zoom_percentage
    Stanag::parsers.insert(std::make_pair(0x86, parsers::stanag_zoom_percentage_parser));
    // 0x87 - 135 - communications_method
    Stanag::parsers.insert(std::make_pair(0x87, parsers::stanag_communications_method_parser));
    // 0x88 - 136 - leap_seconds
    Stanag::parsers.insert(std::make_pair(0x88, parsers::stanag_leap_seconds_parser));
    // 0x89 - 137 - correction_offset
    Stanag::parsers.insert(std::make_pair(0x89, parsers::stanag_correction_offset_parser));
    // 0x8A - 138 - payload_list
    Stanag::parsers.insert(std::make_pair(0x8A, parsers::stanag_payload_list_parser));
    // 0x8B - 139 - active_payloads
    Stanag::parsers.insert(std::make_pair(0x8B, parsers::stanag_active_payloads_parser));
    // 0x8C - 140 - weapons_stores
    Stanag::parsers.insert(std::make_pair(0x8C, parsers::stanag_weapons_stores_parser));
    // 0x8D - 141 - waypoint_list
    Stanag::parsers.insert(std::make_pair(0x8D, parsers::stanag_waypoint_list_parser));
    // 0x8E - 142 - view_domain
    Stanag::parsers.insert(std::make_pair(0x8E, parsers::stanag_view_domain_parser));
}

void Stanag::init_encoders() {
    if (Stanag::encoders.size() > 0)
        return;

    // // 0x01 - 1 - checksum
    // Stanag::encoders.insert(std::make_pair(0x01, encoders::stanag_checksum_encoder));
    // 0x02 - 2 - precision_time_stamp
    Stanag::encoders.insert(std::make_pair(0x02, encoders::stanag_precision_time_stamp_encoder));
    // 0x03 - 3 - mission_id
    Stanag::encoders.insert(std::make_pair(0x03, encoders::stanag_mission_id_encoder));
    // 0x04 - 4 - platform_tail_number
    Stanag::encoders.insert(std::make_pair(0x04, encoders::stanag_platform_tail_number_encoder));
    // 0x05 - 5 - platform_heading_angle
    Stanag::encoders.insert(std::make_pair(0x05, encoders::stanag_platform_heading_angle_encoder));
    // 0x06 - 6 - platform_pitch_angle
    Stanag::encoders.insert(std::make_pair(0x06, encoders::stanag_platform_pitch_angle_encoder));
    // 0x07 - 7 - platform_roll_angle
    Stanag::encoders.insert(std::make_pair(0x07, encoders::stanag_platform_roll_angle_encoder));
    // 0x08 - 8 - platform_true_airspeed
    Stanag::encoders.insert(std::make_pair(0x08, encoders::stanag_platform_true_airspeed_encoder));
    // 0x09 - 9 - platform_indicated_airspeed
    Stanag::encoders.insert(std::make_pair(0x09, encoders::stanag_platform_indicated_airspeed_encoder));
    // 0x0A - 10 - platform_designation
    Stanag::encoders.insert(std::make_pair(0x0A, encoders::stanag_platform_designation_encoder));
    // 0x0B - 11 - image_source_sensor
    Stanag::encoders.insert(std::make_pair(0x0B, encoders::stanag_image_source_sensor_encoder));
    // 0x0C - 12 - image_coordinate_system
    Stanag::encoders.insert(std::make_pair(0x0C, encoders::stanag_image_coordinate_system_encoder));
    // 0x0D - 13 - sensor_latitude
    Stanag::encoders.insert(std::make_pair(0x0D, encoders::stanag_sensor_latitude_encoder));
    // 0x0E - 14 - sensor_longitude
    Stanag::encoders.insert(std::make_pair(0x0E, encoders::stanag_sensor_longitude_encoder));
    // 0x0F - 15 - sensor_true_altitude
    Stanag::encoders.insert(std::make_pair(0x0F, encoders::stanag_sensor_true_altitude_encoder));
    // 0x10 - 16 - sensor_horizontal_field_of_view
    Stanag::encoders.insert(std::make_pair(0x10, encoders::stanag_sensor_horizontal_field_of_view_encoder));
    // 0x11 - 17 - sensor_vertical_field_of_view
    Stanag::encoders.insert(std::make_pair(0x11, encoders::stanag_sensor_vertical_field_of_view_encoder));
    // 0x12 - 18 - sensor_relative_azimuth_angle
    Stanag::encoders.insert(std::make_pair(0x12, encoders::stanag_sensor_relative_azimuth_angle_encoder));
    // 0x13 - 19 - sensor_relative_elevation_angle
    Stanag::encoders.insert(std::make_pair(0x13, encoders::stanag_sensor_relative_elevation_angle_encoder));
    // 0x14 - 20 - sensor_relative_roll_angle
    Stanag::encoders.insert(std::make_pair(0x14, encoders::stanag_sensor_relative_roll_angle_encoder));
    // 0x15 - 21 - slant_range
    Stanag::encoders.insert(std::make_pair(0x15, encoders::stanag_slant_range_encoder));
    // 0x16 - 22 - target_width
    Stanag::encoders.insert(std::make_pair(0x16, encoders::stanag_target_width_encoder));
    // 0x17 - 23 - frame_center_latitude
    Stanag::encoders.insert(std::make_pair(0x17, encoders::stanag_frame_center_latitude_encoder));
    // 0x18 - 24 - frame_center_longitude
    Stanag::encoders.insert(std::make_pair(0x18, encoders::stanag_frame_center_longitude_encoder));
    // 0x19 - 25 - frame_center_elevation
    Stanag::encoders.insert(std::make_pair(0x19, encoders::stanag_frame_center_elevation_encoder));
    // 0x1A - 26 - offset_corner_latitude_point_1
    Stanag::encoders.insert(std::make_pair(0x1A, encoders::stanag_offset_corner_latitude_point_1_encoder));
    // 0x1B - 27 - offset_corner_longitude_point_1
    Stanag::encoders.insert(std::make_pair(0x1B, encoders::stanag_offset_corner_longitude_point_1_encoder));
    // 0x1C - 28 - offset_corner_latitude_point_2
    Stanag::encoders.insert(std::make_pair(0x1C, encoders::stanag_offset_corner_latitude_point_2_encoder));
    // 0x1D - 29 - offset_corner_longitude_point_2
    Stanag::encoders.insert(std::make_pair(0x1D, encoders::stanag_offset_corner_longitude_point_2_encoder));
    // 0x1E - 30 - offset_corner_latitude_point_3
    Stanag::encoders.insert(std::make_pair(0x1E, encoders::stanag_offset_corner_latitude_point_3_encoder));
    // 0x1F - 31 - offset_corner_longitude_point_3
    Stanag::encoders.insert(std::make_pair(0x1F, encoders::stanag_offset_corner_longitude_point_3_encoder));
    // 0x20 - 32 - offset_corner_latitude_point_4
    Stanag::encoders.insert(std::make_pair(0x20, encoders::stanag_offset_corner_latitude_point_4_encoder));
    // 0x21 - 33 - offset_corner_longitude_point_4
    Stanag::encoders.insert(std::make_pair(0x21, encoders::stanag_offset_corner_longitude_point_4_encoder));
    // 0x22 - 34 - icing_detected
    Stanag::encoders.insert(std::make_pair(0x22, encoders::stanag_icing_detected_encoder));
    // 0x23 - 35 - wind_direction
    Stanag::encoders.insert(std::make_pair(0x23, encoders::stanag_wind_direction_encoder));
    // 0x24 - 36 - wind_speed
    Stanag::encoders.insert(std::make_pair(0x24, encoders::stanag_wind_speed_encoder));
    // 0x25 - 37 - static_pressure
    Stanag::encoders.insert(std::make_pair(0x25, encoders::stanag_static_pressure_encoder));
    // 0x26 - 38 - density_altitude
    Stanag::encoders.insert(std::make_pair(0x26, encoders::stanag_density_altitude_encoder));
    // 0x27 - 39 - outside_air_temperature
    Stanag::encoders.insert(std::make_pair(0x27, encoders::stanag_outside_air_temperature_encoder));
    // 0x28 - 40 - target_location_latitude
    Stanag::encoders.insert(std::make_pair(0x28, encoders::stanag_target_location_latitude_encoder));
    // 0x29 - 41 - target_location_longitude
    Stanag::encoders.insert(std::make_pair(0x29, encoders::stanag_target_location_longitude_encoder));
    // 0x2A - 42 - target_location_elevation
    Stanag::encoders.insert(std::make_pair(0x2A, encoders::stanag_target_location_elevation_encoder));
    // 0x2B - 43 - target_track_gate_width
    Stanag::encoders.insert(std::make_pair(0x2B, encoders::stanag_target_track_gate_width_encoder));
    // 0x2C - 44 - target_track_gate_height
    Stanag::encoders.insert(std::make_pair(0x2C, encoders::stanag_target_track_gate_height_encoder));
    // 0x2D - 45 - target_error_estimate_ce90
    Stanag::encoders.insert(std::make_pair(0x2D, encoders::stanag_target_error_estimate_ce90_encoder));
    // 0x2E - 46 - target_error_estimate_le90
    Stanag::encoders.insert(std::make_pair(0x2E, encoders::stanag_target_error_estimate_le90_encoder));
    // 0x2F - 47 - generic_flag_data
    Stanag::encoders.insert(std::make_pair(0x2F, encoders::stanag_generic_flag_data_encoder));
    // 0x30 - 48 - security_local_set
    Stanag::encoders.insert(std::make_pair(0x30, encoders::stanag_security_local_set_encoder));
    // 0x31 - 49 - differential_pressure
    Stanag::encoders.insert(std::make_pair(0x31, encoders::stanag_differential_pressure_encoder));
    // 0x32 - 50 - platform_angle_of_attack
    Stanag::encoders.insert(std::make_pair(0x32, encoders::stanag_platform_angle_of_attack_encoder));
    // 0x33 - 51 - platform_vertical_speed
    Stanag::encoders.insert(std::make_pair(0x33, encoders::stanag_platform_vertical_speed_encoder));
    // 0x34 - 52 - platform_sideslip_angle
    Stanag::encoders.insert(std::make_pair(0x34, encoders::stanag_platform_sideslip_angle_encoder));
    // 0x35 - 53 - airfield_barometric_pressure
    Stanag::encoders.insert(std::make_pair(0x35, encoders::stanag_airfield_barometric_pressure_encoder));
    // 0x36 - 54 - airfield_elevation
    Stanag::encoders.insert(std::make_pair(0x36, encoders::stanag_airfield_elevation_encoder));
    // 0x37 - 55 - relative_humidity
    Stanag::encoders.insert(std::make_pair(0x37, encoders::stanag_relative_humidity_encoder));
    // 0x38 - 56 - platform_ground_speed
    Stanag::encoders.insert(std::make_pair(0x38, encoders::stanag_platform_ground_speed_encoder));
    // 0x39 - 57 - ground_range
    Stanag::encoders.insert(std::make_pair(0x39, encoders::stanag_ground_range_encoder));
    // 0x3A - 58 - platform_fuel_remaining
    Stanag::encoders.insert(std::make_pair(0x3A, encoders::stanag_platform_fuel_remaining_encoder));
    // 0x3B - 59 - platform_call_sign
    Stanag::encoders.insert(std::make_pair(0x3B, encoders::stanag_platform_call_sign_encoder));
    // 0x3C - 60 - weapon_load
    Stanag::encoders.insert(std::make_pair(0x3C, encoders::stanag_weapon_load_encoder));
    // 0x3D - 61 - weapon_fired
    Stanag::encoders.insert(std::make_pair(0x3D, encoders::stanag_weapon_fired_encoder));
    // 0x3E - 62 - laser_prf_code
    Stanag::encoders.insert(std::make_pair(0x3E, encoders::stanag_laser_prf_code_encoder));
    // 0x3F - 63 - sensor_field_of_view_name
    Stanag::encoders.insert(std::make_pair(0x3F, encoders::stanag_sensor_field_of_view_name_encoder));
    // 0x40 - 64 - platform_magnetic_heading
    Stanag::encoders.insert(std::make_pair(0x40, encoders::stanag_platform_magnetic_heading_encoder));
    // 0x41 - 65 - uas_datalink_ls_version_number
    Stanag::encoders.insert(std::make_pair(0x41, encoders::stanag_uas_datalink_ls_version_number_encoder));

    // // // 0x42 - 66 - deprecated

    // 0x42 - 66 - target_location_covariance_matrix
    Stanag::encoders.insert(std::make_pair(0x42, encoders::stanag_target_location_covariance_matrix_encoder));
    // 0x43 - 67 - alternate_platform_latitude
    Stanag::encoders.insert(std::make_pair(0x43, encoders::stanag_alternate_platform_latitude_encoder));
    // 0x44 - 68 - alternate_platform_longitude
    Stanag::encoders.insert(std::make_pair(0x44, encoders::stanag_alternate_platform_longitude_encoder));
    // 0x45 - 69 - alternate_platform_altitude
    Stanag::encoders.insert(std::make_pair(0x45, encoders::stanag_alternate_platform_altitude_encoder));
    // 0x46 - 70 - alternate_platform_name
    Stanag::encoders.insert(std::make_pair(0x46, encoders::stanag_alternate_platform_name_encoder));
    // 0x47 - 71 - alternate_platform_heading
    Stanag::encoders.insert(std::make_pair(0x47, encoders::stanag_alternate_platform_heading_encoder));
    // 0x48 - 72 - event_start_time_utc
    Stanag::encoders.insert(std::make_pair(0x48, encoders::stanag_event_start_time_utc_encoder));
    // 0x49 - 73 - rvt_local_set
    Stanag::encoders.insert(std::make_pair(0x49, encoders::stanag_rvt_local_set_encoder));
    // 0x4A - 74 - vmti_local_set
    Stanag::encoders.insert(std::make_pair(0x4A, encoders::stanag_vmti_local_set_encoder));
    // 0x4B - 75 - sensor_ellipsoid_height
    Stanag::encoders.insert(std::make_pair(0x4B, encoders::stanag_sensor_ellipsoid_height_encoder));
    // 0x4C - 76 - alternate_platform_ellipsoid_height
    Stanag::encoders.insert(std::make_pair(0x4C, encoders::stanag_alternate_platform_ellipsoid_height_encoder));
    // 0x4D - 77 - operational_mode
    Stanag::encoders.insert(std::make_pair(0x4D, encoders::stanag_operational_mode_encoder));
    // 0x4E - 78 - frame_center_height_above_ellipsoid
    Stanag::encoders.insert(std::make_pair(0x4E, encoders::stanag_frame_center_height_above_ellipsoid_encoder));
    // 0x4F - 79 - sensor_north_velocity
    Stanag::encoders.insert(std::make_pair(0x4F, encoders::stanag_sensor_north_velocity_encoder));
    // 0x50 - 80 - sensor_east_velocity
    Stanag::encoders.insert(std::make_pair(0x50, encoders::stanag_sensor_east_velocity_encoder));
    // 0x51 - 81 - image_horizon_pixel_pack
    Stanag::encoders.insert(std::make_pair(0x51, encoders::stanag_image_horizon_pixel_pack_encoder));
    // 0x52 - 82 - corner_latitude_point_1
    Stanag::encoders.insert(std::make_pair(0x52, encoders::stanag_corner_latitude_point_1_encoder));
    // 0x53 - 83 - corner_longitude_point_1
    Stanag::encoders.insert(std::make_pair(0x53, encoders::stanag_corner_longitude_point_1_encoder));
    // 0x54 - 84 - corner_latitude_point_2
    Stanag::encoders.insert(std::make_pair(0x54, encoders::stanag_corner_latitude_point_2_encoder));
    // 0x55 - 85 - corner_longitude_point_2
    Stanag::encoders.insert(std::make_pair(0x55, encoders::stanag_corner_longitude_point_2_encoder));
    // 0x56 - 86 - corner_latitude_point_3
    Stanag::encoders.insert(std::make_pair(0x56, encoders::stanag_corner_latitude_point_3_encoder));
    // 0x57 - 87 - corner_longitude_point_3
    Stanag::encoders.insert(std::make_pair(0x57, encoders::stanag_corner_longitude_point_3_encoder));
    // 0x58 - 88 - corner_latitude_point_4
    Stanag::encoders.insert(std::make_pair(0x58, encoders::stanag_corner_latitude_point_4_encoder));
    // 0x59 - 89 - corner_longitude_point_4
    Stanag::encoders.insert(std::make_pair(0x59, encoders::stanag_corner_longitude_point_4_encoder));
    // 0x5A - 90 - platform_pitch_angle_full
    Stanag::encoders.insert(std::make_pair(0x5A, encoders::stanag_platform_pitch_angle_full_encoder));
    // 0x5B - 91 - platform_roll_angle_full
    Stanag::encoders.insert(std::make_pair(0x5B, encoders::stanag_platform_roll_angle_full_encoder));
    // 0x5C - 92 - platform_angle_of_attack_full
    Stanag::encoders.insert(std::make_pair(0x5C, encoders::stanag_platform_angle_of_attack_full_encoder));
    // 0x5D - 93 - platform_sideslip_angle_full
    Stanag::encoders.insert(std::make_pair(0x5D, encoders::stanag_platform_sideslip_angle_full_encoder));
    // 0x5E - 94 - miis_core_identifier
    Stanag::encoders.insert(std::make_pair(0x5E, encoders::stanag_miis_core_identifier_encoder));
    // // 0x5F - 95 - sar_motion_imagery_local_set
    // Stanag::encoders.insert(std::make_pair(0x5F, encoders::stanag_sar_motion_imagery_local_set_encoder));
    // // 0x60 - 96 - target_width_extended
    // Stanag::encoders.insert(std::make_pair(0x60, encoders::stanag_target_width_extended_encoder));
    // // 0x61 - 97 - range_image_local_set
    // Stanag::encoders.insert(std::make_pair(0x61, encoders::stanag_range_image_local_set_encoder));
    // // 0x62 - 98 - geo_registration_local_set
    // Stanag::encoders.insert(std::make_pair(0x62, encoders::stanag_geo_registration_local_set_encoder));
    // // 0x63 - 99 - composite_imaging_local_set
    // Stanag::encoders.insert(std::make_pair(0x63, encoders::stanag_composite_imaging_local_set_encoder));
    // // 0x64 - 100 - segment_local_set
    Stanag::encoders.insert(std::make_pair(0x64, encoders::stanag_segment_local_set_encoder));
    // 0x65 - 101 - amend_local_set
    Stanag::encoders.insert(std::make_pair(0x65, encoders::stanag_amend_local_set_encoder));
    // // 0x66 - 102 - sdcc_flp
    // Stanag::encoders.insert(std::make_pair(0x66, encoders::stanag_sdcc_flp_encoder));
    // // 0x67 - 103 - density_altitude_extended
    // Stanag::encoders.insert(std::make_pair(0x67, encoders::stanag_density_altitude_extended_encoder));
    // // 0x68 - 104 - sensor_ellipsoid_height_extended
    // Stanag::encoders.insert(std::make_pair(0x68, encoders::stanag_sensor_ellipsoid_height_extended_encoder));
    // // 0x69 - 105 - alternate_platform_ellipsoid_height_extended
    // Stanag::encoders.insert(std::make_pair(0x69, encoders::stanag_alternate_platform_ellipsoid_height_extended_encoder));
    // // 0x6A - 106 - stream_designator
    // Stanag::encoders.insert(std::make_pair(0x6A, encoders::stanag_stream_designator_encoder));
    // // 0x6B - 107 - operational_base
    // Stanag::encoders.insert(std::make_pair(0x6B, encoders::stanag_operational_base_encoder));
    // // 0x6C - 108 - broadcast_source
    // Stanag::encoders.insert(std::make_pair(0x6C, encoders::stanag_broadcast_source_encoder));
    // // 0x6D - 109 - range_to_recovery_location
    // Stanag::encoders.insert(std::make_pair(0x6D, encoders::stanag_range_to_recovery_location_encoder));
    // // 0x6E - 110 - time_airborne
    // Stanag::encoders.insert(std::make_pair(0x6E, encoders::stanag_time_airborne_encoder));
    // // 0x6F - 111 - propulsion_unit_speed
    // Stanag::encoders.insert(std::make_pair(0x6F, encoders::stanag_propulsion_unit_speed_encoder));
    // // 0x70 - 112 - platform_course_angle
    // Stanag::encoders.insert(std::make_pair(0x70, encoders::stanag_platform_course_angle_encoder));
    // // 0x71 - 113 - altitude_agl
    // Stanag::encoders.insert(std::make_pair(0x71, encoders::stanag_altitude_agl_encoder));
    // // 0x72 - 114 - radar_altimeter
    // Stanag::encoders.insert(std::make_pair(0x72, encoders::stanag_radar_altimeter_encoder));
    // // 0x73 - 115 - control_command
    // Stanag::encoders.insert(std::make_pair(0x73, encoders::stanag_control_command_encoder));
    // // 0x74 - 116 - control_command_verification_list
    // Stanag::encoders.insert(std::make_pair(0x74, encoders::stanag_control_command_verification_list_encoder));
    // // 0x75 - 117 - sensor_azimuth_rate
    // Stanag::encoders.insert(std::make_pair(0x75, encoders::stanag_sensor_azimuth_rate_encoder));
    // // 0x76 - 118 - sensor_elevation_rate
    // Stanag::encoders.insert(std::make_pair(0x76, encoders::stanag_sensor_elevation_rate_encoder));
    // // 0x77 - 119 - sensor_roll_rate
    // Stanag::encoders.insert(std::make_pair(0x77, encoders::stanag_sensor_roll_rate_encoder));
    // // 0x78 - 120 - onboard_mi_storage_percent_full
    // Stanag::encoders.insert(std::make_pair(0x78, encoders::stanag_onboard_mi_storage_percent_full_encoder));
    // // 0x79 - 121 - active_wavelength_list
    // Stanag::encoders.insert(std::make_pair(0x79, encoders::stanag_active_wavelength_list_encoder));
    // // 0x7A - 122 - country_codes
    // Stanag::encoders.insert(std::make_pair(0x7A, encoders::stanag_country_codes_encoder));
    // // 0x7B - 123 - number_of_navsats_in_view
    // Stanag::encoders.insert(std::make_pair(0x7B, encoders::stanag_number_of_navsats_in_view_encoder));
    // // 0x7C - 124 - positioning_method_source
    // Stanag::encoders.insert(std::make_pair(0x7C, encoders::stanag_positioning_method_source_encoder));
    // // 0x7D - 125 - platform_status
    // Stanag::encoders.insert(std::make_pair(0x7D, encoders::stanag_platform_status_encoder));
    // // 0x7E - 126 - sensor_control_mode
    // Stanag::encoders.insert(std::make_pair(0x7E, encoders::stanag_sensor_control_mode_encoder));
    // // 0x7F - 127 - sensor_frame_rate_pack
    // Stanag::encoders.insert(std::make_pair(0x7F, encoders::stanag_sensor_frame_rate_pack_encoder));
    // // 0x80 - 128 - wavelengths_list
    // Stanag::encoders.insert(std::make_pair(0x80, encoders::stanag_wavelengths_list_encoder));
    // // 0x81 - 129 - target_id
    // Stanag::encoders.insert(std::make_pair(0x81, encoders::stanag_target_id_encoder));
    // // 0x82 - 130 - airbase_locations
    // Stanag::encoders.insert(std::make_pair(0x82, encoders::stanag_airbase_locations_encoder));
    // // 0x83 - 131 - takeoff_time
    // Stanag::encoders.insert(std::make_pair(0x83, encoders::stanag_takeoff_time_encoder));
    // // 0x84 - 132 - transmission_frequency
    // Stanag::encoders.insert(std::make_pair(0x84, encoders::stanag_transmission_frequency_encoder));
    // // 0x85 - 133 - onboard_mi_storage_capacity
    // Stanag::encoders.insert(std::make_pair(0x85, encoders::stanag_onboard_mi_storage_capacity_encoder));
    // // 0x86 - 134 - zoom_percentage
    // Stanag::encoders.insert(std::make_pair(0x86, encoders::stanag_zoom_percentage_encoder));
    // // 0x87 - 135 - communications_method
    // Stanag::encoders.insert(std::make_pair(0x87, encoders::stanag_communications_method_encoder));
    // // 0x88 - 136 - leap_seconds
    // Stanag::encoders.insert(std::make_pair(0x88, encoders::stanag_leap_seconds_encoder));
    // // 0x89 - 137 - correction_offset
    // Stanag::encoders.insert(std::make_pair(0x89, encoders::stanag_correction_offset_encoder));
    // // 0x8A - 138 - payload_list
    // Stanag::encoders.insert(std::make_pair(0x8A, encoders::stanag_payload_list_encoder));
    // // 0x8B - 139 - active_payloads
    // Stanag::encoders.insert(std::make_pair(0x8B, encoders::stanag_active_payloads_encoder));
    // // 0x8C - 140 - weapons_stores
    // Stanag::encoders.insert(std::make_pair(0x8C, encoders::stanag_weapons_stores_encoder));
    // // 0x8D - 141 - waypoint_list
    // Stanag::encoders.insert(std::make_pair(0x8D, encoders::stanag_waypoint_list_encoder));
    // // 0x8E - 142 - view_domain
    // Stanag::encoders.insert(std::make_pair(0x8E, encoders::stanag_view_domain_encoder));
}

void Stanag::init_pretty() {
    if (Stanag::prettys.size() > 0)
        return;
    // 0x02 - 2 - precision_time_stamp
    Stanag::prettys.insert(std::make_pair(0x02, pretty::timestamp_pretty));

    // 0x0D - 13 - sensor_latitude
    Stanag::prettys.insert(std::make_pair(0x0D, pretty::latitude_pretty));
    // 0x0E - 14 - sensor_longitude
    Stanag::prettys.insert(std::make_pair(0x0E, pretty::longitude_pretty));

    // 0x17 - 23 - frame_center_latitude
    Stanag::prettys.insert(std::make_pair(0x17, pretty::latitude_pretty));
    // 0x18 - 24 - frame_center_longitude
    Stanag::prettys.insert(std::make_pair(0x18, pretty::longitude_pretty));

    // 0x1A - 26 - offset_corner_latitude_point_1
    Stanag::prettys.insert(std::make_pair(0x1A, pretty::latitude_pretty));
    // 0x1B - 27 - offset_corner_longitude_point_1
    Stanag::prettys.insert(std::make_pair(0x1B, pretty::longitude_pretty));
    // 0x1C - 28 - offset_corner_latitude_point_2
    Stanag::prettys.insert(std::make_pair(0x1C, pretty::latitude_pretty));
    // 0x1D - 29 - offset_corner_longitude_point_2
    Stanag::prettys.insert(std::make_pair(0x1D, pretty::longitude_pretty));
    // 0x1E - 30 - offset_corner_latitude_point_3
    Stanag::prettys.insert(std::make_pair(0x1E, pretty::latitude_pretty));
    // 0x1F - 31 - offset_corner_longitude_point_3
    Stanag::prettys.insert(std::make_pair(0x1F, pretty::longitude_pretty));
    // 0x20 - 32 - offset_corner_latitude_point_4
    Stanag::prettys.insert(std::make_pair(0x20, pretty::latitude_pretty));
    // 0x21 - 33 - offset_corner_longitude_point_4
    Stanag::prettys.insert(std::make_pair(0x21, pretty::longitude_pretty));

    // 0x28 - 40 - target_location_latitude
    Stanag::prettys.insert(std::make_pair(0x28, pretty::latitude_pretty));
    // 0x29 - 41 - target_location_longitude
    Stanag::prettys.insert(std::make_pair(0x29, pretty::longitude_pretty));

    // 0x43 - 67 - alternate_platform_latitude
    Stanag::prettys.insert(std::make_pair(0x43, pretty::latitude_pretty));
    // 0x44 - 68 - alternate_platform_longitude
    Stanag::prettys.insert(std::make_pair(0x44, pretty::longitude_pretty));
}

void Stanag::init_ugly() {
    if (Stanag::uglys.size() > 0)
        return;
    // 0x02 - 2 - precision_time_stamp
    Stanag::uglys.insert(std::make_pair(0x02, ugly::timestamp_ugly));

    // 0x0D - 13 - sensor_latitude
    Stanag::uglys.insert(std::make_pair(0x0D, ugly::latitude_ugly));
    // 0x0E - 14 - sensor_longitude
    Stanag::uglys.insert(std::make_pair(0x0E, ugly::longitude_ugly));

    // 0x17 - 23 - frame_center_latitude
    Stanag::uglys.insert(std::make_pair(0x17, ugly::latitude_ugly));
    // 0x18 - 24 - frame_center_longitude
    Stanag::uglys.insert(std::make_pair(0x18, ugly::longitude_ugly));

    // 0x1A - 26 - offset_corner_latitude_point_1
    Stanag::uglys.insert(std::make_pair(0x1A, ugly::latitude_ugly));
    // 0x1B - 27 - offset_corner_longitude_point_1
    Stanag::uglys.insert(std::make_pair(0x1B, ugly::longitude_ugly));
    // 0x1C - 28 - offset_corner_latitude_point_2
    Stanag::uglys.insert(std::make_pair(0x1C, ugly::latitude_ugly));
    // 0x1D - 29 - offset_corner_longitude_point_2
    Stanag::uglys.insert(std::make_pair(0x1D, ugly::longitude_ugly));
    // 0x1E - 30 - offset_corner_latitude_point_3
    Stanag::uglys.insert(std::make_pair(0x1E, ugly::latitude_ugly));
    // 0x1F - 31 - offset_corner_longitude_point_3
    Stanag::uglys.insert(std::make_pair(0x1F, ugly::longitude_ugly));
    // 0x20 - 32 - offset_corner_latitude_point_4
    Stanag::uglys.insert(std::make_pair(0x20, ugly::latitude_ugly));
    // 0x21 - 33 - offset_corner_longitude_point_4
    Stanag::uglys.insert(std::make_pair(0x21, ugly::longitude_ugly));

    // 0x28 - 40 - target_location_latitude
    Stanag::uglys.insert(std::make_pair(0x28, ugly::latitude_ugly));
    // 0x29 - 41 - target_location_longitude
    Stanag::uglys.insert(std::make_pair(0x29, ugly::longitude_ugly));

    // 0x43 - 67 - alternate_platform_latitude
    Stanag::uglys.insert(std::make_pair(0x43, ugly::latitude_ugly));
    // 0x44 - 68 - alternate_platform_longitude
    Stanag::uglys.insert(std::make_pair(0x44, ugly::longitude_ugly));
}

#define __TAGS(number_tag, tag_number, number, string) \
    number_tag.insert(std::make_pair(number, string)); \
    tag_number.insert(std::make_pair(string, number))

void Stanag::init_tags() {
    if (number_tag.size() > 0 || tag_number.size() > 0)
        return;
    __TAGS(number_tag, tag_number, 0x01, "checksum");                        // 1
    __TAGS(number_tag, tag_number, 0x02, "precision_time_stamp");            // 2
    __TAGS(number_tag, tag_number, 0x03, "mission_id");                      // 3
    __TAGS(number_tag, tag_number, 0x04, "platform_tail_number");            // 4
    __TAGS(number_tag, tag_number, 0x05, "platform_heading_angle");          // 5
    __TAGS(number_tag, tag_number, 0x06, "platform_pitch_angle");            // 6
    __TAGS(number_tag, tag_number, 0x07, "platform_roll_angle");             // 7
    __TAGS(number_tag, tag_number, 0x08, "platform_true_airspeed");          // 8
    __TAGS(number_tag, tag_number, 0x09, "platform_indicated_airspeed");     // 9
    __TAGS(number_tag, tag_number, 0x0A, "platform_designation");            // 10
    __TAGS(number_tag, tag_number, 0x0B, "image_source_sensor");             // 11
    __TAGS(number_tag, tag_number, 0x0C, "image_coordinate_system");         // 12
    __TAGS(number_tag, tag_number, 0x0D, "sensor_latitude");                 // 13
    __TAGS(number_tag, tag_number, 0x0E, "sensor_longitude");                // 14
    __TAGS(number_tag, tag_number, 0x0F, "sensor_true_altitude");            // 15
    __TAGS(number_tag, tag_number, 0x10, "sensor_horizontal_field_of_view"); // 16
    __TAGS(number_tag, tag_number, 0x11, "sensor_vertical_field_of_view");   // 17
    __TAGS(number_tag, tag_number, 0x12, "sensor_relative_azimuth_angle");   // 18
    __TAGS(number_tag, tag_number, 0x13, "sensor_relative_elevation_angle"); // 19
    __TAGS(number_tag, tag_number, 0x14, "sensor_relative_roll_angle");      // 20
    __TAGS(number_tag, tag_number, 0x15, "slant_range");                     // 21
    __TAGS(number_tag, tag_number, 0x16, "target_width");                    // 22
    __TAGS(number_tag, tag_number, 0x17, "frame_center_latitude");           // 23
    __TAGS(number_tag, tag_number, 0x18, "frame_center_longitude");          // 24
    __TAGS(number_tag, tag_number, 0x19, "frame_center_elevation");          // 25
    __TAGS(number_tag, tag_number, 0x1A, "offset_corner_latitude_point_1");  // 26
    __TAGS(number_tag, tag_number, 0x1B, "offset_corner_longitude_point_1"); // 27
    __TAGS(number_tag, tag_number, 0x1C, "offset_corner_latitude_point_2");  // 28
    __TAGS(number_tag, tag_number, 0x1D, "offset_corner_longitude_point_2"); // 29
    __TAGS(number_tag, tag_number, 0x1E, "offset_corner_latitude_point_3");  // 30
    __TAGS(number_tag, tag_number, 0x1F, "offset_corner_longitude_point_3"); // 31
    __TAGS(number_tag, tag_number, 0x20, "offset_corner_latitude_point_4");  // 32
    __TAGS(number_tag, tag_number, 0x21, "offset_corner_longitude_point_4"); // 33
    __TAGS(number_tag, tag_number, 0x22, "icing_detected");                  // 34
    __TAGS(number_tag, tag_number, 0x23, "wind_direction");                  // 35
    __TAGS(number_tag, tag_number, 0x24, "wind_speed");                      // 36
    __TAGS(number_tag, tag_number, 0x25, "static_pressure");                 // 37
    __TAGS(number_tag, tag_number, 0x26, "density_altitude");                // 38
    __TAGS(number_tag, tag_number, 0x27, "outside_air_temperature");         // 39
    __TAGS(number_tag, tag_number, 0x28, "target_location_latitude");        // 40
    __TAGS(number_tag, tag_number, 0x29, "target_location_longitude");       // 41
    __TAGS(number_tag, tag_number, 0x2A, "target_location_elevation");       // 42
    __TAGS(number_tag, tag_number, 0x2B, "target_track_gate_width");         // 43
    __TAGS(number_tag, tag_number, 0x2C, "target_track_gate_height");        // 44
    __TAGS(number_tag, tag_number, 0x2D, "target_error_estimate_ce90");      // 45
    __TAGS(number_tag, tag_number, 0x2E, "target_error_estimate_le90");      // 46
    __TAGS(number_tag, tag_number, 0x2F, "generic_flag_data");               // 47
    __TAGS(number_tag, tag_number, 0x30, "security_local_set");              // 48
    __TAGS(number_tag, tag_number, 0x31, "differential_pressure");           // 49
    __TAGS(number_tag, tag_number, 0x32, "platform_angle_of_attack");        // 50
    __TAGS(number_tag, tag_number, 0x33, "platform_vertical_speed");         // 51
    __TAGS(number_tag, tag_number, 0x34, "platform_sideslip_angle");         // 52
    __TAGS(number_tag, tag_number, 0x35, "airfield_barometric_pressure");    // 53
    __TAGS(number_tag, tag_number, 0x36, "airfield_elevation");              // 54
    __TAGS(number_tag, tag_number, 0x37, "relative_humidity");               // 55
    __TAGS(number_tag, tag_number, 0x38, "platform_ground_speed");           // 56
    __TAGS(number_tag, tag_number, 0x39, "ground_range");                    // 57
    __TAGS(number_tag, tag_number, 0x3A, "platform_fuel_remaining");         // 58
    __TAGS(number_tag, tag_number, 0x3B, "platform_call_sign");              // 59
    __TAGS(number_tag, tag_number, 0x3C, "weapon_load");                     // 60
    __TAGS(number_tag, tag_number, 0x3D, "weapon_fired");                    // 61
    __TAGS(number_tag, tag_number, 0x3E, "laser_prf_code");                  // 62
    __TAGS(number_tag, tag_number, 0x3F, "sensor_field_of_view_name");       // 63
    __TAGS(number_tag, tag_number, 0x40, "platform_magnetic_heading");       // 64
    __TAGS(number_tag, tag_number, 0x41, "uas_datalink_ls_version_number");  // 65
    // //////////////// ? deprecated ? ///////////////////
    // __TAGS(number_tag, tag_number, 0x42, "deprecated");  // 66
    __TAGS(number_tag, tag_number, 0x42, "target_location_covariance_matrix");            // 66
    __TAGS(number_tag, tag_number, 0x43, "alternate_platform_latitude");                  // 67
    __TAGS(number_tag, tag_number, 0x44, "alternate_platform_longitude");                 // 68
    __TAGS(number_tag, tag_number, 0x45, "alternate_platform_altitude");                  // 69
    __TAGS(number_tag, tag_number, 0x46, "alternate_platform_name");                      // 70
    __TAGS(number_tag, tag_number, 0x47, "alternate_platform_heading");                   // 71
    __TAGS(number_tag, tag_number, 0x48, "event_start_time_utc");                         // 72
    __TAGS(number_tag, tag_number, 0x49, "rvt_local_set");                                // 73
    __TAGS(number_tag, tag_number, 0x4A, "vmti_local_set");                               // 74
    __TAGS(number_tag, tag_number, 0x4B, "sensor_ellipsoid_height");                      // 75
    __TAGS(number_tag, tag_number, 0x4C, "alternate_platform_ellipsoid_height");          // 76
    __TAGS(number_tag, tag_number, 0x4D, "operational_mode");                             // 77
    __TAGS(number_tag, tag_number, 0x4E, "frame_center_height_above_ellipsoid");          // 78
    __TAGS(number_tag, tag_number, 0x4F, "sensor_north_velocity");                        // 79
    __TAGS(number_tag, tag_number, 0x50, "sensor_east_velocity");                         // 80
    __TAGS(number_tag, tag_number, 0x51, "image_horizon_pixel_pack");                     // 81
    __TAGS(number_tag, tag_number, 0x52, "corner_latitude_point_1");                      // 82
    __TAGS(number_tag, tag_number, 0x53, "corner_longitude_point_1");                     // 83
    __TAGS(number_tag, tag_number, 0x54, "corner_latitude_point_2");                      // 84
    __TAGS(number_tag, tag_number, 0x55, "corner_longitude_point_2");                     // 85
    __TAGS(number_tag, tag_number, 0x56, "corner_latitude_point_3");                      // 86
    __TAGS(number_tag, tag_number, 0x57, "corner_longitude_point_3");                     // 87
    __TAGS(number_tag, tag_number, 0x58, "corner_latitude_point_4");                      // 88
    __TAGS(number_tag, tag_number, 0x59, "corner_longitude_point_4");                     // 89
    __TAGS(number_tag, tag_number, 0x5A, "platform_pitch_angle_full");                    // 90
    __TAGS(number_tag, tag_number, 0x5B, "platform_roll_angle_full");                     // 91
    __TAGS(number_tag, tag_number, 0x5C, "platform_angle_of_attack_full");                // 92
    __TAGS(number_tag, tag_number, 0x5D, "platform_sideslip_angle_full");                 // 93
    __TAGS(number_tag, tag_number, 0x5E, "miis_core_identifier");                         // 94
    __TAGS(number_tag, tag_number, 0x5F, "sar_motion_imagery_local_set");                 // 95
    __TAGS(number_tag, tag_number, 0x60, "target_width_extended");                        // 96
    __TAGS(number_tag, tag_number, 0x61, "range_image_local_set");                        // 97
    __TAGS(number_tag, tag_number, 0x62, "geo_registration_local_set");                   // 98
    __TAGS(number_tag, tag_number, 0x63, "composite_imaging_local_set");                  // 99
    __TAGS(number_tag, tag_number, 0x64, "segment_local_set");                            // 100
    __TAGS(number_tag, tag_number, 0x65, "amend_local_set");                              // 101
    __TAGS(number_tag, tag_number, 0x66, "sdcc_flp");                                     // 102
    __TAGS(number_tag, tag_number, 0x67, "density_altitude_extended");                    // 103
    __TAGS(number_tag, tag_number, 0x68, "sensor_ellipsoid_height_extended");             // 104
    __TAGS(number_tag, tag_number, 0x69, "alternate_platform_ellipsoid_height_extended"); // 105
    __TAGS(number_tag, tag_number, 0x6A, "stream_designator");                            // 106
    __TAGS(number_tag, tag_number, 0x6B, "operational_base");                             // 107
    __TAGS(number_tag, tag_number, 0x6C, "broadcast_source");                             // 108
    __TAGS(number_tag, tag_number, 0x6D, "range_to_recovery_location");                   // 109
    __TAGS(number_tag, tag_number, 0x6E, "time_airborne");                                // 110
    __TAGS(number_tag, tag_number, 0x6F, "propulsion_unit_speed");                        // 111
    __TAGS(number_tag, tag_number, 0x70, "platform_course_angle");                        // 112
    __TAGS(number_tag, tag_number, 0x71, "altitude_agl");                                 // 113
    __TAGS(number_tag, tag_number, 0x72, "radar_altimeter");                              // 114
    __TAGS(number_tag, tag_number, 0x73, "control_command");                              // 115
    __TAGS(number_tag, tag_number, 0x74, "control_command_verification_list");            // 116
    __TAGS(number_tag, tag_number, 0x75, "sensor_azimuth_rate");                          // 117
    __TAGS(number_tag, tag_number, 0x76, "sensor_elevation_rate");                        // 118
    __TAGS(number_tag, tag_number, 0x77, "sensor_roll_rate");                             // 119
    __TAGS(number_tag, tag_number, 0x78, "onboard_mi_storage_percent_full");              // 120
    __TAGS(number_tag, tag_number, 0x79, "active_wavelength_list");                       // 121
    __TAGS(number_tag, tag_number, 0x7A, "country_codes");                                // 122
    __TAGS(number_tag, tag_number, 0x7B, "number_of_navsats_in_view");                    // 123
    __TAGS(number_tag, tag_number, 0x7C, "positioning_method_source");                    // 124
    __TAGS(number_tag, tag_number, 0x7D, "platform_status");                              // 125
    __TAGS(number_tag, tag_number, 0x7E, "sensor_control_mode");                          // 126
    __TAGS(number_tag, tag_number, 0x7F, "sensor_frame_rate_pack");                       // 127
    __TAGS(number_tag, tag_number, 0x80, "wavelengths_list");                             // 128
    __TAGS(number_tag, tag_number, 0x81, "target_id");                                    // 129
    __TAGS(number_tag, tag_number, 0x82, "airbase_locations");                            // 130
    __TAGS(number_tag, tag_number, 0x83, "takeoff_time");                                 // 131
    __TAGS(number_tag, tag_number, 0x84, "transmission_frequency");                       // 132
    __TAGS(number_tag, tag_number, 0x85, "onboard_mi_storage_capacity");                  // 133
    __TAGS(number_tag, tag_number, 0x86, "zoom_percentage");                              // 134
    __TAGS(number_tag, tag_number, 0x87, "communications_method");                        // 135
    __TAGS(number_tag, tag_number, 0x88, "leap_seconds");                                 // 136
    __TAGS(number_tag, tag_number, 0x89, "correction_offset");                            // 137
    __TAGS(number_tag, tag_number, 0x8A, "payload_list");                                 // 138
    __TAGS(number_tag, tag_number, 0x8B, "active_payloads");                              // 139
    __TAGS(number_tag, tag_number, 0x8C, "weapons_stores");                               // 140
    __TAGS(number_tag, tag_number, 0x8D, "waypoint_list");                                // 141
    __TAGS(number_tag, tag_number, 0x8E, "view_domain");                                  // 142
}

#undef __TAGS

bool Stanag::is_strict() {
    if (this->ctx != nullptr) {
        return this->ctx->strict;
    }
    return false;
}

bool Stanag::stanag_segment_local_set_parser(ByteReader &reader, nl::json *res) {
    KLV klv;
    nl::json temp_key;
    nl::json temp_value;
    nl::json _res;
    // reader.skip(1); // #TODO this is related to bad content
    uint64_t last_remaining = 0;
    uint64_t remaining = 0;
    for (uint64_t i = 0;; i++) {
        remaining = reader.get_remaining();
        if (remaining <= 0) {
            break;
        } else if (remaining == last_remaining) {
            CTX_LOG(Level::error, "stuck");
            break;
        }
        last_remaining = remaining;
        uint64_t tag_value = 0;
        std::string tag_name = "";
        klv = KLV::parse(reader, PARSE_FORMATS::_BER_OID, PARSE_FORMATS::_BER);
        if (!get_tag(klv, &tag_value)) {
            CTX_LOG_PARSER(Level::error, "failed to get tag", &klv, tag_value, tag_name);
            continue;
        }
        if (!get_key_complex(tag_value, &temp_key)) {
            CTX_LOG_PARSER(Level::warn, "not registered", &klv, tag_value, tag_name);
            continue;
        }
        tag_name = temp_key.dump();
        // CTX_LOG_PARSER(Level::debug, klv.debug(), &klv, tag_value, tag_name);
        if (!get_value_complex(klv, tag_value, &temp_value)) {
            CTX_LOG_PARSER(Level::error, "parser failed", &klv, tag_value, tag_name);
            continue;
        }
        CTX_LOG_PARSER(Level::debug, nl::ensure_dump(temp_value), &klv, tag_value, tag_name);
        _res[temp_key.get<std::string>()] = temp_value;
    }
    *res = _res;
    return true;
}

bool Stanag::stanag_segment_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    // std::vector<uint8_t> _res(__TYPICAL_KLV_SIZE);
    std::vector<uint8_t> _res;
    try {
        for (auto const &[number, tag] : this->number_tag) {
            if (number == 1)
                continue; // ignore checksum tag, we'll handle this later
            auto j_pair = item.find(tag);
            if (j_pair == item.end()) {
                CTX_LOG_ENCODER(Level::debug, "tag missing", NULL, number, tag);
                continue;
            }
            const nl::json &j_value = j_pair.value();
            auto enc = encoders.find(number);
            if (enc == encoders.end()) {
                CTX_LOG_ENCODER(Level::warn, "encoder missing", &j_value, number, tag);
                continue;
            }
            std::vector<uint8_t> value_bytes;
            if (!enc->second(j_value, &value_bytes, ctx)) {
                CTX_LOG_ENCODER(Level::error, "encoder failed", NULL, number, tag);
                continue;
            }
            std::vector<uint8_t> tag_bytes;
            uint64_t n = number;
            if (!encoders::encode_ber_oid(n, &tag_bytes)) {
                CTX_LOG_ENCODER(Level::error, "failed to encode tag in ber oid", NULL, number, tag);
                continue;
            }
            std::vector<uint8_t> length_bytes;
            uint64_t l = value_bytes.size();
            if (!encoders::encode_ber(l, &length_bytes)) {
                if (ctx != nullptr) {
                    {
                        std::stringstream sstr;
                        sstr << "failed to encode length (" << l << ") in ber oid";
                        CTX_LOG_ENCODER(Level::error, sstr.str(), &j_value, number, tag);
                    }
                }
                continue;
            }
            _res.insert(_res.end(), tag_bytes.begin(), tag_bytes.end());
            _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
            _res.insert(_res.end(), value_bytes.begin(), value_bytes.end());
        }
        *res = std::move(_res);
        return true;
    } catch (std::exception &e) {
        CTX_LOG(Level::error, e.what());
        return false;
    }
    return false;
}

bool Stanag::stanag_amend_local_set_parser(ByteReader &reader, nl::json *res) {
    KLV klv;
    nl::json temp_key;
    nl::json temp_value;
    nl::json _res;
    uint64_t last_remaining = 0;
    uint64_t remaining = 0;
    for (uint64_t i = 0;; i++) {
        remaining = reader.get_remaining();
        if (remaining <= 0) {
            break;
        } else if (remaining == last_remaining) {
            CTX_LOG(Level::error, "stuck");
            break;
        }
        last_remaining = remaining;
        uint64_t tag_value = 0;
        std::string tag_name = "";
        klv = KLV::parse(reader, PARSE_FORMATS::_BER_OID, PARSE_FORMATS::_BER);
        if (!get_tag(klv, &tag_value)) {
            CTX_LOG_PARSER(Level::error, "failed to get tag", &klv, tag_value, tag_name);
            continue;
        }
        if (!get_key_complex(tag_value, &temp_key)) {
            CTX_LOG_PARSER(Level::warn, "not registered", &klv, tag_value, tag_name);
            continue;
        }
        tag_name = temp_key.dump();
        // CTX_LOG_PARSER(Level::debug, klv.debug(), &klv, tag_value, tag_name);
        if (!get_value_complex(klv, tag_value, &temp_value)) {
            CTX_LOG_PARSER(Level::error, "parser failed", &klv, tag_value, tag_name);
            continue;
        }
        CTX_LOG_PARSER(Level::debug, nl::ensure_dump(temp_value), &klv, tag_value, tag_name);
        _res[temp_key.get<std::string>()] = temp_value;
    }
    *res = _res;
    return true;
}

bool Stanag::stanag_amend_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    // std::vector<uint8_t> _res(__TYPICAL_KLV_SIZE);
    std::vector<uint8_t> _res;
    try {
        for (auto const &[number, tag] : this->number_tag) {
            if (number == 1)
                continue; // ignore checksum tag, we'll handle this later
            auto j_pair = item.find(tag);
            if (j_pair == item.end()) {
                CTX_LOG_ENCODER(Level::debug, "tag missing", NULL, number, tag);
                continue;
            }
            const nl::json &j_value = j_pair.value();
            auto enc = encoders.find(number);
            if (enc == encoders.end()) {
                CTX_LOG_ENCODER(Level::warn, "encoder missing", &j_value, number, tag);
                continue;
            }
            std::vector<uint8_t> value_bytes;
            if (!enc->second(j_value, &value_bytes, ctx)) {
                CTX_LOG_ENCODER(Level::error, "encoder failed", NULL, number, tag);
                continue;
            }
            std::vector<uint8_t> tag_bytes;
            uint64_t n = number;
            if (!encoders::encode_ber_oid(n, &tag_bytes)) {
                CTX_LOG_ENCODER(Level::error, "failed to encode tag in ber oid", NULL, number, tag);
                continue;
            }
            std::vector<uint8_t> length_bytes;
            uint64_t l = value_bytes.size();
            if (!encoders::encode_ber(l, &length_bytes)) {
                if (ctx != nullptr) {
                    {
                        std::stringstream sstr;
                        sstr << "failed to encode length (" << l << ") in ber oid";
                        CTX_LOG_ENCODER(Level::error, sstr.str(), &j_value, number, tag);
                    }
                }
                continue;
            }
            _res.insert(_res.end(), tag_bytes.begin(), tag_bytes.end());
            _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
            _res.insert(_res.end(), value_bytes.begin(), value_bytes.end());
        }
        *res = std::move(_res);
        return true;
    } catch (std::exception &e) {
        CTX_LOG(Level::error, e.what());
        return false;
    }
    return false;
}

bool parsers::stanag_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("stanag") : nullptr);
    return stanag.parse(reader, res);
}

bool encoders::stanag_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("stanag") : nullptr);
    return stanag.encode(item, res);
}

bool parsers::stanag_segment_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("segment") : nullptr);
    return stanag.stanag_segment_local_set_parser(reader, res);
    // return false;
}

bool encoders::stanag_segment_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("segment") : nullptr);
    return stanag.stanag_segment_local_set_encoder(item, res);
}

bool parsers::stanag_amend_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("amend") : nullptr);
    return stanag.stanag_amend_local_set_parser(reader, res);
}

bool encoders::stanag_amend_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    Stanag stanag(ctx != nullptr ? ctx->sub("amend") : nullptr);
    return stanag.stanag_amend_local_set_encoder(item, res);
}