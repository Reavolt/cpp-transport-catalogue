#include "geo.h"

namespace geo
{
    double ComputeDistance(Coordinates from, Coordinates to)
    {
        if(from == to)
        {
            return 0;
        }

        static const double dr = 3.1415926535 / 180.;
        return std::acos(std::sin(from.lat * dr) * std::sin(to.lat * dr) +
                         std::cos(from.lat * dr) * std::cos(to.lat * dr) * std::cos(std::abs(from.lng - to.lng) * dr)) *
               EarthRadius;
    }

    bool Coordinates::operator==(const Coordinates& other) const
    {
        return lat == other.lat && lng == other.lng;
    }

    bool Coordinates::operator!=(const Coordinates& other) const
    {
        return !(*this == other);
    }
}    // namespace geo