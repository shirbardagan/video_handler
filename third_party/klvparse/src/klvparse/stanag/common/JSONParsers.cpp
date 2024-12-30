#include <klvparse/stanag/common/JSONParsers.h>

using namespace klvparse;

namespace nlohmann {
    std::string ensure_dump(json data, int indent) {
        try {
            return data.dump(indent);
        } catch (const std::exception &ex1) {
            try {
                return data.dump(indent, (char)32, false, nl::json::error_handler_t::replace);
            } catch (const std::exception &ex2) {
                return "(FAILED)";
            }
        }
    }
} // namespace nlohmann

namespace parsers = klvparse::parsers;

bool parsers::bytes_length_parser(ByteReader &reader, size_t length, nl::json *j) {
    std::stringstream ss;
    uint8_t byte = 0;
    // std::cout << reader.debug() << std::endl;
    for (uint64_t i = 0; i < length; i++) {
        if (!reader.get_ui8(&byte)) {
            return false;
        }
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)byte;
    }
    // std::cout << ss.str() << std::endl;
    (*j) = ss.str();
    return true;
}

bool parsers::bytes_parser(ByteReader &reader, nl::json *j) {
    std::stringstream ss;
    uint8_t byte = 0;
    // std::cout << reader.debug() << std::endl;
    for (uint64_t i = 0;; i++) {
        if (!reader.get_ui8(&byte)) {
            break;
        }
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)byte;
    }
    // std::cout << ss.str() << std::endl;
    (*j) = ss.str();
    return true;
}

bool parsers::uuid_parser(ByteReader &reader, nl::json *j) {
    std::stringstream ss;
    uint8_t byte = 0;
    // std::cout << reader.debug() << std::endl;
    for (uint64_t i = 0; i < 16; i++) {
        if (!reader.get_ui8(&byte)) {
            return false;
        }
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)byte;
        if(i == 3 || i == 5 || i == 7 || i == 9) {
            ss << "-";
        }
    }
    // std::cout << ss.str() << std::endl;
    (*j) = ss.str();
    return true;
}

#define __BASE_PARSER(GETTER, TYPE_NAME, TYPE)                            \
    bool parsers::TYPE_NAME##_parser(ByteReader &reader, nl::json *res) { \
        TYPE _res = 0;                                                    \
        if (!reader.GETTER(&_res)) {                                      \
            return false;                                                 \
        }                                                                 \
        *res = _res;                                                      \
        return true;                                                      \
    }

#define __VARIABLE_PARSER(GETTER, TYPE_NAME, TYPE)                                   \
    bool parsers::variable_##TYPE_NAME##_parser(ByteReader &reader, nl::json *res) { \
        TYPE _res = 0;                                                               \
        if (!reader.GETTER(&_res)) {                                                 \
            return false;                                                            \
        }                                                                            \
        *res = _res;                                                                 \
        return true;                                                                 \
    }

__BASE_PARSER(get_ui8, uint8, uint8_t);
__BASE_PARSER(get_ui16, uint16, uint16_t);
__BASE_PARSER(get_ui24, uint24, uint32_t);
__BASE_PARSER(get_ui32, uint32, uint32_t);
__BASE_PARSER(get_ui40, uint40, uint64_t);
__BASE_PARSER(get_ui48, uint48, uint64_t);
__BASE_PARSER(get_ui56, uint56, uint64_t);
__BASE_PARSER(get_ui64, uint64, uint64_t);
__BASE_PARSER(get_i8, int8, int8_t);
__BASE_PARSER(get_i16, int16, int16_t);
__BASE_PARSER(get_i32, int32, int32_t);
__BASE_PARSER(get_i64, int64, int64_t);

__VARIABLE_PARSER(get_variable_ui16, uint16, uint16_t);
__VARIABLE_PARSER(get_variable_ui24, uint24, uint32_t);
__VARIABLE_PARSER(get_variable_ui32, uint32, uint32_t);
__VARIABLE_PARSER(get_variable_ui40, uint40, uint64_t);
__VARIABLE_PARSER(get_variable_ui48, uint48, uint64_t);
__VARIABLE_PARSER(get_variable_ui56, uint56, uint64_t);
__VARIABLE_PARSER(get_variable_ui64, uint64, uint64_t);

