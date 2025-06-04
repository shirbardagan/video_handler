#include <klvparse/stanag/vmti/VMTI.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;
namespace encoders = klvparse::encoders;

// std::unordered_map<uint64_t, std::string> VMTI::vmti_number_tag;
std::map<uint64_t, std::string> VMTI::number_tag;
std::unordered_map<std::string, uint64_t> VMTI::tag_number;
// std::unordered_map<uint64_t, std::function<bool(KLV *, KLVStorage *)>> VMTI::parsers;
std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> VMTI::parsers;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> VMTI::encoders;

void VMTI::init() {
    init_tags();
    init_parsers();
    init_encoders();
}

VMTI::VMTI() {
    init();
    this->ctx = std::make_shared<Context>();
}

VMTI::VMTI(Context ctx) {
    init();
    this->ctx = std::make_shared<Context>(ctx);
}

VMTI::VMTI(std::weak_ptr<Context> ctx) {
    init();
    this->ctx = ctx.expired() ? nullptr : ctx.lock();
}

VMTI::VMTI(std::shared_ptr<Context> ctx) {
    init();
    this->ctx = ctx;
}

VMTI::~VMTI() {
}

bool VMTI::parse(const uint8_t *data, uint64_t size, nl::json *res, uint64_t skip) {
    std::vector<uint8_t> buffer(data, data + size);
    return parse(buffer, res, skip);
}

bool VMTI::parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip) {
    ByteReader reader(buffer);
    return parse(reader, res, skip);
}

