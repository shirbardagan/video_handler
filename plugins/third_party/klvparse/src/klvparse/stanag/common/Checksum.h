#pragma once

#include <klvparse/byte/ByteReader.h>

namespace klvparse {
    /**
     * checks if the last uint16 equals the sum of uint16 beforehand.
     * returns true if test was successfull,
     * if is_valid != NULL, stores if calculated checksum equals stored checksum
     * if calculated != NULL, stores calculated checksum value in *calculated;
     * if actual != NULL, stores actual checksum value in *actual;
     */
    bool _verify_checksum(ByteReader &buffer, bool *is_valid = NULL, uint16_t *calculated = NULL, uint16_t *actual = NULL);
} // namespace klvparse
