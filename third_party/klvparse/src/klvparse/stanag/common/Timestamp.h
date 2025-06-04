#pragma once

#include <chrono>
#include <sstream>
#include <stdint.h>

namespace klvparse {
    void timestamp_to_timespec(uint64_t timestamp, struct timespec *ts);
    std::string timespec_to_string(struct timespec *ts);
    std::string timestamp_to_string(uint64_t timestamp);
    uint64_t string_to_timestamp(std::string ts);
} // namespace klvparse