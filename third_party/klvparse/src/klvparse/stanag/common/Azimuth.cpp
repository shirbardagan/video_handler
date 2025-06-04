#include <klvparse/stanag/common/Azimuth.h>

using namespace klvparse;

Azimuth klvparse::degree_to_azimuth(double deg) {
    Azimuth result;
    result.a = deg;
    deg = deg - result.a;
    double temp = deg * 60;
    result.b = temp;
    deg = temp - result.b;
    temp = deg * 60;
    result.c = temp;
    // result.c = round((double)(temp * 10)) / 10;
    return result;
}

std::string klvparse::azimuth_to_string(Azimuth azi) {
    std::stringstream sstr;
    sstr << azi.a << "°" << azi.b << "'" << round((double)(azi.c * 10)) / 10;
    return sstr.str();
};

std::string klvparse::degree_to_azimuth_string(double deg) {
    return azimuth_to_string(degree_to_azimuth(deg));
}