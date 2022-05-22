#pragma once
#include "geo.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace domain
{
    enum class RouteType
    {
        NONE,
        CIRCLE,
        LINEAR
    };

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
        RouteType                route_type_{};

        Bus() = default;
        Bus(const std::string& name, const std::vector<const Stop*>& stops, RouteType route_type_);
    };

    struct BusInfo
    {
        std::string name_{};
        size_t      stops_count_      = 0;
        size_t      uniq_stops_count_ = 0;
        int         bus_length_       = 0;
        double      curvature_        = 0.0;
        bool        found_            = true;
    };

    struct StopInfo
    {
        std::string              name_{};
        std::vector<std::string> buses_name_{};
        bool                     found_ = true;
    };

    const int SIMPLE_NUMBER = 37;

    struct StopPtrHasher
    {
        size_t operator()(const std::pair<const domain::Stop*, const domain::Stop*>& p) const
        {
            return pointer_hasher(p.first) * SIMPLE_NUMBER + pointer_hasher(p.second) * SIMPLE_NUMBER * SIMPLE_NUMBER;
        }

        size_t operator()(const domain::Stop* p) const
        {
            return pointer_hasher(p) * SIMPLE_NUMBER;
        }

    private:
        std::hash<const void*> pointer_hasher{};
    };
}    // namespace domain