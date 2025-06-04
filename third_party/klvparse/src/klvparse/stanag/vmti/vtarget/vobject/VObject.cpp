#include <klvparse/stanag/vmti/vtarget/vobject/VObject.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;
namespace encoders = klvparse::encoders;

// std::unordered_map<uint64_t, std::string> VObject::vmti_number_tag;
std::map<uint64_t, std::string> VObject::number_tag;
std::unordered_map<std::string, uint64_t> VObject::tag_number;
// std::unordered_map<uint64_t, std::function<bool(KLV *, KLVStorage *)>> VObject::parsers;
std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> VObject::parsers;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> VObject::encoders;

void VObject::init() {
    init_tags();
    init_parsers();
    init_encoders();
}

VObject::VObject() {
    init();
}

VObject::VObject(Context ctx) {
    init();
    this->ctx = std::make_shared<Context>(ctx);
}

VObject::VObject(std::weak_ptr<Context> ctx) {
    init();
    this->ctx = ctx.expired() ? nullptr : ctx.lock();
}

VObject::VObject(std::shared_ptr<Context> ctx) {
    init();
    this->ctx = ctx;
}

VObject::~VObject() {
}

bool VObject::parse(const uint8_t *data, uint64_t size, nl::json *res, uint64_t skip) {
    std::vector<uint8_t> buffer(data, data + size);
    return parse(buffer, res, skip);
}

bool VObject::parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip) {
    ByteReader reader(buffer);
    return parse(reader, res, skip);
}

bool VObject::parse(ByteReader &reader, nl::json *res, uint64_t skip) {
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
        nl::json temp_key;
        nl::json temp_value;
        nl::json _res = nl::json::object();
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

bool VObject::verify_checksum(ByteReader &reader) {
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

// bool VObject::parse(const nl::json &j, KLV *res) {
// #define __TYPICAL_KLV_SIZE 1000
bool VObject::encode(const nl::json &j, std::vector<uint8_t> *res) {
    // std::vector<uint8_t> _res(__TYPICAL_KLV_SIZE);
    // std::cout << "~~~~~~~~~~~~~~" << std::endl;
    // std::cout << j.dump() << std::endl;
    std::vector<uint8_t> _res;
    try {
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

bool VObject::get_tag(KLV &klv, uint64_t *res) {
    uint64_t tag;
    klv.key.restart();
    if (!klv.key.get_ber_oid(&tag)) {
        return false;
    }
    *res = tag;
    return true;
}

bool VObject::get_key_complex(uint64_t tag, nl::json *res) {
    auto pair = this->number_tag.find(tag);
    if (pair != this->number_tag.end()) {
        *res = pair->second;
        return true;
    }
    return false;
}

bool VObject::get_key(KLV &klv, nl::json *res) {
    uint64_t tag;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_key_complex(tag, res);
}

bool VObject::get_value_complex(KLV &klv, uint64_t tag, nl::json *res) {
    auto pair = this->parsers.find(tag);
    // item->value->restart();
    if (pair != this->parsers.end()) {
        return pair->second(klv.value, res, this->ctx);
    }
    return false;
}

bool VObject::get_value(KLV &klv, nl::json *res) {
    uint64_t tag = 0;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_value_complex(klv, tag, res);
}

void VObject::init_parsers() {
    if (VObject::parsers.size() > 0)
        return;

    VObject::parsers.insert(std::make_pair(0x01, parsers::vobject_ontology_parser));
    VObject::parsers.insert(std::make_pair(0x02, parsers::vobject_class_parser));
}

void VObject::init_encoders() {
    if (VObject::encoders.size() > 0)
        return;

    VObject::encoders.insert(std::make_pair(0x01, encoders::vobject_ontology_encoder));
    VObject::encoders.insert(std::make_pair(0x02, encoders::vobject_class_encoder));
}
#define __TAGS(number_tag, tag_number, number, string) \
    number_tag.insert(std::make_pair(number, string)); \
    tag_number.insert(std::make_pair(string, number))

void VObject::init_tags() {
    if (VObject::number_tag.size() > 0 || VObject::tag_number.size() > 0)
        return;
    __TAGS(number_tag, tag_number, 0x01, "ontology"); // 1
    __TAGS(number_tag, tag_number, 0x02, "class");    // 2
}

#undef __TAGS

bool VObject::is_strict() {
    if (this->ctx != nullptr) {
        return this->ctx->strict;
    }
    return false;
}

bool parsers::vobject_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    VObject vobject_parser(ctx != nullptr ? ctx->sub("vobject") : nullptr);
    return vobject_parser.parse(reader, res);
}
bool encoders::vobject_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    VObject vobject_encoder(ctx != nullptr ? ctx->sub("vobject") : nullptr);
    return vobject_encoder.encode(item, res);
}