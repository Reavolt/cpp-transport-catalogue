#pragma once

#include "detail.h"
#include "geo.h"

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue
{
    struct Stop
    {
        std::string name_{};
        Coordinates coordinates_{};

        Stop() = default;
        Stop(const std::string& name, const Coordinates& coordinates) :
            name_(name),
            coordinates_(coordinates)
        {
        }
    };

    struct Bus
    {
        std::string              name_{};
        std::vector<const Stop*> stops_{};

        Bus() = default;
        Bus(const std::string& name, const std::vector<const Stop*>& stops) :
            name_(name),
            stops_(stops)
        {
        }
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

    class TransportCatalogue
    {
        using StopNameToBus =
          std::unordered_map<const Stop*, std::unordered_set<Bus*>, detail::StopPtrHasher>;
        using StopsToDistance = std::unordered_map<std::pair<Stop*, Stop*>, int, detail::StopPtrHasher>;
        using StopnameToStop = std::unordered_map<std::string_view, Stop*>;
        using BusnameToBus   = std::unordered_map<std::string_view, Bus*>;

    private:    // types
        std::deque<Stop> stops_{};
        StopnameToStop   stopname_to_stop_{};
        StopNameToBus    stopname_to_bus_{};

        std::deque<Bus> buses_{};
        BusnameToBus    busname_to_bus_{};

        StopsToDistance stops_to_distance_{};

    public:    //methods
        void                AddStop(const Stop& stop_data);
        [[nodiscard]] Stop* FindStop(const std::string_view name) const;
        void                AddBus(const Bus& bus_data);
        [[nodiscard]] Bus*  FindBus(const std::string_view name) const;

        void              SetDistanceBetweenStops(const std::string_view from_stop,
                                                  const std::string_view to_stop,
                                                  const int              distance);
        [[nodiscard]] int GetDistanceBetweenStops(const std::string_view from_stop,
                                                  const std::string_view to_stop) const;

        [[nodiscard]] BusInfo  GetBusInfo(const std::string_view bus_name) const;
        [[nodiscard]] StopInfo GetStopInfo(const std::string_view bus_name) const;

    private:    //methods
        [[nodiscard]] size_t GetUniqStopsCount(const std::vector<const Stop*> bus_stops) const;
        [[nodiscard]] int    GetRouteLength(const std::vector<const Stop*> bus_stops) const;
        [[nodiscard]] double GetRouteDistance(const std::vector<const Stop*> bus_stops) const;
    };
}    // namespace transport_catalogue