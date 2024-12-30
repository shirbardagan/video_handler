#include <klvparse/stanag/vmti/vtarget/VTarget.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;
namespace encoders = klvparse::encoders;

// std::unordered_map<uint64_t, std::string> VTarget::vmti_number_tag;
std::map<uint64_t, std::string> VTarget::number_tag;
std::unordered_map<std::string, uint64_t> VTarget::tag_number;
std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> VTarget::parsers;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> VTarget::encoders;

void VTarget::init() {
    init_tags();
    init_parsers();
    init_encoders();
}

VTarget::VTarget() {
    init();
    this->ctx = std::make_shared<Context>();
}

VTarget::VTarget(Context ctx) {
    init();
    this->ctx = std::make_shared<Context>(ctx);
}

VTarget::VTarget(std::weak_ptr<Context> ctx) {
    init();
    this->ctx = ctx.expired() ? nullptr : ctx.lock();
}

VTarget::VTarget(std::shared_ptr<Context> ctx) {
    init();
    this->ctx = ctx;
}

VTarget::~VTarget() {
}

bool VTarget::parse(const uint8_t *data, uint64_t size, nl::json *res, uint64_t skip) {
    std::vector<uint8_t> buffer(data, data + size);
    return parse(buffer, res, skip);
}

bool VTarget::parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip) {
    ByteReader reader(buffer);
    return parse(reader, res, skip);
}

