#pragma once

#include <math.h>
#include <sstream>
#include <stdint.h>

namespace klvparse {
    typedef struct _Azimuth
    {
        int16_t a = 0;
        int16_t b = 0;
        double c = 0;
    } Azimuth;

    Azimuth degree_to_azimuth(double deg);
    std::string azimuth_to_string(Azimuth azi);
    std::string degree_to_azimuth_string(double deg);
} // namespace klvparse