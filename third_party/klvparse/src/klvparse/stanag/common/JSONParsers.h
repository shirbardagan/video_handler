#pragma once

#include <klvparse/stanag/common/Context.h>

#include <klvparse/byte/ByteReader.h>
#include <klvparse/klv/KLV.h>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

namespace nlohmann {
    std::string ensure_dump(json j, int indent = -1);
};

namespace klvparse {
    namespace parsers {
        
#define __BASE_PARSER_H(GETTER, TYPE_NAME, TYPE) \
    bool TYPE_NAME##_parser(ByteReader &reader, nl::json *res)

#define __VARIABLE_PARSER_H(GETTER, TYPE_NAME, TYPE) \
    bool variable_##TYPE_NAME##_parser(ByteReader &reader, nl::json *res)

#define __DEG_PARSER_H(GETTER, SOURCE_TYPE_NAME, DEST_TYPE_NAME, SOURCE_TYPE, dest_TYPE, SOURCE_TYPE_unsigned)                                      \
    bool deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(SOURCE_TYPE src, dest_TYPE *dest, dest_TYPE min, dest_TYPE max, dest_TYPE offset);            \
    bool deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(SOURCE_TYPE src, dest_TYPE *dest, dest_TYPE min, dest_TYPE max, dest_TYPE offset); \
    bool deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_parser(ByteReader &reader, nl::json *res, dest_TYPE min, dest_TYPE max, dest_TYPE offset);  \
    bool deg_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved_parser(ByteReader &reader, nl::json *res, dest_TYPE min, dest_TYPE max, dest_TYPE offset, const char *reserved = "Reserved")

#define __DEG_VARIABLE_PARSER_H(GETTER, SOURCE_TYPE_NAME, DEST_TYPE_NAME, SOURCE_TYPE, dest_TYPE, SOURCE_TYPE_unsigned, byte_length)                                         \
    bool deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME(SOURCE_TYPE src, uint8_t length, dest_TYPE *dest, dest_TYPE min, dest_TYPE max, dest_TYPE offset);            \
    bool deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved(SOURCE_TYPE src, uint8_t length, dest_TYPE *dest, dest_TYPE min, dest_TYPE max, dest_TYPE offset); \
    bool deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_parser(ByteReader &reader, nl::json *res, dest_TYPE min, dest_TYPE max, dest_TYPE offset);                  \
    bool deg_variable_##SOURCE_TYPE_NAME##_to_##DEST_TYPE_NAME##_reserved_parser(ByteReader &reader, nl::json *res, dest_TYPE min, dest_TYPE max, dest_TYPE offset, const char *reserved);

        bool bytes_length_parser(ByteReader &reader, size_t length, nl::json *j);
        bool bytes_parser(ByteReader &reader, nl::json *j);
        bool uuid_parser(ByteReader &reader, nl::json *j);

        __BASE_PARSER_H(get_ui8, uint8, uint8_t);
        __BASE_PARSER_H(get_ui16, uint16, uint16_t);
        __BASE_PARSER_H(get_ui24, uint24, uint32_t);
        __BASE_PARSER_H(get_ui32, uint32, uint32_t);
        __BASE_PARSER_H(get_ui40, uint40, uint64_t);
        __BASE_PARSER_H(get_ui48, uint48, uint64_t);
        __BASE_PARSER_H(get_ui56, uint56, uint64_t);
        __BASE_PARSER_H(get_ui64, uint64, uint64_t);
        __BASE_PARSER_H(get_i8, int8, int8_t);
        __BASE_PARSER_H(get_i16, int16, int16_t);
        __BASE_PARSER_H(get_i32, int32, int32_t);
        __BASE_PARSER_H(get_i64, int64, int64_t);

        __VARIABLE_PARSER_H(get_variable_ui16, uint16, uint16_t);
        __VARIABLE_PARSER_H(get_variable_ui24, uint24, uint32_t);
        __VARIABLE_PARSER_H(get_variable_ui32, uint32, uint32_t);
        __VARIABLE_PARSER_H(get_variable_ui40, uint40, uint64_t);
        __VARIABLE_PARSER_H(get_variable_ui48, uint48, uint64_t);
        __VARIABLE_PARSER_H(get_variable_ui56, uint56, uint64_t);
        __VARIABLE_PARSER_H(get_variable_ui64, uint64, uint64_t);

        bool string_parser(ByteReader &reader, nl::json *res);
        bool number_or_string_parser(ByteReader &reader, nl::json *res);

        std::string timestamp_from_uint64(uint64_t time);

        bool timestamp_parser(ByteReader &reader, nl::json *res);

        float_t uint8_deg_to_float(uint8_t val, float_t min = 0, float_t max = 0, float_t offset = 0);
        double_t int32_deg_to_double(int32_t val, double_t min = 0, double_t max = 0, double_t offset = 0);

        __DEG_PARSER_H(get_ui8, uint8, float, uint8_t, float, uint8_t);
        __DEG_PARSER_H(get_ui16, uint16, float, uint16_t, float, uint16_t);
        __DEG_PARSER_H(get_i16, int16, float, int16_t, float, uint16_t);
        __DEG_PARSER_H(get_ui32, uint32, double, uint32_t, double, uint32_t);
        __DEG_PARSER_H(get_i32, int32, double, int32_t, double, uint32_t);

        __DEG_VARIABLE_PARSER_H(get_variable_ui64, uint64, double, uint64_t, double, uint64_t, 8);
        __DEG_VARIABLE_PARSER_H(get_variable_ui32, uint32, float, uint32_t, float, uint32_t, 4);

        bool general_klv_parser(ByteReader &reader, nl::json *res);
    } // namespace parsers
} // namespace klvparse