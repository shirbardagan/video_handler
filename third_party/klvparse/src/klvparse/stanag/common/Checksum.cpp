#include <klvparse/stanag/common/Checksum.h>

bool klvparse::_verify_checksum(ByteReader &reader, bool *is_valid, uint16_t *calculated, uint16_t *actual) {
    uint16_t checksum_calculated = 0;
    uint64_t index = reader.get_index();
    uint8_t curr = 0;
    for (uint8_t i = 0; reader.get_remaining() > 2; i++) {
        if (!reader.get_ui8(&curr)) {
            reader.set_index(index);
            return false;
        }
        checksum_calculated += (curr << (8 * ((i + 1) % 2)));
    }
    uint16_t checksum_actual = 0;
    if (!reader.get_ui16(&checksum_actual)) {
        return false;
    }
    // std::cout << std::bitset<16>{checksum_calculated} << ", " << std::bitset<16>{checksum_actual} << std::endl;
    reader.set_index(index);
    if (calculated != NULL) {
        *calculated = checksum_calculated;
    }
    if (actual != NULL) {
        *actual = checksum_actual;
    }
    if (is_valid != NULL) {
        *is_valid = checksum_calculated == checksum_actual;
    }
    return true;
}