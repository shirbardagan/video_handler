#include <iostream>

#include <klvparse/stanag/StanagParsers.h>

int main() {
    const uint8_t data[] = {
        0x03,
        0x12,
        0x00,
        0x00,
        0x0F,
        0x56,
        0x49,
        0x53,
        0x20,
        0x4E,
        0x6F,
        0x73,
        0x65,
        0x20,
        0x43,
        0x61,
        0x6D,
        0x65,
        0x72,
        0x61,
        0x15,
        0x01,
        0x00,
        0x12,
        0x41,
        0x43,
        0x4D,
        0x45,
        0x20,
        0x56,
        0x49,
        0x53,
        0x20,
        0x4D,
        0x6F,
        0x64,
        0x65,
        0x6C,
        0x20,
        0x31,
        0x32,
        0x33,
        0x14,
        0x02,
        0x00,
        0x11,
        0x41,
        0x43,
        0x4D,
        0x45,
        0x20,
        0x49,
        0x52,
        0x20,
        0x4D,
        0x6F,
        0x64,
        0x65,
        0x6C,
        0x20,
        0x34,
        0x35,
        0x36,
    };
    nl::json res;
    ByteReader reader(data, sizeof(data));
    auto ctx = std::make_shared<klvparse::Context>();
    klvparse::parsers::stanag_payload_list_parser(reader, &res, ctx);
    std::cout << res.dump() << std::endl;
    return 0;
}