bool VMTI::parse(ByteReader &reader, nl::json *res, uint64_t skip) {
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
        nl::json temp_key;
        nl::json temp_value;
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
            KLV klv = KLV::parse(reader, PARSE_FORMATS::_BER_OID, PARSE_FORMATS::_BER);
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

bool VMTI::verify_checksum(ByteReader &reader) {
    bool checksum_is_valid = false;
    uint16_t calculated = 0;
    uint16_t actual = 0;
    if (!_verify_checksum(reader, &checksum_is_valid, &calculated, &actual)) {
        CTX_LOG(Level::warn, "failed to calculate checksum, ignoring packet");
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

bool VMTI::encode(const nl::json &j, std::vector<uint8_t> *res) {
    std::vector<uint8_t> _res;
    try {
        for (auto const &[number, tag] : this->number_tag) {
            if (number == 1) {
                continue; // ignore checksum tag, we'll handle this later
            }
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
            if (!enc->second(j_value, &value_bytes, this->ctx)) {
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
            _res.insert(_res.end(), tag_bytes.begin(), tag_bytes.end());
            _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
            _res.insert(_res.end(), value_bytes.begin(), value_bytes.end());
        }
        { // checksum related - insert checksum tlv - tag 1, length 2,
            std::vector<uint8_t> checksum_klv_bytes = {0x01, 0x02, 0x00, 0x00};
            _res.insert(_res.end(), checksum_klv_bytes.begin(), checksum_klv_bytes.end());
        }
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
        //         STANAG_LOG(Level::debug, sstr.str());
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

bool VMTI::get_tag(KLV &item, uint64_t *res) {
    uint64_t tag;
    item.key.restart();
    if (!item.key.get_ber_oid(&tag)) {
        return false;
    }
    *res = tag;
    return true;
}

bool VMTI::get_key_complex(uint64_t tag, nl::json *res) {
    auto pair = this->number_tag.find(tag);
    if (pair != this->number_tag.end()) {
        *res = pair->second;
        return true;
    }
    return false;
}

bool VMTI::get_key(KLV &klv, nl::json *res) {
    uint64_t tag;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_key_complex(tag, res);
}

bool VMTI::get_value_complex(KLV &klv, uint64_t tag, nl::json *res) {
    auto pair = this->VMTI::parsers.find(tag);
    // item->value->restart();
    if (pair != this->VMTI::parsers.end()) {
        return pair->second(klv.value, res, this->ctx);
    }
    return false;
}

bool VMTI::get_value(KLV &klv, nl::json *res) {
    uint64_t tag = 0;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_value_complex(klv, tag, res);
}

void VMTI::init_parsers() {
    if (VMTI::parsers.size() > 0)
        return;

    // 0x01 - 1 - checksum
    VMTI::parsers.insert(std::make_pair(0x01, parsers::vmti_checksum_parser));
    // 0x02 - 2 - precision_time_stamp
    VMTI::parsers.insert(std::make_pair(0x02, parsers::vmti_precision_time_stamp_parser));
    // 0x03 - 3 - mission_id
    VMTI::parsers.insert(std::make_pair(0x03, parsers::vmti_system_name_parser));
    // 0x04 - 4 - ls_version
    VMTI::parsers.insert(std::make_pair(0x04, parsers::vmti_ls_version_parser));
    // 0x05 - 5 - number_detected
    VMTI::parsers.insert(std::make_pair(0x05, parsers::vmti_number_detected_parser));
    // 0x06 - 6 - number_reported
    VMTI::parsers.insert(std::make_pair(0x06, parsers::vmti_number_reported_parser));
    // 0x07 - 7 - frame_number
    VMTI::parsers.insert(std::make_pair(0x07, parsers::vmti_frame_number_parser));
    // 0x08 - 8 - frame_width
    VMTI::parsers.insert(std::make_pair(0x08, parsers::vmti_frame_width_parser));
    // 0x09 - 9 - frame_height
    VMTI::parsers.insert(std::make_pair(0x09, parsers::vmti_frame_height_parser));
    // 0x0a - 10 - source_sensor
    VMTI::parsers.insert(std::make_pair(0x0a, parsers::vmti_source_sensor_parser));
    // 0x0b - 11 - horizontal_fov
    VMTI::parsers.insert(std::make_pair(0x0b, parsers::vmti_horizontal_fov_parser));
    // 0x0c - 12 - vertical_fov
    VMTI::parsers.insert(std::make_pair(0x0c, parsers::vmti_vertical_fov_parser));
    // 0x0d - 13 - miis_id
    VMTI::parsers.insert(std::make_pair(0x0d, parsers::vmti_miis_id_parser));
    // 0x65 - 101 - target_series
    VMTI::parsers.insert(std::make_pair(0x65, parsers::vmti_target_series_parser));
    // 0x66 - 102 - algorithm_series
    VMTI::parsers.insert(std::make_pair(0x66, parsers::vmti_algorithm_series_parser));
    // 0x67 - 103 - ontology_series
    VMTI::parsers.insert(std::make_pair(0x67, parsers::vmti_ontology_series_parser));
}

void VMTI::init_encoders() {
    if (VMTI::encoders.size() > 0)
        return;

    // // 0x01 - 1 - checksum
    // VMTI::parsers.insert(std::make_pair(0x01, parsers::vmti_checksum_parser));
    // 0x02 - 2 - precision_time_stamp
    VMTI::encoders.insert(std::make_pair(0x02, encoders::vmti_precision_time_stamp_encoder));
    // 0x03 - 3 - mission_id
    VMTI::encoders.insert(std::make_pair(0x03, encoders::vmti_system_name_encoder));
    // 0x04 - 4 - ls_version
    VMTI::encoders.insert(std::make_pair(0x04, encoders::vmti_ls_version_encoder));
    // 0x05 - 5 - number_detected
    VMTI::encoders.insert(std::make_pair(0x05, encoders::vmti_number_detected_encoder));
    // 0x06 - 6 - number_reported
    VMTI::encoders.insert(std::make_pair(0x06, encoders::vmti_number_reported_encoder));
    // 0x07 - 7 - frame_number
    VMTI::encoders.insert(std::make_pair(0x07, encoders::vmti_frame_number_encoder));
    // 0x08 - 8 - frame_width
    VMTI::encoders.insert(std::make_pair(0x08, encoders::vmti_frame_width_encoder));
    // 0x09 - 9 - frame_height
    VMTI::encoders.insert(std::make_pair(0x09, encoders::vmti_frame_height_encoder));
    // 0x0a - 10 - source_sensor
    VMTI::encoders.insert(std::make_pair(0x0a, encoders::vmti_source_sensor_encoder));
    // 0x0b - 11 - horizontal_fov
    VMTI::encoders.insert(std::make_pair(0x0b, encoders::vmti_horizontal_fov_encoder));
    // 0x0c - 12 - vertical_fov
    VMTI::encoders.insert(std::make_pair(0x0c, encoders::vmti_vertical_fov_encoder));
    // 0x0d - 13 - miis_id
    VMTI::encoders.insert(std::make_pair(0x0d, encoders::vmti_miis_id_encoder));
    // 0x65 - 101 - target_series
    VMTI::encoders.insert(std::make_pair(0x65, encoders::vmti_target_series_encoder));
    // 0x66 - 102 - algorithm_series
    VMTI::encoders.insert(std::make_pair(0x65, encoders::vmti_algorithm_series_encoder));
    // 0x67 - 103 - ontology_series
    VMTI::encoders.insert(std::make_pair(0x65, encoders::vmti_ontology_series_encoder));
}
#define __TAGS(number_tag, tag_number, number, string) \
    number_tag.insert(std::make_pair(number, string)); \
    tag_number.insert(std::make_pair(string, number))

void VMTI::init_tags() {
    if (VMTI::number_tag.size() > 0 || VMTI::tag_number.size() > 0)
        return;
    __TAGS(number_tag, tag_number, 0x01, "checksum");             // 1
    __TAGS(number_tag, tag_number, 0x02, "precision_time_stamp"); // 2
    __TAGS(number_tag, tag_number, 0x03, "system_name");          // 3
    __TAGS(number_tag, tag_number, 0x04, "ls_version");           // 4
    __TAGS(number_tag, tag_number, 0x05, "number_detected");      // 5
    __TAGS(number_tag, tag_number, 0x06, "number_reported");      // 6
    __TAGS(number_tag, tag_number, 0x07, "frame_number");         // 7
    __TAGS(number_tag, tag_number, 0x08, "frame_width");          // 8
    __TAGS(number_tag, tag_number, 0x09, "frame_height");         // 9
    __TAGS(number_tag, tag_number, 0x0a, "source_sensor");        // 10
    __TAGS(number_tag, tag_number, 0x0b, "horizontal_fov");       // 11
    __TAGS(number_tag, tag_number, 0x0c, "vertical_fov");         // 12
    __TAGS(number_tag, tag_number, 0x0d, "miis_id");              // 13
    __TAGS(number_tag, tag_number, 0x65, "target_series");        // 101
    __TAGS(number_tag, tag_number, 0x66, "algorithm_series");     // 102
    __TAGS(number_tag, tag_number, 0x67, "ontology_series");      // 103
}

#undef __TAGS

bool VMTI::is_strict() {
    if (this->ctx != nullptr) {
        return this->ctx->strict;
    }
    return false;
}

bool parsers::vmti_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    VMTI vmti_parser(ctx != nullptr ? ctx->sub("vmti") : nullptr);
    return vmti_parser.parse(reader, res);
}
bool encoders::vmti_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    VMTI vmti_parser(ctx != nullptr ? ctx->sub("vmti") : nullptr);
    return vmti_parser.encode(item, res);
}