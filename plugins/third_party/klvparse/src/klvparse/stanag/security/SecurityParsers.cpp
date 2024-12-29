#include <klvparse/stanag/security/SecurityParsers.h>

using namespace klvparse;
namespace parsers = klvparse::parsers;

const char *klvparse::security_classification_to_string(uint8_t classification) {
    switch (classification) {
    case 0x01:
        return "UNCLASSIFIED";
    case 0x02:
        return "RESTRICTED";
    case 0x03:
        return "CONFIDENTIAL";
    case 0x04:
        return "SECRET";
    case 0x05:
        return "TOP SECRET";
    default:
        return NULL;
    }
}

bool parsers::security_classification_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    uint8_t value = 0;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    const char *str = security_classification_to_string(value);
    if (str != NULL) {
        *res = str;
    } else {
        *res = value;
    }
    return true;
}

const char *klvparse::security_ccric_method_to_string(uint8_t ccric_method) {
    switch (ccric_method) {
    case 0x01:
        return "ISO-3166 Two Letter";
    case 0x02:
        return "ISO-3166 Three Letter";
    case 0x03:
        return "FIPS 10-4 Two Letter";
    case 0x04:
        return "FIPS 10-4 Four Letter";
    case 0x05:
        return "ISO-3166 Numeric";
    case 0x06:
        return "1059 Two Letter";
    case 0x07:
        return "1059 Three Letter";
    case 0x08:
        return "Omitted Value";
    case 0x09:
        return "Omitted Value";
    case 0x0a:
        return "FIPS 10-4 Mixed";
    case 0x0b:
        return "ISO 3166 Mixed";
    case 0x0c:
        return "STANAG 1059 Mixed";
    case 0x0d:
        return "GENC Two Letter";
    case 0x0e:
        return "GENC Three Letter";
    case 0x0f:
        return "GENC Numeric";
    case 0x10:
        return "GENC Mixed";
    default:
        return NULL;
    }
}

bool parsers::security_ccric_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    uint8_t value = 0;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    const char *str = security_ccric_method_to_string(value);
    if (str != NULL) {
        *res = str;
    } else {
        *res = value;
    }
    return true;
}
bool parsers::security_country_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_sci_information_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_caveats_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_releasing_instructions_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_by_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_from_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_reason_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res); // #TODO this should be date like
}
bool parsers::security_system_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
const char *klvparse::security_oc_method_to_string(uint8_t oc_method) {
    switch (oc_method) {
    case 0x01:
        return "ISO-3166 Two Letter";
    case 0x02:
        return "ISO-3166 Three Letter";
    case 0x03:
        return "ISO-3166 Numeric";
    case 0x04:
        return "FIPS 10-4 Two Letter";
    case 0x05:
        return "FIPS 10-4 Four Letter";
    case 0x06:
        return "1059 Two Letter";
    case 0x07:
        return "1059 Three Letter";
    case 0x08:
        return "Omitted Value";
    case 0x09:
        return "Omitted Value";
    case 0x0A:
        return "Omitted Value";
    case 0x0B:
        return "Omitted Value";
    case 0x0C:
        return "Omitted Value";
    case 0x0D:
        return "GENC Two Letter";
    case 0x0E:
        return "GENC Three Letter";
    case 0x0F:
        return "GENC Numeric";
    case 0x40:
        return "GENC AdminSub";
    default:
        return NULL;
    }
}
bool parsers::security_oc_method_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    uint8_t value = 0;
    if (!reader.get_ui8(&value)) {
        return false;
    }
    const char *str = security_ccric_method_to_string(value);
    if (str != NULL) {
        *res = str;
    } else {
        *res = value;
    }
    return true;
}
bool parsers::security_oc_code_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_comment_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res);
}
bool parsers::security_umid_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    // return bytes_length_parser(reader, 32, res);
    return bytes_parser(reader, res);
}
bool parsers::security_stream_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return uint8_parser(reader, res);
}
bool parsers::security_transport_stream_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return uint16_parser(reader, res);
}
bool parsers::security_item_designator_id_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    // return bytes_length_parser(reader, 16, res);
    return bytes_parser(reader, res);
    // return string_parser(reader, res);
}
bool parsers::security_version_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return uint16_parser(reader, res);
}
bool parsers::security_ccric_version_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res); // #TODO this should be date like
}
bool parsers::security_oc_version_date_parser(ByteReader &reader, nl::json *res, std::shared_ptr<Context> storage) {
    return string_parser(reader, res); // #TODO this should be date like
}