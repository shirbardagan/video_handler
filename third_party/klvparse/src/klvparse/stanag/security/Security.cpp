#include <klvparse/stanag/security/Security.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;
namespace encoders = klvparse::encoders;

// std::unordered_map<uint64_t, std::string> Security::security_number_tag;
std::map<uint64_t, std::string> Security::number_tag;
std::unordered_map<std::string, uint64_t> Security::tag_number;
// std::unordered_map<uint64_t, std::function<bool(KLV *, KLVStorage *)>> Security::parsers;
std::unordered_map<uint64_t, std::function<bool(ByteReader &, nl::json *, std::shared_ptr<Context>)>> Security::parsers;
std::unordered_map<uint64_t, std::function<bool(const nl::json &, std::vector<uint8_t> *, std::shared_ptr<Context>)>> Security::encoders;

void Security::init() {
    init_tags();
    init_parsers();
    init_encoders();
}

Security::Security() {
    init();
    this->ctx = std::make_shared<Context>();
}

Security::Security(Context ctx) {
    init();
    this->ctx = std::make_shared<Context>(ctx);
}

Security::Security(std::weak_ptr<Context> ctx) {
    init();
    this->ctx = ctx.expired() ? nullptr : ctx.lock();
}

Security::Security(std::shared_ptr<Context> ctx) {
    init();
    this->ctx = ctx;
}

Security::~Security() {
}

bool Security::parse(const uint8_t *data, uint64_t size, nl::json *res, uint64_t skip) {
    std::vector<uint8_t> buffer(data, data + size);
    return parse(buffer, res, skip);
}

bool Security::parse(std::vector<uint8_t> &buffer, nl::json *res, uint64_t skip) {
    ByteReader reader(buffer);
    return parse(reader, res, skip);
}

