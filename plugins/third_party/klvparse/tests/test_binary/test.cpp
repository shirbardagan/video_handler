#include <fstream>

#include <klvparse/stanag/Stanag.h>

bool get_file_buffer(const char *path, std::vector<uint8_t> &buffer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    // file.seekg(0,std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer = std::vector<uint8_t>(size);
    if (file.read((char *)buffer.data(), size))
        return true;
    return false;
}

int main(int argc, char const *argv[]) {
    klvparse::Stanag parser;
    parser.ctx->verbosity = klvparse::Level::debug;
    // parser.ctx->verbosity = klvparse::Level::debug;
    nl::json parsed;
    std::vector<uint8_t> buffer;

    get_file_buffer(argv[1], buffer);
    parser.parse(buffer, &parsed);
    nl::json pretty;
    parser.pretty(parsed, &pretty);
    

    // get_file_buffer("/home/pavel/Desktop/klv/elbit_test_klv/0.bin", buffer);
    // parser.parse(buffer, &parsed, 14, false);

    // get_file_buffer("/home/pavel/Desktop/klv/klvparse-js/tests/parse/elbit1/unparsed.klv", buffer);
    // parser.parse(buffer, &parsed);

    // get_file_buffer("/home/pavel/Desktop/klv/klvparse-js/tests/parse/iai1/unparsed.klv", buffer);
    // parser.parse(buffer, &parsed, 5);

    std::cout << parsed.dump(2, (char)32, false, nl::json::error_handler_t::replace) << std::endl;
    std::cout << "@" << std::endl;
    std::cout << pretty.dump(2, (char)32, false, nl::json::error_handler_t::replace) << std::endl;
    // std::vector<uint8_t> encoded;
    // if (parser.encode(parsed, &encoded)) {
    //     std::cout << ByteReader(encoded).dump() << std::endl;
    //     parser.parse(encoded, &parsed);
    //     std::cout << parsed.dump(2, (char)32, false, nl::json::error_handler_t::replace) << std::endl;
    // } else {
    //     std::cout << "failed" << std::endl;
    // }

    // std::vector<uint8_t> buffer(16);
    // buffer[0] = 0xff;
    // // buffer[1] = 0x80;
    // buffer[1] = 255;
    // buffer[2] = 0x80;
    // // buffer[3] = 0x80;

    // ByteReader r = ByteReader(buffer);
    // std::cout << r.get_remaining() << std::endl;
    // // uint16_t res = 0;
    // // r.get_uint16(res);
    // int32_t res = 0;
    // r.get_int24_be(res);
    // // int16_t res = 0;
    // // r.get_int16_be(res);
    // std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex << res << std::endl;
    // std::cout << "0b" << std::bitset<64>(res) << std::endl;
    // std::cout << std::dec <<  res << std::endl;
    return 0;
}