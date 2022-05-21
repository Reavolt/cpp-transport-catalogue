#pragma once
#include "geo.h"

#include <optional>
#include <string_view>
#include <vector>

namespace domain
{
    struct Stop
    {
        std::string      name_{};
        geo::Coordinates coordinates_{};

        Stop() = default;
        Stop(const std::string& name, const geo::Coordinates& coordinates);
    };

    struct Bus
    {
        std::string              name_{};
        std::vector<const Stop*> stops_{};

        Bus() = default;
        Bus(const std::string& name, const std::vector<const Stop*>& stops);
    };

    struct BusInfo
    {
        std::string name_{};
        size_t      stops_count_      = 0;
        size_t      uniq_stops_count_ = 0;
        int         bus_length_       = 0;
        double      curvature_        = 0.0;
    };

    struct StopInfo
    {
        std::string              name_{};
        std::vector<std::string> buses_name_{};
        bool                     not_found_ = false;
    };
}    // namespace domain