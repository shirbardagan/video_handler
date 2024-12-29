#include <klvparse/stanag/security/SecurityEncoders.h>
using namespace klvparse;
namespace encoders = klvparse::encoders;

bool encoders::security_classification_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source = 0;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "UNCLASSIFIED") {
            source = 0x01;
        } else if (str == "RESTRICTED") {
            source = 0x02;
        } else if (str == "CONFIDENTIAL") {
            source = 0x03;
        } else if (str == "SECRET") {
            source = 0x04;
        } else if (str == "TOP SECRET") {
            source = 0x05;
        } else {
            return false;
        }
    } else {
        source = item.get<uint8_t>();
        if (source > 0x05) {
            return false;
        }
    }
    return encode_uint8(source, res);
}
bool encoders::security_ccric_method_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source = 0;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "ISO-3166 Two Letter") {
            source = 0x01;
        } else if (str == "ISO-3166 Three Letter") {
            source = 0x02;
        } else if (str == "FIPS 10-4 Two Letter") {
            source = 0x03;
        } else if (str == "FIPS 10-4 Four Letter") {
            source = 0x04;
        } else if (str == "ISO-3166 Numeric") {
            source = 0x05;
        } else if (str == "1059 Two Letter") {
            source = 0x06;
        } else if (str == "1059 Three Letter") {
            source = 0x07;
        } else if (str == "Omitted Value") {
            source = 0x08;
        } else if (str == "Omitted Value") {
            source = 0x09;
        } else if (str == "FIPS 10-4 Mixed") {
            source = 0x0a;
        } else if (str == "ISO 3166 Mixed") {
            source = 0x0b;
        } else if (str == "STANAG 1059 Mixed") {
            source = 0x0c;
        } else if (str == "GENC Two Letter") {
            source = 0x0d;
        } else if (str == "GENC Three Letter") {
            source = 0x0e;
        } else if (str == "GENC Numeric") {
            source = 0x0f;
        } else if (str == "GENC Mixed") {
            source = 0x10;
        } else {
            return false;
        }
    } else {
        source = item.get<uint8_t>();
        if (source > 0x10) {
            return false;
        }
    }
    return encode_uint8(source, res);
}
bool encoders::security_country_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_sci_information_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_caveats_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_releasing_instructions_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_by_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_from_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_reason_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res); // #TODO this should be date like
}
bool encoders::security_system_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_oc_method_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    uint8_t source = 0;
    if (item.is_string()) {
        std::string str = item.get<std::string>();
        if (str == "ISO-3166 Two Letter") {
            source = 0x01;
        } else if (str == "ISO-3166 Three Letter") {
            source = 0x02;
        } else if (str == "ISO-3166 Numeric") {
            source = 0x03;
        } else if (str == "FIPS 10-4 Two Letter") {
            source = 0x04;
        } else if (str == "FIPS 10-4 Four Letter") {
            source = 0x05;
        } else if (str == "1059 Two Letter") {
            source = 0x06;
        } else if (str == "1059 Three Letter") {
            source = 0x07;
        } else if (str == "Omitted Value") {
            source = 0x08;
        } else if (str == "Omitted Value") {
            source = 0x09;
        } else if (str == "Omitted Value") {
            source = 0x0a;
        } else if (str == "Omitted Value") {
            source = 0x0b;
        } else if (str == "Omitted Value") {
            source = 0x0c;
        } else if (str == "GENC Two Letter") {
            source = 0x0d;
        } else if (str == "GENC Three Letter") {
            source = 0x0e;
        } else if (str == "GENC Numeric") {
            source = 0x0f;
        } else if (str == "GENC AdminSub") {
            source = 0x40;
        } else {
            return false;
        }
    } else {
        source = item.get<uint8_t>();
        // if (source > 0x40) {
        //     return false;
        // }
    }
    return encode_uint8(source, res);
}
bool encoders::security_oc_code_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_comment_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res);
}
bool encoders::security_umid_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    // return bytes_length_encoder(item, 32, res);
    return bytes_encoder(item, res);
}
bool encoders::security_stream_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint8_encoder(item, res);
}
bool encoders::security_transport_stream_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint16_encoder(item, res);
}
bool encoders::security_item_designator_id_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    // return bytes_length_encoder(item, 16, res);
    return bytes_encoder(item, res);
    // return string_encoder(item, res);
}
bool encoders::security_version_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return uint16_encoder(item, res);
}
bool encoders::security_ccric_version_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res); // #TODO this should be date like
}
bool encoders::security_oc_version_date_encoder(const nl::json &item, std::vector<uint8_t> *res, std::shared_ptr<Context> ctx) {
    return string_encoder(item, res); // #TODO this should be date like
}