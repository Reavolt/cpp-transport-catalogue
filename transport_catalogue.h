#pragma once

#include "geo.h"
#include "domain.h"
#include <deque>
#include <string>
#include <string_view>
#include <ostream>         
#include <sstream>
#include <iostream>
#include <iomanip>         
#include <unordered_set>
#include <unordered_map>
#include <algorithm>       
#include <utility>         
#include <cctype>          
#include <functional>      

namespace transport_db
{
    class PointersHasher
    {
    public:
        std::size_t operator()(const std::pair<const domain::Stop*, const domain::Stop*> pair_of_pointers) const noexcept
        {
            return hasher_(static_cast<const void*>(pair_of_pointers.first)) * 41 + hasher_(static_cast<const void*>(pair_of_pointers.second));
        }

        std::size_t operator()(const domain::Stop* stop) const noexcept
        {
            return hasher_(static_cast<const void*>(stop)) * 41;
        }
    private:
        std::hash<const void*> hasher_;
    };

    struct StopInfo
    {
        std::string_view name_{};
        std::vector<std::string> bus_number_{};
        bool absent_ = false;
    };

    struct BusInfo
    {
        std::string bus_number_ = {};
        int stops_count_ = 0;
        int unique_stops_ = 0;
        double geo_route_length_ = 0.0;
        int meters_route_length_ = 0;
        double curvature_ = 0.0;
    };

    class TransportCatalogue
    {
    public:
        TransportCatalogue();
        virtual ~TransportCatalogue() = default;

        void AddStop(geo::Coordinates coordinates, const std::string& name);
        void AddRoute(const domain::Bus& route);
        void AddStopToBusMap(const std::string_view route);

        std::string_view GetStopName(const domain::Stop* stop_ptr);
        std::string_view GetStopName(const domain::Stop stop);
        std::deque<domain::Stop> GetAllStops();
        StopInfo GetStopInfo(std::string_view stop_name);
       
        std::string_view GetBusName(const domain::Bus* route_ptr);
        std::string_view GetBusName(const domain::Bus route);
        BusInfo GetBusInfo(const std::string_view route);
        std::deque<domain::Bus> GetAllBuses();

        void SetDistance(domain::Stop* stop_from, domain::Stop* stop_to, size_t dist);
        size_t GetDistance(domain::Stop* stop_from, domain::Stop* stop_to);
        size_t GetDistanceDirectly(domain::Stop* stop_from, domain::Stop* stop_to);

        domain::Stop* GetStopByName(std::string_view stop_name);
        domain::Bus* GetRouteByName(std::string_view bus_name);
        std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, PointersHasher> GetStopsFromTo();


    private:
        std::deque<domain::Stop> all_stops_;
        std::unordered_map<std::string_view, domain::Stop*> all_stops_map_;
        std::deque<domain::Bus> all_buses_;
        std::unordered_map<std::string_view, domain::Bus*> all_buses_map_;
        std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, PointersHasher> distances_;
        std::unordered_map<domain::Stop*, std::unordered_set<domain::Bus*>, PointersHasher> stop_to_bus_map_;
    };
}
