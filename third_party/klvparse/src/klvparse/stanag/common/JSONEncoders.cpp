#include <klvparse/stanag/common/JSONEncoders.h>

namespace encoders = klvparse::encoders;

bool encoders::bytes_length_encoder(const nl::json &j, size_t length, std::vector<uint8_t> *res) {
    if (!j.is_string()) {
        return false;
    }
    std::string val = j.get<std::string>();
    if (val.length() / 2 != length) {
        return false;
    }
    if (val.length() % 2 != 0) {
        return false;
    }
    auto size = val.length() / 2;
    std::vector<uint8_t> buffer(size);
    for (uint64_t i = 0; i < size; i++) {
        std::stringstream ss;
        ss << std::hex << val.substr(i * 2, 2);
        uint16_t _res = 0;
        ss >> _res;
        if (ss >> _res) {
            return false;
        }
        buffer.at(i) = _res;
    }
    *res = buffer;
    return true;
}

bool encoders::bytes_encoder(const nl::json &j, std::vector<uint8_t> *res) {
    if (!j.is_string()) {
        return false;
    }
    std::string val = j.get<std::string>();
    if (val.length() % 2 != 0) {
        return false;
    }
    auto size = val.length() / 2;
    std::vector<uint8_t> buffer(size);
    for (uint64_t i = 0; i < size; i++) {
        std::stringstream ss;
        ss << std::hex << val.substr(i * 2, 2);
        uint16_t _res = 0;
        ss >> _res;
        if (ss >> _res) {
            return false;
        }
        buffer.at(i) = _res;
    }
    *res = buffer;
    return true;
}

bool encoders::uuid_encoder(const nl::json &j, std::vector<uint8_t> *res) {
    if (!j.is_string()) {
        return false;
    }
    std::string val = j.get<std::string>();
    val.erase(std::remove_if(val.begin(), val.end(), [](char c) { return c == '-'; }), val.end());
    nl::json temp = val;
    return bytes_length_encoder(temp, 16, res);
}

bool encoders::timestamp_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    // using time_point = std::chrono::system_clock::time_point;
    // std::tm t({});
    // std::istringstream iss;
    // iss.str(item.get<std::string>());
    // time_point uptime_timepoint;
    // iss >> date::parse("%d-%m-%Y-T%H:%M:%S.%z%Z", uptime_timepoint);
    // iss >> std::get_time(&t, "%d-%m-%Y-T%H:%M:%S.%z%Z");
    // std::time_t tm = mktime(&t);
    // auto epoch = std::chrono::time_point_cast<std::chrono::microseconds>(uptime_timepoint).time_since_epoch();
    // uint64_t tt = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    // auto epoch = std::chrono::time_point_cast<std::chrono::milliseconds>(uptime_timepoint).time_since_epoch();
    // uint64_t tt = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count() * 1000;
    return encoders::encode_uint64(item.get<uint64_t>(), res);
}

bool encoders::string_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    std::string str = item.is_string() ? item.get<std::string>() : item.dump(-1, (char)32, false, nl::json::error_handler_t::replace);
    std::vector<uint8_t> _res(str.begin(), str.end());
    *res = std::move(_res);
    return true;
}

bool encoders::general_klv_encoder(const nl::json &item, std::vector<uint8_t> *res) {
    if (!item.is_object()) {
        return false;
    }
    std::vector<uint8_t> _res;
    uint64_t count = 0;
    for (auto const &[tag, value] : item.items()) {
        std::stringstream _tag(tag);
        uint64_t number = -1;
        _tag >> number;
        if (number == -1) {
            std::cout << "general klv tag: " << tag << " is not a number" << std::endl;
            continue;
        }
        std::vector<uint8_t> tag_bytes;
        uint8_t number_uint8 = (uint8_t)number;
        if (!encode_uint8(number_uint8, &tag_bytes)) {
            std::cout << "general klv tag: " << tag << " failed to encode tag (" << number_uint8 << ") as uint8" << std::endl;
            continue;
        }
        std::vector<uint8_t> value_bytes;
        if (!string_encoder(value, &value_bytes)) {
            std::cout << "general klv tag: " << tag << " failed to encode tag's value as string" << std::endl;
            continue;
        }
        std::vector<uint8_t> length_bytes;
        uint8_t l = (uint8_t)value_bytes.size();
        if (!encode_uint8(l, &length_bytes)) {
            std::cout << "general klv tag: " << tag << " failed to encode tag's length (" << l << ") as uint8" << std::endl;
            continue;
        };
        _res.insert(_res.end(), tag_bytes.begin(), tag_bytes.end());
        _res.insert(_res.end(), length_bytes.begin(), length_bytes.end());
        _res.insert(_res.end(), value_bytes.begin(), value_bytes.end());
        count += 1;
    }
    if (count <= 0) {
        return false;
    }
    *res = std::move(_res);
    return true;
}