bool Security::parse(ByteReader &reader, nl::json *res, uint64_t skip) {
    try {
        if (skip > 0) {
            reader.skip(skip);
        }
        // reader.skip(1);
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

bool Security::encode(const nl::json &j, std::vector<uint8_t> *res) {
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

bool Security::get_tag(KLV &item, uint64_t *res) {
    uint64_t tag;
    item.key.restart();
    if (!item.key.get_ber_oid(&tag)) {
        return false;
    }
    *res = tag;
    return true;
}

bool Security::get_key_complex(uint64_t tag, nl::json *res) {
    auto pair = this->number_tag.find(tag);
    if (pair != this->number_tag.end()) {
        *res = pair->second;
        return true;
    }
    return false;
}

bool Security::get_key(KLV &klv, nl::json *res) {
    uint64_t tag;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_key_complex(tag, res);
}

bool Security::get_value_complex(KLV &klv, uint64_t tag, nl::json *res) {
    auto pair = this->Security::parsers.find(tag);
    // item->value->restart();
    if (pair != this->Security::parsers.end()) {
        return pair->second(klv.value, res, this->ctx);
    }
    return false;
}

bool Security::get_value(KLV &klv, nl::json *res) {
    uint64_t tag = 0;
    if (!get_tag(klv, &tag)) {
        return false;
    }
    return get_value_complex(klv, tag, res);
}

void Security::init_parsers() {
    if (Security::parsers.size() > 0)
        return;

    // 0x01 - 1 - classification
    Security::parsers.insert(std::make_pair(0x01, parsers::security_classification_parser));
    // 0x02 - 2 - ccric_method
    Security::parsers.insert(std::make_pair(0x02, parsers::security_ccric_method_parser));
    // 0x03 - 3 - country
    Security::parsers.insert(std::make_pair(0x03, parsers::security_country_parser));
    // 0x04 - 4 - sci_information
    Security::parsers.insert(std::make_pair(0x04, parsers::security_sci_information_parser));
    // 0x05 - 5 - caveats
    Security::parsers.insert(std::make_pair(0x05, parsers::security_caveats_parser));
    // 0x06 - 6 - releasing_instructions
    Security::parsers.insert(std::make_pair(0x06, parsers::security_releasing_instructions_parser));
    // 0x07 - 7 - by
    Security::parsers.insert(std::make_pair(0x07, parsers::security_by_parser));
    // 0x08 - 8 - from
    Security::parsers.insert(std::make_pair(0x08, parsers::security_from_parser));
    // 0x09 - 9 - reason
    Security::parsers.insert(std::make_pair(0x09, parsers::security_reason_parser));
    // 0x0a - 10 - date
    Security::parsers.insert(std::make_pair(0x0a, parsers::security_date_parser));
    // 0x0b - 11 - system
    Security::parsers.insert(std::make_pair(0x0b, parsers::security_system_parser));
    // 0x0c - 12 - oc_method
    Security::parsers.insert(std::make_pair(0x0c, parsers::security_oc_method_parser));
    // 0x0d - 13 - oc_code
    Security::parsers.insert(std::make_pair(0x0d, parsers::security_oc_code_parser));
    // 0x0e - 14 - comment
    Security::parsers.insert(std::make_pair(0x0e, parsers::security_comment_parser));
    // 0x0f - 15 - umid
    Security::parsers.insert(std::make_pair(0x0f, parsers::security_umid_parser));
    // 0x13 - 19 - stream_id
    Security::parsers.insert(std::make_pair(0x13, parsers::security_stream_id_parser));
    // 0x14 - 20 - transport_stream_id
    Security::parsers.insert(std::make_pair(0x14, parsers::security_transport_stream_id_parser));
    // 0x15 - 21 - item_designator_id
    Security::parsers.insert(std::make_pair(0x15, parsers::security_item_designator_id_parser));
    // 0x16 - 22 - version
    Security::parsers.insert(std::make_pair(0x16, parsers::security_version_parser));
    // 0x17 - 23 - ccric_version_date
    Security::parsers.insert(std::make_pair(0x17, parsers::security_ccric_version_date_parser));
    // 0x18 - 24 - oc_version_date
    Security::parsers.insert(std::make_pair(0x18, parsers::security_oc_version_date_parser));
}

void Security::init_encoders() {
    if (Security::encoders.size() > 0)
        return;

    // 0x01 - 1 - classification
    Security::encoders.insert(std::make_pair(0x01, encoders::security_classification_encoder));
    // 0x02 - 2 - ccric_method
    Security::encoders.insert(std::make_pair(0x02, encoders::security_ccric_method_encoder));
    // 0x03 - 3 - country
    Security::encoders.insert(std::make_pair(0x03, encoders::security_country_encoder));
    // 0x04 - 4 - sci_information
    Security::encoders.insert(std::make_pair(0x04, encoders::security_sci_information_encoder));
    // 0x05 - 5 - caveats
    Security::encoders.insert(std::make_pair(0x05, encoders::security_caveats_encoder));
    // 0x06 - 6 - releasing_instructions
    Security::encoders.insert(std::make_pair(0x06, encoders::security_releasing_instructions_encoder));
    // 0x07 - 7 - by
    Security::encoders.insert(std::make_pair(0x07, encoders::security_by_encoder));
    // 0x08 - 8 - from
    Security::encoders.insert(std::make_pair(0x08, encoders::security_from_encoder));
    // 0x09 - 9 - reason
    Security::encoders.insert(std::make_pair(0x09, encoders::security_reason_encoder));
    // 0x0a - 10 - date
    Security::encoders.insert(std::make_pair(0x0a, encoders::security_date_encoder));
    // 0x0b - 11 - system
    Security::encoders.insert(std::make_pair(0x0b, encoders::security_system_encoder));
    // 0x0c - 12 - oc_method
    Security::encoders.insert(std::make_pair(0x0c, encoders::security_oc_method_encoder));
    // 0x0d - 13 - oc_code
    Security::encoders.insert(std::make_pair(0x0d, encoders::security_oc_code_encoder));
    // 0x0e - 14 - comment
    Security::encoders.insert(std::make_pair(0x0e, encoders::security_comment_encoder));
    // 0x0f - 15 - umid
    Security::encoders.insert(std::make_pair(0x0f, encoders::security_umid_encoder));
    // 0x13 - 19 - stream_id
    Security::encoders.insert(std::make_pair(0x13, encoders::security_stream_id_encoder));
    // 0x14 - 20 - transport_stream_id
    Security::encoders.insert(std::make_pair(0x14, encoders::security_transport_stream_id_encoder));
    // 0x15 - 21 - item_designator_id
    Security::encoders.insert(std::make_pair(0x15, encoders::security_item_designator_id_encoder));
    // 0x16 - 22 - version
    Security::encoders.insert(std::make_pair(0x16, encoders::security_version_encoder));
    // 0x17 - 23 - ccric_version_date
    Security::encoders.insert(std::make_pair(0x17, encoders::security_ccric_version_date_encoder));
    // 0x18 - 24 - oc_version_date
    Security::encoders.insert(std::make_pair(0x18, encoders::security_oc_version_date_encoder));
}
#define __TAGS(number_tag, tag_number, number, string) \
    number_tag.insert(std::make_pair(number, string)); \
    tag_number.insert(std::make_pair(string, number))

void Security::init_tags() {
    if (Security::number_tag.size() > 0 || Security::tag_number.size() > 0)
        return;
    __TAGS(number_tag, tag_number, 0x01, "classification");         // 1
    __TAGS(number_tag, tag_number, 0x02, "ccric_method");           // 2
    __TAGS(number_tag, tag_number, 0x03, "country");                // 3
    __TAGS(number_tag, tag_number, 0x04, "sci_information");        // 4
    __TAGS(number_tag, tag_number, 0x05, "caveats");                // 5
    __TAGS(number_tag, tag_number, 0x06, "releasing_instructions"); // 6
    __TAGS(number_tag, tag_number, 0x07, "by");                     // 7
    __TAGS(number_tag, tag_number, 0x08, "from");                   // 8
    __TAGS(number_tag, tag_number, 0x09, "reason");                 // 9
    __TAGS(number_tag, tag_number, 0x0a, "date");                   // 10
    __TAGS(number_tag, tag_number, 0x0b, "system");                 // 11
    __TAGS(number_tag, tag_number, 0x0c, "oc_method");              // 12
    __TAGS(number_tag, tag_number, 0x0d, "oc_code");                // 13
    __TAGS(number_tag, tag_number, 0x0e, "comment");                // 14
    __TAGS(number_tag, tag_number, 0x0f, "umid");                   // 15
    __TAGS(number_tag, tag_number, 0x13, "stream_id");              // 19
    __TAGS(number_tag, tag_number, 0x14, "transport_stream_id");    // 20
    __TAGS(number_tag, tag_number, 0x15, "item_designator_id");     // 21
    __TAGS(number_tag, tag_number, 0x16, "version");                // 22
    __TAGS(number_tag, tag_number, 0x17, "ccric_version_date");     // 23
    __TAGS(number_tag, tag_number, 0x18, "oc_version_date");        // 24
}

#undef __TAGS

bool Security::is_strict() {
    if (this->ctx != nullptr) {
        return this->ctx->strict;
    }
    return false;
}

bool parsers::security_local_set_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> ctx) {
    Security security_parser(ctx != nullptr ? ctx->sub("security") : nullptr);
    return security_parser.parse(reader, res);
}
bool encoders::security_local_set_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    Security security_parser(ctx != nullptr ? ctx->sub("security") : nullptr);
    return security_parser.encode(item, res);
}