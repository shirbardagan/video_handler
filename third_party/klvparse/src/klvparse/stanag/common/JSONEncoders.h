#pragma once

#include <klvparse/stanag/common/Context.h>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/klv/KLV.h>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

namespace klvparse {
    namespace encoders {
        bool bytes_length_encoder(const nl::json &j, size_t length, std::vector<uint8_t> *res);
        bool bytes_encoder(const nl::json &j, std::vector<uint8_t> *res);
        bool uuid_encoder(const nl::json &j, std::vector<uint8_t> *res);
        bool timestamp_encoder(const nl::json &item, std::vector<uint8_t> *res);
        bool string_encoder(const nl::json &item, std::vector<uint8_t> *res);
        bool general_klv_encoder(const nl::json &item, std::vector<uint8_t> *res);

#define __BASE_ENCODER_H(type_name, type) \
    bool type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res);

#define __BASE_VARIABLE_H(type_name, type) \
    bool variable_##type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res);

#define __DEG_ENCODER_H(dest_type_name, source_type_name, dest_type, source_type, dest_type_unsigned)                                                                   \
    bool deg_##source_type_name##_to_##dest_type_name(source_type src, dest_type *dest, source_type min, source_type max, source_type offset);                          \
    bool deg_##source_type_name##_to_##dest_type_name##_reserved(source_type src, dest_type *dest, source_type min, source_type max, source_type offset);               \
    bool deg_##source_type_name##_to_##dest_type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset); \
    bool deg_##source_type_name##_to_##dest_type_name##_reserved_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset);

#define __DEG_VARIABLE_ENCODER_H(dest_type_name, source_type_name, dest_type, source_type, dest_type_unsigned, byte_length)                                                        \
    bool deg_variable_##source_type_name##_to_##dest_type_name(source_type src, uint8_t length, dest_type *dest, source_type min, source_type max, source_type offset);            \
    bool deg_variable_##source_type_name##_to_##dest_type_name##_reserved(source_type src, uint8_t length, dest_type *dest, source_type min, source_type max, source_type offset); \
    bool deg_variable_##source_type_name##_to_##dest_type_name##_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset);   \
    bool deg_variable_##source_type_name##_to_##dest_type_name##_reserved_encoder(const nl::json &item, std::vector<uint8_t> *res, source_type min, source_type max, source_type offset);

        __BASE_ENCODER_H(uint8, uint8_t);
        __BASE_ENCODER_H(uint16, uint16_t);
        __BASE_ENCODER_H(uint24, uint32_t);
        __BASE_ENCODER_H(uint32, uint32_t);
        __BASE_ENCODER_H(uint40, uint64_t);
        __BASE_ENCODER_H(uint48, uint64_t);
        __BASE_ENCODER_H(uint56, uint64_t);
        __BASE_ENCODER_H(uint64, uint64_t);

        __BASE_VARIABLE_H(uint8, uint8_t);
        __BASE_VARIABLE_H(uint16, uint16_t);
        __BASE_VARIABLE_H(uint24, uint32_t);
        __BASE_VARIABLE_H(uint32, uint32_t);
        __BASE_VARIABLE_H(uint40, uint64_t);
        __BASE_VARIABLE_H(uint48, uint64_t);
        __BASE_VARIABLE_H(uint56, uint64_t);
        __BASE_VARIABLE_H(uint64, uint64_t);

        __DEG_ENCODER_H(uint8, float, uint8_t, float, uint8_t);
        __DEG_ENCODER_H(uint16, float, uint16_t, float, uint16_t);
        __DEG_ENCODER_H(int16_be, float, int16_t, float, uint16_t);
        __DEG_ENCODER_H(uint32, double, uint32_t, double, uint32_t);
        __DEG_ENCODER_H(int32_be, double, int32_t, double, uint32_t);
        __DEG_VARIABLE_ENCODER_H(uint64, double, uint64_t, double, uint64_t, 8);
        __DEG_VARIABLE_ENCODER_H(uint32, float, uint32_t, float, uint32_t, 4);
    } // namespace encoders
} // namespace klvparse