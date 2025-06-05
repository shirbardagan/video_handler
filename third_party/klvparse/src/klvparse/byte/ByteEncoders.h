#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <vector>

#define __BASIC_ENCODER_H(type_name, type, num_bytes) \
    bool encode_##type_name(type value, std::vector<uint8_t> *res);

#define __VARIABLE_ENCODER_H(type_name, type, num_bytes) \
    bool encode_variable_##type_name(type value, std::vector<uint8_t> *res);

#define __VARIABLE_ENCODER_COMPLEX_H(type_name, type, num_bytes) \
    bool encode_variable_##type_name(type value, std::vector<uint8_t> *res);

// #define __BE_SIMPLE_ENCODER_H(type_name, type, num_bytes) \
//     bool get_##type_name##_be(type &value);

// #define __BE_COMPLEX_ENCODER_H(type_name, type, num_bytes, container_bytes) \
//     bool get_##type_name##_be(type &value);

namespace klvparse {
    namespace encoders {
        uint8_t bytes_needed(uint64_t input);

        __BASIC_ENCODER_H(uint8, uint8_t, 1);
        __BASIC_ENCODER_H(uint16, uint16_t, 2);
        __BASIC_ENCODER_H(uint24, uint32_t, 3);
        __BASIC_ENCODER_H(uint32, uint32_t, 4);
        __BASIC_ENCODER_H(uint40, uint64_t, 5);
        __BASIC_ENCODER_H(uint48, uint64_t, 6);
        __BASIC_ENCODER_H(uint56, uint64_t, 7);
        __BASIC_ENCODER_H(uint64, uint64_t, 8);

        __BASIC_ENCODER_H(int8_be, int8_t, 1);
        __BASIC_ENCODER_H(int16_be, int16_t, 2);
        __BASIC_ENCODER_H(int24_be, int32_t, 3);
        __BASIC_ENCODER_H(int32_be, int32_t, 4);
        __BASIC_ENCODER_H(int40_be, int64_t, 5);
        __BASIC_ENCODER_H(int48_be, int64_t, 6);
        __BASIC_ENCODER_H(int56_be, int64_t, 7);
        __BASIC_ENCODER_H(int64_be, int64_t, 8);

        __VARIABLE_ENCODER_H(uint8, uint8_t, 1);
        __VARIABLE_ENCODER_H(uint16, uint16_t, 2);
        __VARIABLE_ENCODER_COMPLEX_H(uint24, uint32_t, 3);
        __VARIABLE_ENCODER_H(uint32, uint32_t, 4);
        __VARIABLE_ENCODER_COMPLEX_H(uint40, uint64_t, 5);
        __VARIABLE_ENCODER_COMPLEX_H(uint48, uint64_t, 6);
        __VARIABLE_ENCODER_COMPLEX_H(uint56, uint64_t, 7);
        __VARIABLE_ENCODER_H(uint64, uint64_t, 8);

        // __TO_VARIABLE_BE_H(int16_be, int16_t, uint16_t, 2);
        // __TO_VARIABLE_BE_H(int32_be, int32_t, uint32_t, 4);
        // __TO_VARIABLE_BE_H(int64_be, int64_t, uint64_t, 8);

        // __BE_SIMPLE_ENCODER_H(int8, int8_t, 1);
        // __BE_SIMPLE_ENCODER_H(int16, int16_t, 2);
        // __TO_BE_COMPLEX_H(int24, int32_t, 3, 4);
        // __BE_SIMPLE_ENCODER_H(int32, int32_t, 4);
        // __TO_BE_COMPLEX_H(int40, int64_t, 5, 8);
        // __TO_BE_COMPLEX_H(int48, int64_t, 6, 8);
        // __TO_BE_COMPLEX_H(int56, int64_t, 7, 8);
        // __BE_SIMPLE_ENCODER_H(int64, int64_t, 8);

        bool encode_ber(uint64_t value, std::vector<uint8_t> *res);
        bool encode_ber_oid(uint64_t value, std::vector<uint8_t> *res);
    }; // namespace encoders

} // namespace klvparse
