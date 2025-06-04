#include <iostream>

#include <klvparse/byte/ByteEncoders.h>
#include <klvparse/byte/ByteReader.h>

bool test_encode_ber(uint64_t val, bool debug = false) {
    std::vector<uint8_t> buf;
    klvparse::encoders::encode_ber(val, &buf);
    ByteReader reader(buf);
    if (debug) {
        std::cout << "value: " << val << std::endl;
        std::cout << "buffer: " << reader.debug() << std::endl;
    }
    uint64_t res = 0;
    if (reader.get_ber(&res)) {
        if (debug) {
            std::cout << "result: " << res << ", " << std::hex << res << std::endl;
        }
        return res == val;
    }
    return false;
}

int main() {
    for (uint64_t val = 0; val < ((uint64_t)-1); val++) {
        // for (uint64_t val = -2; val > 0; val--) {
        if (!test_encode_ber(val, false)) {
            test_encode_ber(val, true);
            break;
        }
        // test_encode_ber(val, true);
    }
    // test_encode_ber(256, true);
    return 0;
}