#include <klvparse/byte/ByteEncoders.h>
// #include <cmath>
#include <math.h>

using namespace klvparse;
namespace encoders = klvparse::encoders;

#define __BASIC_ENCODER(type_name, type, num_bytes)                            \
    bool encoders::encode_##type_name(type value, std::vector<uint8_t> *res) { \
        std::vector<uint8_t> _res(num_bytes);                                  \
        for (uint64_t i = 0; i < num_bytes; i++) {                             \
            _res[i] = value >> (8 * (num_bytes - i - 1)) & 0xff;               \
        }                                                                      \
        *res = std::move(_res);                                                \
        return true;                                                           \
    }

__BASIC_ENCODER(uint8, uint8_t, 1);
__BASIC_ENCODER(uint16, uint16_t, 2);
__BASIC_ENCODER(uint24, uint32_t, 3);
__BASIC_ENCODER(uint32, uint32_t, 4);
__BASIC_ENCODER(uint40, uint64_t, 5);
__BASIC_ENCODER(uint48, uint64_t, 6);
__BASIC_ENCODER(uint56, uint64_t, 7);
__BASIC_ENCODER(uint64, uint64_t, 8);

__BASIC_ENCODER(int8_be, int8_t, 1);
__BASIC_ENCODER(int16_be, int16_t, 2);
__BASIC_ENCODER(int24_be, int32_t, 3);
__BASIC_ENCODER(int32_be, int32_t, 4);
__BASIC_ENCODER(int40_be, int64_t, 5);
__BASIC_ENCODER(int48_be, int64_t, 6);
__BASIC_ENCODER(int56_be, int64_t, 7);
__BASIC_ENCODER(int64_be, int64_t, 8);

// static int field_set(uint64_t input) {
//     uint64_t field = input * 0x20406080a0c0e1ULL;
//     return (field >> 60) & 15;
// }
uint8_t encoders::bytes_needed(uint64_t input) {
    uint8_t n = 0;
    while (input != 0) {
        input >>= 8;
        n++;
    }
    return n;
}

#define __VARIABLE_ENCODER(type_name, type, num_bytes)                                  \
    bool encoders::encode_variable_##type_name(type value, std::vector<uint8_t> *res) { \
        uint8_t length = bytes_needed(value);                                           \
        std::vector<uint8_t> _res(length);                                              \
        for (uint8_t i = 0; i < length; i++) {                                          \
            _res[i] = value >> (8 * (length - i - 1)) & 0xff;                           \
        }                                                                               \
        *res = std::move(_res);                                                         \
        return true;                                                                    \
    }

#define __VARIABLE_ENCODER_COMPLEX(type_name, type, num_bytes)                          \
    bool encoders::encode_variable_##type_name(type value, std::vector<uint8_t> *res) { \
        uint8_t length = bytes_needed(value);                                           \
        if (length > num_bytes) {                                                       \
            return false;                                                               \
        }                                                                               \
        std::vector<uint8_t> _res(length);                                              \
        for (uint8_t i = 0; i < length; i++) {                                          \
            _res[i] = value >> (8 * (length - i - 1)) & 0xff;                           \
        }                                                                               \
        *res = std::move(_res);                                                         \
        return true;                                                                    \
    }

__VARIABLE_ENCODER(uint8, uint8_t, 1);
__VARIABLE_ENCODER(uint16, uint16_t, 2);
__VARIABLE_ENCODER_COMPLEX(uint24, uint32_t, 3);
__VARIABLE_ENCODER(uint32, uint32_t, 4);
__VARIABLE_ENCODER_COMPLEX(uint40, uint64_t, 5);
__VARIABLE_ENCODER_COMPLEX(uint48, uint64_t, 6);
__VARIABLE_ENCODER_COMPLEX(uint56, uint64_t, 7);
__VARIABLE_ENCODER(uint64, uint64_t, 8);

// __TO_VARIABLE_BE(int16_be, int16_t, uint16_t, 2);
// __TO_VARIABLE_BE(int32_be, int32_t, uint32_t, 4);
// __TO_VARIABLE_BE(int64_be, int64_t, uint64_t, 8);

// __TO_BE_SIMPLE(int8, int8_t, 1);
// __TO_BE_SIMPLE(int16, int16_t, 2);
// __TO_BE_COMPLEX(int24, int32_t, 3, 4);
// __TO_BE_SIMPLE(int32, int32_t, 4);
// __TO_BE_COMPLEX(int40, int64_t, 5, 8);
// __TO_BE_COMPLEX(int48, int64_t, 6, 8);
// __TO_BE_COMPLEX(int56, int64_t, 7, 8);
// __TO_BE_SIMPLE(int64, int64_t, 8);

bool encoders::encode_ber(uint64_t value, std::vector<uint8_t> *res) {
    if (value < 0x80) {
        std::vector<uint8_t> _res(1);
        _res[0] = (uint8_t)value;
        *res = std::move(_res);
        return true;
    }
    // uint8_t size = 1 + (value / 0xff);
    uint64_t nbits = (uint64_t)ceil(log2(value + 1));
    uint8_t nbytes = (uint8_t)ceil(((double_t)nbits) / 8);
    std::vector<uint8_t> _res(nbytes + 1);
    _res[0] = 0x80 | nbytes;
    for (uint64_t i = 0; i < nbytes; i++) {
        // _res[i + 1] = value >> ((8 * (nbytes - 8)) - 1) & 0xff;
        _res[i + 1] = (value >> ((nbytes - 1 - i) * 8)) & 0xff;
    }
    *res = std::move(_res);
    return true;
}

bool encoders::encode_ber_oid(uint64_t value, std::vector<uint8_t> *res) {
    if (value < 0x80) {
        std::vector<uint8_t> _res(1);
        _res[0] = (uint8_t)value;
        *res = std::move(_res);
        return true;
    }
    // uint8_t size = 1 + (value / 0xff);
    uint64_t nbits = (uint64_t)ceil(log2(value + 1));
    uint8_t nbytes = (uint8_t)ceil(((double_t)nbits) / 7);
    std::vector<uint8_t> _res(nbytes);
    for (uint64_t i = 0; i < nbytes - 1; i++) {
        _res[i] = 0x80 | (value >> ((nbytes - 1 - i) * 7)) & 0x7f;
    }
    _res[nbytes - 1] = value & 0x7f;
    *res = std::move(_res);
    return true;
}