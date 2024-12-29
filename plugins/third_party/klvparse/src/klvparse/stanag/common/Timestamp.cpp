#include <klvparse/stanag/common/Timestamp.h>

using namespace klvparse;

void klvparse::timestamp_to_timespec(uint64_t timestamp, struct timespec *ts) {
    ts->tv_sec = timestamp / (int)1e6;
    ts->tv_nsec = (timestamp % (int)1e6) * 1e3;
}

#define STRFTIME_SIZE 100
std::string klvparse::timespec_to_string(struct timespec *ts) {
    char output[STRFTIME_SIZE];
    #if defined(WIN32)
        // struct tm *t = gmtime(&(ts->tv_sec));
        // int ret1 = strftime(output, STRFTIME_SIZE, "%Y-%m-%dT%H:%M:%S", t);
        struct tm t;
        if (gmtime_s(&t, &(ts->tv_sec)) == NULL) {
            throw std::runtime_error("gmtime_s failed");
        }
        int ret1 = strftime(output, STRFTIME_SIZE, "%Y-%m-%dT%H:%M:%S", &t);
    #else
        struct tm t;
        if (gmtime_r(&(ts->tv_sec), &t) == NULL) {
            throw std::runtime_error("gmtime_r failed");
        }
        int ret1 = strftime(output, STRFTIME_SIZE, "%Y-%m-%dT%H:%M:%S", &t);
    #endif
    // int ret2 = snprintf(output + ret1, STRFTIME_SIZE - ret1, ".%ldZ", ts->tv_nsec / (int)1e3);
    // int ret1 = strftime(output, STRFTIME_SIZE, "%d/%m/%Y-%H:%M:%S", &t);
    int ret2 = snprintf(output + ret1, STRFTIME_SIZE - ret1, ".%ld", ts->tv_nsec / (int)1e6);
    return output;
}

std::string klvparse::timestamp_to_string(uint64_t timestamp) {
    struct timespec ts;
    timestamp_to_timespec(timestamp, &ts);
    return timespec_to_string(&ts);
}
uint64_t klvparse::string_to_timestamp(std::string ts) {
    return 0;
}