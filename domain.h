#pragma once


#include <string_view>
#include <vector>
#include <string>
#include <set>
#include "geo.h"

namespace domain {
    struct Stop
    {
    public:
        size_t edge_id;
        geo::Coordinates coords;
        std::string name;

        explicit Stop(size_t id, geo::Coordinates coordinates, std::string name) 
            : edge_id(id)
            , coords(coordinates)
            , name(name) {
        }
    };

    struct Bus
    {
        std::string bus_number;
        std::vector<Stop*> stops = {};
        bool is_roundtrip = false;

    };

} // namespace domain