bool parsers::string_parser(ByteReader &reader, nl::json *res) {
    const uint8_t *data;
    uint64_t size = 0;
    if (!reader.get_data(&data, &size)) {
        return false;
    }
    if (data[size - 1] == '\0') { // might want to change this into while
        size -= 1;
    }
    std::string str;
    str.assign((const char *)data, size);
    *res = str;
    return true;
}

bool parsers::number_or_string_parser(ByteReader &reader, nl::json *res) {
    if (reader.get_remaining() < 8) {
        uint64_t _res;
        if (!reader.get_variable_ui64(&_res)) {
            return false;
        }
        *res = _res;
        return true;
    }
    return string_parser(reader, res);
}

// std::string parsers::timestamp_from_uint64(uint64_t time) {
//     using time_point = std::chrono::system_clock::time_point;
//     time_point uptime_timepoint{std::chrono::duration_cast<time_point::duration>(std::chrono::microseconds(time))};
//     // std::time_t t = std::chrono::system_clock::to_time_t(uptime_timepoint);
//     // auto tm = *std::localtime(&t);
//     // auto tm = *std::gmtime(&t);

//     std::stringstream s;
//     // s << std::put_time(&tm, "yyyy-MM-dd'T'HH:mm:ss'.'SSSZ");
//     // s << std::put_time(&tm, "%d-%m-%Y-T%H:%M:%S.%z%Z");
//     s << date::format("%d-%m-%Y-T%H:%M:%S.%z%Z", uptime_timepoint);
//     return s.str();
// }

bool parsers::timestamp_parser(ByteReader &reader, nl::json *res) {
    uint64_t _res = 0;
    if (!reader.get_ui64(&_res)) {
        return false;
    }
    // std::string str = timestamp_from_uint64(_res);
    // *res = str;
    *res = _res;
    return true;
}

#define __IN_RANGE(val, min, max) (min < val && val < max)

