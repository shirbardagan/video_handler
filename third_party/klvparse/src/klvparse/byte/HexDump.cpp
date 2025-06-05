
#include <klvparse/byte/HexDump.h>

#define HEX_SIZE 4
#define LINE_SIZE 94


// based on gst_util_dump_mem_offset
std::string hexdump(const uint8_t *mem, size_t size, size_t offset) {
    size_t i, j;
    std::stringstream out;
    i = j = 0;
    char hex[HEX_SIZE] = {};
    char line[LINE_SIZE] = {};
    std::stringstream string; //(50);
    std::stringstream chars;  //(18)
    while (i < size) {
        if (isascii(mem[i]) && isprint(mem[i])) {
            chars << (char)mem[i];
        } else {
            chars << ".";
        }
        snprintf(hex, HEX_SIZE, "%02x ", mem[i]);
        string << hex;

        j++;
        i++;

        if (j == 16 || i == size) {
            std::string _string = string.str();
            std::string _chars = chars.str();
            snprintf(line, LINE_SIZE, "%08lx (%p): %-48.48s %-16.16s\n",
                     i - j + offset, mem + i - j, _string.c_str(), _chars.c_str());
            out << line;
            j = 0;

            string.str("");
            chars.str("");
        }
    }
    return out.str();
}