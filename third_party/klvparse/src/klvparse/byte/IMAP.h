#pragma once

// #include <gst/gst.h>
#include <math.h>
#include <iostream>

#define __POSITIVE_INFINITY_HIGH_BYTE 0xC8
#define __NEGATIVE_INFINITY_HIGH_BYTE 0xE8
#define __POSITIVE_QUIET_NAN_HIGH_BYTE 0xD0
#define __NEGATIVE_QUIET_NAN_HIGH_BYTE 0xF0
#define __HIGH_BITS_MASK 0xF8

// #define __IMAPB_FORWARD_HEADER(__TYPE_OUT, __BYTE_LENGTH) \
//     gboolean IMAPB_to_##__TYPE_OUT(double x, __TYPE_OUT *res, double min, double max, uint8_t byte_length = __BYTE_LENGTH);

// // forawrd should be int / uint
// #define __IMAPB_FORWARD(__TYPE_OUT, __BYTE_LENGTH)                                                             \
//     gboolean IMAPB_to_##__TYPE_OUT(double x, __TYPE_OUT *res, double min, double max, uint8_t byte_length) { \
//         uint64_t bPow = ceil(log2(max - min));                                                                  \
//         uint64_t dPow = byte_length >= 8 ? -1 : (1 << (byte_length * 8)) - 1;                                   \
//         uint64_t sF = 1 << (dPow - bPow);                                                                       \
//         double zOffset = min < 0 && max > 0 ? (sF * min) - floor(sF * min) : 0;                               \
//         /*implement special float*/                                                                            \
//         *res = (__TYPE_OUT)((sF * (x - min) + zOffset));                                                       \
//         return true;                                                                                           \
//     }

// #define __IMAPB_BACKWARD_HEADER(__TYPE_OUT, __BYTE_LENGTH) \
//     gboolean IMAPB_to_##__TYPE_OUT(uint64_t y, __TYPE_OUT *res, double min, double max, uint8_t byte_length = __BYTE_LENGTH);

// // backward should be float / double
// #define __IMAPB_BACKWARD(__TYPE_OUT, __BYTE_LENGTH)                                                            \
//     gboolean IMAPB_to_##__TYPE_OUT(uint64_t y, __TYPE_OUT *res, double min, double max, uint8_t byte_length) { \
//         uint64_t bPow = ceil(log2(max - min));                                                                  \
//         uint64_t dPow = byte_length >= 8 ? -1 : (1 << (byte_length * 8)) - 1;                                   \
//         uint64_t sF = 1 << (dPow - bPow);                                                                       \
//         uint64_t sR = 1 << (bPow - dPow);                                                                       \
//         double zOffset = min < 0 && max > 0 ? (sF * min) - floor(sF * min) : 0;                               \
//         /*implement special int*/                                                                              \
//         *res = (__TYPE_OUT)((sR * (y - zOffset) + min));                                                       \
//         return true;                                                                                           \
//     }

#define __IMAPB_FORWARD_HEADER(__TYPE_NAME, __TYPE_OUT, __BYTE_LENGTH) \
    __TYPE_OUT IMAPB_to_##__TYPE_NAME(double x, double min, double max, uint8_t byte_length = __BYTE_LENGTH);

// forawrd should be int / uint
#define __IMAPB_FORWARD(__TYPE_NAME, __TYPE_OUT, __BYTE_LENGTH)                                \
    __TYPE_OUT IMAPB_to_##__TYPE_NAME(double x, double min, double max, uint8_t byte_length) { \
        int64_t bPow = ceil(log2(max - min));                                                  \
        /*uint64_t dPow = byte_length >= 8 ? -1 : (1 << (byte_length * 8)) - 1;*/              \
        int64_t dPow = 8 * byte_length - 1;                                                    \
        double sF = pow(2, dPow - bPow);                                                       \
        double sR = pow(2, bPow - dPow);                                                       \
        double zOffset = min < 0 && max > 0 ? (sF * min) - floor(sF * min) : 0;                \
        /*implement special float*/                                                            \
        return (__TYPE_OUT)((sF * (x - min) + zOffset));                                       \
    }

#define __IMAPB_BACKWARD_HEADER(__TYPE_NAME, __TYPE_OUT, __BYTE_LENGTH) \
    __TYPE_OUT IMAPB_to_##__TYPE_NAME(uint64_t y, double min, double max, uint8_t byte_length = __BYTE_LENGTH);

// backward should be float / double
#define __IMAPB_BACKWARD(__TYPE_NAME, __TYPE_OUT, __BYTE_LENGTH)                                 \
    __TYPE_OUT IMAPB_to_##__TYPE_NAME(uint64_t y, double min, double max, uint8_t byte_length) { \
        int64_t bPow = ceil(log2(max - min));                                                    \
        /*uint64_t dPow = byte_length >= 8 ? -1 : (1 << (byte_length * 8)) - 1;*/                \
        int64_t dPow = 8 * byte_length - 1;                                                      \
        double sF = pow(2, dPow - bPow);                                                         \
        double sR = pow(2, bPow - dPow);                                                         \
        double zOffset = min < 0 && max > 0 ? (sF * min) - floor(sF * min) : 0;                  \
        /*implement special int*/                                                                \
        return (__TYPE_OUT)(((sR * (y - zOffset)) + min));                                       \
    }

__IMAPB_FORWARD_HEADER(uint8, uint8_t, 1)
__IMAPB_FORWARD_HEADER(int8, int8_t, 1)
__IMAPB_FORWARD_HEADER(uint16, uint16_t, 2)
__IMAPB_FORWARD_HEADER(int16, int16_t, 2)
__IMAPB_FORWARD_HEADER(uint32, uint32_t, 4)
__IMAPB_FORWARD_HEADER(int32, int32_t, 4)
__IMAPB_FORWARD_HEADER(uint64, uint64_t, 8)
__IMAPB_FORWARD_HEADER(int64, int64_t, 8)

__IMAPB_BACKWARD_HEADER(float, float, 4)
__IMAPB_BACKWARD_HEADER(double, double, 8)
