#include <klvparse/stanag/common/JSONPrettyUgly.h>

using namespace klvparse;
namespace pretty = klvparse::pretty;
bool pretty::timestamp_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    *res = timestamp_to_string(item.get<uint64_t>());
    return true;
}

bool pretty::azimuth_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    *res = degree_to_azimuth_string(item.get<double>());
    return true;
}

bool pretty::latitude_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    *res = degree_to_azimuth_string(item.get<double>()) + "N";
    return true;
}

bool pretty::longitude_pretty(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    *res = degree_to_azimuth_string(item.get<double>()) + "E";
    return true;
}

namespace ugly = klvparse::ugly;
bool ugly::timestamp_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool ugly::azimuth_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool ugly::latitude_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}
bool ugly::longitude_ugly(const nl::json &item, nl::json *res, std::shared_ptr<Context> ctx) {
    return false;
}