#define __BASE_ENCODER(type_name, type)                                                   \
    bool encoders::type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res) { \
        if (!item.is_number()) {                                                          \
            return false;                                                                 \
        }                                                                                 \
        type source = item.get<type>();                                                   \
        return encoders::encode_##type_name(source, res);                                 \
    }

__BASE_ENCODER(uint8, uint8_t);
__BASE_ENCODER(uint16, uint16_t);
__BASE_ENCODER(uint24, uint32_t);
__BASE_ENCODER(uint32, uint32_t);
__BASE_ENCODER(uint40, uint64_t);
__BASE_ENCODER(uint48, uint64_t);
__BASE_ENCODER(uint56, uint64_t);
__BASE_ENCODER(uint64, uint64_t);

#define __BASE_VARIABLE(type_name, type)                                                             \
    bool encoders::variable_##type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res) { \
        if (!item.is_number()) {                                                                     \
            return false;                                                                            \
        }                                                                                            \
        type source = item.get<type>();                                                              \
        return encoders::encode_variable_##type_name(source, res);                                   \
    }

__BASE_VARIABLE(uint8, uint8_t);
__BASE_VARIABLE(uint16, uint16_t);
__BASE_VARIABLE(uint24, uint32_t);
__BASE_VARIABLE(uint32, uint32_t);
__BASE_VARIABLE(uint40, uint64_t);
__BASE_VARIABLE(uint48, uint64_t);
__BASE_VARIABLE(uint56, uint64_t);
__BASE_VARIABLE(uint64, uint64_t);

#define __IN_RANGE(val, min, max) (min < val && val < max)

