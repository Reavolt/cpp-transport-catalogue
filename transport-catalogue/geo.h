#pragma once

#include <cmath>
#include <cstdint>

namespace geo
{
    const static uint32_t EarthRadius = 6371000;

    struct Coordinates
    {
        double lat;
        double lng;

        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
    };
    double ComputeDistance(Coordinates from, Coordinates to);
}    // namespace geo