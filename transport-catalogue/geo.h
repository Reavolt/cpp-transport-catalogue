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

        bool operator==(const Coordinates& other) const
        {
            return lat == other.lat && lng == other.lng;
        }

        bool operator!=(const Coordinates& other) const
        {
            return !(*this == other);
        }
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}    // namespace geo