#define __DEG_ENCODER(dest_type_name, source_type_name, dest_type, source_type, dest_type_unsigned)                                                                                         \
    bool encoders::deg_##source_type_name##_to_##dest_type_name(source_type src, dest_type *dest, source_type min, source_type max, source_type offset) {                                   \
        dest_type_unsigned range = -1;                                                                                                                                                      \
        source_type normalizer = range / (max - min);                                                                                                                                       \
        *dest = (src - offset) * normalizer;                                                                                                                                                \
        return true;                                                                                                                                                                        \
    }                                                                                                                                                                                       \
    bool encoders::deg_##source_type_name##_to_##dest_type_name##_reserved(source_type src, dest_type *dest, source_type min, source_type max, source_type offset) {                        \
        if (!__IN_RANGE(src, min, max)) {                                                                                                                                                   \
            *dest = (dest_type_unsigned)-1;                                                                                                                                                 \
            return true;                                                                                                                                                                    \
        }                                                                                                                                                                                   \
        dest_type_unsigned range = -2;                                                                                                                                                      \
        source_type normalizer = range / (max - min);                                                                                                                                       \
        *dest = (src - offset) * normalizer;                                                                                                                                                \
        return true;                                                                                                                                                                        \
    }                                                                                                                                                                                       \
    bool encoders::deg_##source_type_name##_to_##dest_type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset) {          \
        if (!item.is_number()) {                                                                                                                                                            \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        source_type source = item.get<source_type>();                                                                                                                                       \
        dest_type value;                                                                                                                                                                    \
        if (!deg_##source_type_name##_to_##dest_type_name(source, &value, min, max, offset)) {                                                                                              \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        std::vector<uint8_t> _res;                                                                                                                                                          \
        if (!encoders::encode_##dest_type_name(value, &_res)) {                                                                                                                             \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        *res = std::move(_res);                                                                                                                                                             \
        return true;                                                                                                                                                                        \
    }                                                                                                                                                                                       \
    bool encoders::deg_##source_type_name##_to_##dest_type_name##_reserved_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset) { \
        source_type source = item.is_number() ? item.get<source_type>() : max + 1;                                                                                                          \
        dest_type value;                                                                                                                                                                    \
        if (!deg_##source_type_name##_to_##dest_type_name(source, &value, min, max, offset)) {                                                                                              \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        std::vector<uint8_t> _res;                                                                                                                                                          \
        if (!encoders::encode_##dest_type_name(value, &_res)) {                                                                                                                             \
            return false;                                                                                                                                                                   \
        }                                                                                                                                                                                   \
        *res = std::move(_res);                                                                                                                                                             \
        return true;                                                                                                                                                                        \
    }

__DEG_ENCODER(uint8, float, uint8_t, float, uint8_t);
__DEG_ENCODER(uint16, float, uint16_t, float, uint16_t);
__DEG_ENCODER(int16_be, float, int16_t, float, uint16_t);
__DEG_ENCODER(uint32, double, uint32_t, double, uint32_t);
__DEG_ENCODER(int32_be, double, int32_t, double, uint32_t);

#define __DEG_VARIABLE_ENCODER(dest_type_name, source_type_name, dest_type, source_type, dest_type_unsigned, byte_length)                                                                            \
    bool encoders::deg_variable_##source_type_name##_to_##dest_type_name(source_type src, uint8_t length, dest_type *dest, source_type min, source_type max, source_type offset) {                   \
        dest_type_unsigned range = length >= byte_length ? -1 : (1 << (length * 8)) - 1;                                                                                                             \
        dest_type normalizer = range / (max - min);                                                                                                                                                  \
        *dest = (src - offset) * normalizer;                                                                                                                                                         \
        return true;                                                                                                                                                                                 \
    }                                                                                                                                                                                                \
    bool encoders::deg_variable_##source_type_name##_to_##dest_type_name##_reserved(source_type src, uint8_t length, dest_type *dest, source_type min, source_type max, source_type offset) {        \
        if (!__IN_RANGE(src, min, max)) {                                                                                                                                                            \
            *dest = (dest_type_unsigned)-1;                                                                                                                                                          \
            return true;                                                                                                                                                                             \
        }                                                                                                                                                                                            \
        dest_type_unsigned range = length >= byte_length ? -2 : (1 << (length * 8)) - 2;                                                                                                             \
        dest_type normalizer = range / (max - min);                                                                                                                                                  \
        *dest = (src - offset) * normalizer;                                                                                                                                                         \
        return true;                                                                                                                                                                                 \
    }                                                                                                                                                                                                \
    bool encoders::deg_variable_##source_type_name##_to_##dest_type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset) {          \
        if (!item.is_number()) {                                                                                                                                                                     \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        source_type source = item.get<source_type>();                                                                                                                                                \
        dest_type value;                                                                                                                                                                             \
        if (!deg_variable_##source_type_name##_to_##dest_type_name(source, byte_length, &value, min, max, offset)) {                                                                                 \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        std::vector<uint8_t> _res;                                                                                                                                                                   \
        if (!encoders::encode_##dest_type_name(value, &_res)) {                                                                                                                                      \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        *res = _res;                                                                                                                                                                                 \
        return true;                                                                                                                                                                                 \
    }                                                                                                                                                                                                \
    bool encoders::deg_variable_##source_type_name##_to_##dest_type_name##_reserved_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset) { \
        source_type source = item.is_number() ? item.get<source_type>() : max + 1;                                                                                                                   \
        dest_type value;                                                                                                                                                                             \
        if (!deg_variable_##source_type_name##_to_##dest_type_name(source, byte_length, &value, min, max, offset)) {                                                                                 \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        std::vector<uint8_t> _res;                                                                                                                                                                   \
        if (!encoders::encode_##dest_type_name(value, &_res)) {                                                                                                                                      \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
        *res = std::move(_res);                                                                                                                                                                      \
        return true;                                                                                                                                                                                 \
    }

__DEG_VARIABLE_ENCODER(uint64, double, uint64_t, double, uint64_t, 8);
__DEG_VARIABLE_ENCODER(uint32, float, uint32_t, float, uint32_t, 4);