#include <fstream>

#include <klvparse/stanag/Stanag.h>
// #include <ByteReader.h>

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
    std::vector<uint8_t> buffer;
    get_file_buffer(argv[1], buffer);
    nl::json parsed = nl::json::parse(buffer);
    klvparse::Stanag stanag;
    std::vector<uint8_t> encoded;
    stanag.encode(parsed, &encoded);
    // stanag.ctx->verbosity = klvparse::Level::debug;
    nl::json parsed_again;
    stanag.parse(encoded, &parsed_again);
    nl::json diff = nl::json::diff(parsed, parsed_again);
    std::cout << "parsed: " << parsed.dump(2) << std::endl;
    std::cout << " @@ " << std::endl;
    std::cout << " @@ " << std::endl;
    std::cout << "parsed again: " << parsed_again.dump(2) << std::endl;
    std::cout << " @@ " << std::endl;
    std::cout << " @@ " << std::endl;
    // std::cout << "diff: " << diff.dump() << std::endl;
    return 0;
}