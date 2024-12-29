#include <iostream>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/byte/ByteReader.h>

// bool __to_uint64(uint64_t &value, std::vector<uint8_t> *res) {
//     std::vector<uint8_t> _res(8);
//     for (uint64_t i = 0; i < 8; i++) {
//         _res[i] = value >> ((8 * (8 - i))) & 0xff;
//     }
//     *res = std::move(_res);
//     return true;
// }
bool test_to_uint(uint64_t val, bool debug = false) {
    std::vector<uint8_t> buf;
    klvparse::encoders::encode_uint64(val, &buf);
    // __to_uint64(val, &buf);
    ByteReader reader(buf);
    if (debug) {
        std::cout << "value: " << val << std::endl;
        std::cout << "buffer: " << reader.debug() << std::endl;
    }
    uint64_t res = 0;
    if (reader.get_ui64(&res)) {
        if (debug) {
            std::cout << "result: " << res << std::endl;
        }
        return res == val;
    }
    return false;
}

int main() {
    for (uint64_t val = 0; val < ((uint64_t)-1); val++) {
        // for (uint64_t val = -1; val > 0; val--) {
        if (!test_to_uint(val, false)) {
            test_to_uint(val, true);
            break;
        }
        // test_to_uint(val, true);
    }
    // test_to_uint(200, true);
    return 0;
}