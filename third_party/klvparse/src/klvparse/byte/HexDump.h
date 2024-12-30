#include <cstring>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

std::string hexdump(const uint8_t *mem, size_t size, size_t offset = 0);