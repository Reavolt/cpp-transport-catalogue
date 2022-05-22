#pragma once

#include "domain.h"
#include "geo.h"

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue
{
    class TransportCatalogue
    {
        using StopNameToBus = std::unordered_map<const domain::Stop*, std::unordered_set<domain::Bus*>, domain::StopPtrHasher>;
        using StopsToDistance = std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, int, domain::StopPtrHasher>;
        using StopnameToStop  = std::unordered_map<std::string_view, domain::Stop*>;
        using BusnameToBus    = std::unordered_map<std::string_view, domain::Bus*>;

    private:    // types
        std::deque<domain::Stop> stops_{};
        StopnameToStop           stopname_to_stop_{};
        StopNameToBus            stopname_to_bus_{};

        std::deque<domain::Bus> buses_{};
        BusnameToBus            busname_to_bus_{};

        StopsToDistance stops_to_distance_{};

    public:    //methods
        void                        AddStop(const domain::Stop& stop_data);
        [[nodiscard]] domain::Stop* FindStop(const std::string_view name) const;
        void                        AddBus(const domain::Bus& bus_data);
        [[nodiscard]] domain::Bus*  FindBus(const std::string_view name) const;

        void SetDistanceBetweenStops(const std::string_view from_stop, const std::string_view to_stop, const int distance);
        [[nodiscard]] int GetDistanceBetweenStops(const std::string_view from_stop, const std::string_view to_stop) const;

        [[nodiscard]] domain::BusInfo  GetBusInfo(const std::string_view bus_name) const;
        [[nodiscard]] domain::StopInfo GetStopInfo(const std::string_view bus_name) const;

    private:    //methods
        [[nodiscard]] size_t GetUniqStopsCount(const std::vector<const domain::Stop*> bus_stops) const;
        [[nodiscard]] int    GetRouteLength(const std::vector<const domain::Stop*> bus_stops) const;
        [[nodiscard]] double GetRouteDistance(const std::vector<const domain::Stop*> bus_stops) const;
    };
}    // namespace transport_catalogue