bool VTarget::parse(ByteReader &reader, nl::json *res, uint64_t skip) {
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
        // reader.get_ber_oid()//////////////////////////////////////////////////////////////
        nl::json temp_key;
        nl::json temp_value;
        nl::json _res = nl::json::object();
        uint64_t id_value = 0;
        if (!reader.get_ber_oid(&id_value)) {
            return false;
        }
        _res[_VTARGET_ID_NUMBER] = id_value;
        // if (!parsers::variable_uint24_parser(reader, &temp_value)) {
        //     return false;
        // }
        // _res[_VTARGET_ID_NUMBER] = temp_value;
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
            uint64_t tag = 0;
            if (!get_tag(klv, &tag_value)) {
                CTX_LOG_PARSER(Level::error, "failed to get tag", &klv, tag_value, tag_name);
                continue;
            }
            // CTX_LOG_PARSER(Level::debug, klv.debug(), &klv, tag_value, tag_name);
            if (!get_key_complex(tag_value, &temp_key)) {
                CTX_LOG_PARSER(Level::warn, "not registered", &klv, tag_value, tag_name);
                continue;
            }
            tag_name = temp_key.dump();
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

bool VTarget::verify_checksum(ByteReader &reader) {
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

bool VTarget::encode(const nl::json &j, std::vector<uint8_t> *res) {
    std::vector<uint8_t> _res;
    try {
        {
            std::vector<uint8_t> id_bytes;
            auto id_pair = j.find(_VTARGET_ID);
            if (id_pair == j.end()) {
                CTX_LOG(Level::error, "required \"" _VTARGET_ID "\" is missing");
                return false;
            }
            const nl::json &id_json = id_pair.value();
            if (!id_json.is_number()) {
                CTX_LOG_ENCODER(Level::error, "should be a number", &id_json, 0, _VTARGET_ID);
                return false;
            }
            uint64_t id_value = id_json.get<uint64_t>();
            if (!encoders::encode_ber_oid(id_value, &id_bytes)) {
                CTX_LOG_ENCODER(Level::error, "failed to encode as ber-oid", &id_json, 0, _VTARGET_ID);
                return false;
            }
            if (id_bytes.size() > 3) {
                {
                    std::stringstream sstr;
                    sstr << "failed to encode as ber-oid, result was longer than 3 bytes:\n"
                         << ByteReader(id_bytes).dump();
                    CTX_LOG_ENCODER(Level::error, sstr.str(), &id_json, 0, _VTARGET_ID);
                }
                return false;
            }
            _res.insert(_res.end(), id_bytes.begin(), id_bytes.end());
        }
        for (auto const &[number, tag] : this->number_tag) {
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
        *res = std::move(_res);
        return true;
    } catch (std::exception &e) {
        CTX_LOG(Level::error, e.what());
        return false;
    }
    return false;
}

bool VTarget::get_tag(KLV &klv, uint64_t *res) {
    uint64_t tag;
    klv.key.restart();
    if (!klv.key.get_ber_oid(&tag)) {
        return false;
    }
    *res = tag;
    return true;
}

bool VTarget::get_key_complex(uint64_t tag, nl::json *res) {
    auto pair = this->number_tag.find(tag);
    if (pair != this->number_tag.end()) {
        *res = pair->second;
        return true;
    }
    return false;
}

bool VTarget::get_key(KLV &klv, nl::json *res) {
    uint64_t tag;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_key_complex(tag, res);
}

bool VTarget::get_value_complex(KLV &klv, uint64_t tag, nl::json *res) {
    auto pair = this->parsers.find(tag);
    // klv.value.restart();
    if (pair != this->parsers.end()) {
        return pair->second(klv.value, res, this->ctx);
    }
    return false;
}

bool VTarget::get_value(KLV &klv, nl::json *res) {
    uint64_t tag = 0;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_value_complex(klv, tag, res);
}

void VTarget::init_parsers() {
    if (VTarget::parsers.size() > 0)
        return;

    VTarget::parsers.insert(std::make_pair(0x01, parsers::vtarget_center_parser));
    VTarget::parsers.insert(std::make_pair(0x02, parsers::vtarget_top_left_parser));
    VTarget::parsers.insert(std::make_pair(0x03, parsers::vtarget_bottom_right_parser));
    VTarget::parsers.insert(std::make_pair(0x04, parsers::vtarget_priority_parser));
    VTarget::parsers.insert(std::make_pair(0x05, parsers::vtarget_confidence_parser));
    VTarget::parsers.insert(std::make_pair(0x06, parsers::vtarget_history_parser));
    VTarget::parsers.insert(std::make_pair(0x07, parsers::vtarget_bbox_fill_percentage_parser));
    VTarget::parsers.insert(std::make_pair(0x08, parsers::vtarget_color_parser));
    VTarget::parsers.insert(std::make_pair(0x09, parsers::vtarget_intensity_parser));
    VTarget::parsers.insert(std::make_pair(0x0a, parsers::vtarget_latitude_offset_parser));
    VTarget::parsers.insert(std::make_pair(0x0b, parsers::vtarget_longitude_offset_parser));
    VTarget::parsers.insert(std::make_pair(0x0c, parsers::vtarget_height_parser));
    VTarget::parsers.insert(std::make_pair(0x0d, parsers::vtarget_top_left_latitude_parser));
    VTarget::parsers.insert(std::make_pair(0x0e, parsers::vtarget_top_left_longitude_parser));
    VTarget::parsers.insert(std::make_pair(0x0f, parsers::vtarget_bottom_right_latitude_parser));
    VTarget::parsers.insert(std::make_pair(0x10, parsers::vtarget_bottom_right_longitude_parser));
    VTarget::parsers.insert(std::make_pair(0x11, parsers::vtarget_location_parser));
    VTarget::parsers.insert(std::make_pair(0x12, parsers::vtarget_boundary_parser));
    VTarget::parsers.insert(std::make_pair(0x13, parsers::vtarget_y_parser));
    VTarget::parsers.insert(std::make_pair(0x14, parsers::vtarget_x_parser));
    VTarget::parsers.insert(std::make_pair(0x15, parsers::vtarget_fpa_parser));
    VTarget::parsers.insert(std::make_pair(0x65, parsers::vtarget_mask_parser));
    VTarget::parsers.insert(std::make_pair(0x66, parsers::vtarget_object_parser));
    VTarget::parsers.insert(std::make_pair(0x67, parsers::vtarget_feature_parser));
    VTarget::parsers.insert(std::make_pair(0x68, parsers::vtarget_tracker_parser));
    VTarget::parsers.insert(std::make_pair(0x69, parsers::vtarget_chip_parser));
    VTarget::parsers.insert(std::make_pair(0x6a, parsers::vtarget_chip_series_parser));
}

void VTarget::init_encoders() {
    if (VTarget::encoders.size() > 0)
        return;

    VTarget::encoders.insert(std::make_pair(0x01, encoders::vtarget_center_encoder));
    VTarget::encoders.insert(std::make_pair(0x02, encoders::vtarget_top_left_encoder));
    VTarget::encoders.insert(std::make_pair(0x03, encoders::vtarget_bottom_right_encoder));
    VTarget::encoders.insert(std::make_pair(0x04, encoders::vtarget_priority_encoder));
    VTarget::encoders.insert(std::make_pair(0x05, encoders::vtarget_confidence_encoder));
    VTarget::encoders.insert(std::make_pair(0x06, encoders::vtarget_history_encoder));
    VTarget::encoders.insert(std::make_pair(0x07, encoders::vtarget_bbox_fill_percentage_encoder));
    VTarget::encoders.insert(std::make_pair(0x08, encoders::vtarget_color_encoder));
    VTarget::encoders.insert(std::make_pair(0x09, encoders::vtarget_intensity_encoder));
    VTarget::encoders.insert(std::make_pair(0x0a, encoders::vtarget_latitude_offset_encoder));
    VTarget::encoders.insert(std::make_pair(0x0b, encoders::vtarget_longitude_offset_encoder));
    VTarget::encoders.insert(std::make_pair(0x0c, encoders::vtarget_height_encoder));
    VTarget::encoders.insert(std::make_pair(0x0d, encoders::vtarget_top_left_latitude_encoder));
    VTarget::encoders.insert(std::make_pair(0x0e, encoders::vtarget_top_left_longitude_encoder));
    VTarget::encoders.insert(std::make_pair(0x0f, encoders::vtarget_bottom_right_latitude_encoder));
    VTarget::encoders.insert(std::make_pair(0x10, encoders::vtarget_bottom_right_longitude_encoder));
    VTarget::encoders.insert(std::make_pair(0x11, encoders::vtarget_location_encoder));
    VTarget::encoders.insert(std::make_pair(0x12, encoders::vtarget_boundary_encoder));
    VTarget::encoders.insert(std::make_pair(0x13, encoders::vtarget_y_encoder));
    VTarget::encoders.insert(std::make_pair(0x14, encoders::vtarget_x_encoder));
    VTarget::encoders.insert(std::make_pair(0x15, encoders::vtarget_fpa_encoder));
    VTarget::encoders.insert(std::make_pair(0x65, encoders::vtarget_mask_encoder));
    VTarget::encoders.insert(std::make_pair(0x66, encoders::vtarget_object_encoder));
    VTarget::encoders.insert(std::make_pair(0x67, encoders::vtarget_feature_encoder));
    VTarget::encoders.insert(std::make_pair(0x68, encoders::vtarget_tracker_encoder));
    VTarget::encoders.insert(std::make_pair(0x69, encoders::vtarget_chip_encoder));
    VTarget::encoders.insert(std::make_pair(0x6a, encoders::vtarget_chip_series_encoder));
}
#define __TAGS(number_tag, tag_number, number, string) \
    number_tag.insert(std::make_pair(number, string)); \
    tag_number.insert(std::make_pair(string, number))

void VTarget::init_tags() {
    if (VTarget::number_tag.size() > 0 || VTarget::tag_number.size() > 0)
        return;
    __TAGS(number_tag, tag_number, 0x01, "center");                 // 1
    __TAGS(number_tag, tag_number, 0x02, "top_left");               // 2
    __TAGS(number_tag, tag_number, 0x03, "bottom_right");           // 3
    __TAGS(number_tag, tag_number, 0x04, "priority");               // 4
    __TAGS(number_tag, tag_number, 0x05, "confidence");             // 5
    __TAGS(number_tag, tag_number, 0x06, "history");                // 6
    __TAGS(number_tag, tag_number, 0x07, "bbox_fill_percentage");   // 7
    __TAGS(number_tag, tag_number, 0x08, "color");                  // 8
    __TAGS(number_tag, tag_number, 0x09, "intensity");              // 9
    __TAGS(number_tag, tag_number, 0x0a, "latitude_offset");        // 10
    __TAGS(number_tag, tag_number, 0x0b, "longitude_offset");       // 11
    __TAGS(number_tag, tag_number, 0x0c, "height");                 // 12
    __TAGS(number_tag, tag_number, 0x0d, "top_left_latitude");      // 13
    __TAGS(number_tag, tag_number, 0x0e, "top_left_longitude");     // 14
    __TAGS(number_tag, tag_number, 0x0f, "bottom_right_latitude");  // 15
    __TAGS(number_tag, tag_number, 0x10, "bottom_right_longitude"); // 16
    __TAGS(number_tag, tag_number, 0x11, "location");               // 17
    __TAGS(number_tag, tag_number, 0x12, "boundary");               // 18
    __TAGS(number_tag, tag_number, 0x13, "y");                      // 19
    __TAGS(number_tag, tag_number, 0x14, "x");                      // 20
    __TAGS(number_tag, tag_number, 0x15, "fpa");                    // 21
    __TAGS(number_tag, tag_number, 0x65, "mask");                   // 101
    __TAGS(number_tag, tag_number, 0x66, "object");                 // 102
    __TAGS(number_tag, tag_number, 0x67, "feature");                // 103
    __TAGS(number_tag, tag_number, 0x68, "tracker");                // 104
    __TAGS(number_tag, tag_number, 0x69, "chip");                   // 105
    __TAGS(number_tag, tag_number, 0x6a, "chip series");            // 106
}

#undef __TAGS

bool VTarget::is_strict() {
    if (this->ctx != nullptr) {
        return this->ctx->strict;
    }
    return false;
}

bool parsers::vtarget_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    VTarget vtarget_parser(ctx != nullptr ? ctx->sub("vtarget") : nullptr);
    return vtarget_parser.parse(reader, res);
}

bool encoders::vtarget_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    VTarget vtarget_encoder(ctx != nullptr ? ctx->sub("vtarget") : nullptr);
    return vtarget_encoder.encode(item, res);
}