#define __DEG_PARSER(GETTER, SOURCE_TYPE_NAME, DEST_TYPE_NAME, SOURCE_TYPE, DEST_TYPE, SOURCE_TYPE_UNSIGNED)                                                                                \
    bool parsers::deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(SOURCE_TYPE src, DEST_TYPE *dest, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {                                          \
        SOURCE_TYPE_UNSIGNED range = -1;                                                                                                                                                    \
        DEST_TYPE normalizer = (max - min) / range;                                                                                                                                         \
        *dest = (((DEST_TYPE)src) * normalizer) + offset;                                                                                                                                   \
        return true;                                                                                                                                                                        \
    }                                                                                                                                                                                       \
    bool parsers::deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(SOURCE_TYPE src, DEST_TYPE *dest, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {                               \
        SOURCE_TYPE_UNSIGNED range = -2;                                                                                                                                                    \
        DEST_TYPE normalizer = (max - min) / range;                                                                                                                                         \
        DEST_TYPE angle = (((DEST_TYPE)src) * normalizer) + offset;                                                                                                                         \
        if (__IN_RANGE(angle, min, max)) {                                                                                                                                                  \
            *dest = angle;                                                                                                                                                                  \
            return true;                                                                                                                                                                    \
        }                                                                                                                                                                                   \
        return false;                                                                                                                                                                       \
    }                                                                                                                                                                                       \
    bool parsers::deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_parser(ByteReader &reader, nl::json *res, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {                                \
        SOURCE_TYPE _res = 0;                                                                                                                                                               \
        if (!reader.GETTER(&_res)) {                                                                                                                                                        \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        DEST_TYPE angle;                                                                                                                                                                    \
        if (!deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(_res, &angle, min, max, offset)) {                                                                                                \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        *res = angle;                                                                                                                                                                       \
        return true;                                                                                                                                                                        \
    }                                                                                                                                                                                       \
    bool parsers::deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved_parser(ByteReader &reader, nl::json *res, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset, const char *reserved) { \
        SOURCE_TYPE _res = 0;                                                                                                                                                               \
        if (!reader.GETTER(&_res)) {                                                                                                                                                        \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        DEST_TYPE angle;                                                                                                                                                                    \
        if (!deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(_res, &angle, min, max, offset)) {                                                                                     \
            *res = reserved;                                                                                                                                                                \
        } else {                                                                                                                                                                            \
            *res = angle;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        return true;                                                                                                                                                                        \
    }

__DEG_PARSER(get_ui8, uint8, float, uint8_t, float, uint8_t);
__DEG_PARSER(get_ui16, uint16, float, uint16_t, float, uint16_t);
__DEG_PARSER(get_i16, int16, float, int16_t, float, uint16_t);
__DEG_PARSER(get_ui32, uint32, double, uint32_t, double, uint32_t);
__DEG_PARSER(get_i32, int32, double, int32_t, double, uint32_t);

#define __DEG_VARIABLE_PARSER(GETTER, SOURCE_TYPE_NAME, DEST_TYPE_NAME, SOURCE_TYPE, DEST_TYPE, SOURCE_TYPE_UNSIGNED, byte_length)                                                                   \
    bool parsers::deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(SOURCE_TYPE src, uint8_t length, DEST_TYPE *dest, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {                          \
        SOURCE_TYPE_UNSIGNED range = length >= byte_length ? -1 : (1 << (length * 8)) - 1;                                                                                                           \
        DEST_TYPE normalizer = (max - min) / range;                                                                                                                                                  \
        *dest = (((DEST_TYPE)src) * normalizer) + offset;                                                                                                                                            \
        return true;                                                                                                                                                                                 \
    }                                                                                                                                                                                                \
    bool parsers::deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(SOURCE_TYPE src, uint8_t length, DEST_TYPE *dest, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {               \
        SOURCE_TYPE_UNSIGNED range = length >= byte_length ? -2 : (1 << (length * 8)) - 2;                                                                                                           \
        DEST_TYPE normalizer = (max - min) / range;                                                                                                                                                  \
        DEST_TYPE angle = (((DEST_TYPE)src) * normalizer) + offset;                                                                                                                                  \
        if (__IN_RANGE(angle, min, max)) {                                                                                                                                                           \
            *dest = angle;                                                                                                                                                                           \
            return true;                                                                                                                                                                             \
        }                                                                                                                                                                                            \
        return false;                                                                                                                                                                                \
    }                                                                                                                                                                                                \
    bool parsers::deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_parser(ByteReader &reader, nl::json *res, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset) {                                \
        SOURCE_TYPE _res = 0;                                                                                                                                                                        \
        size_t length;                                                                                                                                                                               \
        if (!reader.GETTER(&_res, &length)) {                                                                                                                                                        \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        DEST_TYPE angle;                                                                                                                                                                             \
        if (!deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(_res, length, &angle, min, max, offset)) {                                                                                        \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        *res = angle;                                                                                                                                                                                \
        return true;                                                                                                                                                                                 \
    }                                                                                                                                                                                                \
    bool parsers::deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved_parser(ByteReader &reader, nl::json *res, DEST_TYPE min, DEST_TYPE max, DEST_TYPE offset, const char *reserved) { \
        SOURCE_TYPE _res = 0;                                                                                                                                                                        \
        size_t length;                                                                                                                                                                               \
        if (!reader.GETTER(&_res, &length)) {                                                                                                                                                        \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        DEST_TYPE angle;                                                                                                                                                                             \
        if (!deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(_res, length, &angle, min, max, offset)) {                                                                             \
            *res = reserved;                                                                                                                                                                         \
        } else {                                                                                                                                                                                     \
            *res = angle;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        return true;                                                                                                                                                                                 \
    }

__DEG_VARIABLE_PARSER(get_variable_ui64, uint64, double, uint64_t, double, uint64_t, 8);
__DEG_VARIABLE_PARSER(get_variable_ui32, uint32, float, uint32_t, float, uint32_t, 4);

bool parsers::general_klv_parser(ByteReader &reader, nl::json *res) {
    nl::json _res = nl::json::object();
    for (uint8_t i = 0; reader.get_remaining() > 0; i++) {
        KLV klv = KLV::parse(reader, PARSE_FORMATS::_1, PARSE_FORMATS::_1);
        uint8_t key;
        if (klv.key.get_ui8(&key)) {
            nl::json temp;
            bool has = false;
            if (klv.value.get_remaining() > 0) {
                if (!has)
                    if (string_parser(klv.value, &temp)) {
                        has = true;
                    }
                if (has) {
                    _res[std::to_string((int)key)] = temp;
                }
            }
        }
    }
    *res = _res;
    return true;
}
