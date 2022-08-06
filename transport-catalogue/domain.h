#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

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
