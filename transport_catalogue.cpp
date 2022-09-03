#include "transport_catalogue.h"

namespace transport_db
{
    TransportCatalogue::TransportCatalogue()
    {}

    void TransportCatalogue::AddStop(geo::Coordinates coordinates, const std::string& name) {
        all_stops_.push_back(std::move(domain::Stop(all_stops_.size(), coordinates, name)));
        domain::Stop* stop = &(all_stops_.back());
        std::string_view stop_view{ (*stop).name };
        all_stops_map_[stop_view] = stop;
    }

    void TransportCatalogue::AddRoute(const domain::Bus& route)
    {
        if (all_buses_map_.count(route.bus_number) == 0)
        {
            auto& ref = all_buses_.emplace_back(std::move(route));
            all_buses_map_.insert({ std::string_view(ref.bus_number), &ref });
            for (size_t i = 0; i < route.stops.size(); ++i) {
                stop_to_bus_map_[route.stops[i]].insert(&ref);
            }
        }
    }

    void TransportCatalogue::AddStopToBusMap(const std::string_view route) {
        auto bus = GetRouteByName(route);
        for (size_t i = 0; i < bus->stops.size(); ++i) {
            stop_to_bus_map_[bus->stops[i]].insert(bus);
        }
    }

    BusInfo TransportCatalogue::GetBusInfo(const std::string_view route)
    {
        BusInfo bus_info;
        if (all_buses_map_.count(route))
        {
            bus_info.bus_number_ = route;
            auto& Bus = all_buses_map_[route];
            std::vector<domain::Stop*> tmp = Bus->stops;
            bus_info.stops_count_ = static_cast<int>(Bus->stops.size());
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            bus_info.unique_stops_ = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());
            if (bus_info.stops_count_ > 1)
            {
                for (int i = 0; i < bus_info.stops_count_ - 1; ++i)
                {
                    bus_info.geo_route_length_ += ComputeDistance(Bus->stops[i]->coords, Bus->stops[i + 1]->coords);
                    bus_info.meters_route_length_ += GetDistance(Bus->stops[i], Bus->stops[i + 1]);
                }
                bus_info.curvature_ = bus_info.meters_route_length_ / bus_info.geo_route_length_;
            }
            return bus_info;
        }
        else {
            return bus_info;
        }
    }

    void TransportCatalogue::SetDistance(domain::Stop* stop_from, domain::Stop* stop_to, size_t dist)
    {
        if (stop_from != nullptr && stop_to != nullptr)
        {
            distances_.insert({ { stop_from, stop_to }, dist });
        }
    }

    size_t TransportCatalogue::GetDistance(domain::Stop* stop_from, domain::Stop* stop_to)
    {
        size_t result = GetDistanceDirectly(stop_from, stop_to);
        return (result > 0 ? result : GetDistanceDirectly(stop_to, stop_from));
    }

    size_t TransportCatalogue::GetDistanceDirectly(domain::Stop* stop_from, domain::Stop* stop_to)
    {
        if (distances_.count({ stop_from, stop_to }) > 0)
        {
            return distances_.at({ stop_from, stop_to });
        }
        else
        {
            return 0U;
        }
    }

    std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, PointersHasher> TransportCatalogue::GetStopsFromTo() 
    {
        return distances_;
    }

    std::string_view TransportCatalogue::GetStopName(const domain::Stop* stop_ptr)
    {
        return std::string_view(stop_ptr->name);
    }

    std::string_view TransportCatalogue::GetStopName(const domain::Stop stop)
    {
        return std::string_view(stop.name);
    }

    std::string_view TransportCatalogue::GetBusName(const domain::Bus* route_ptr)
    {
        return std::string_view(route_ptr->bus_number);
    }

    std::string_view TransportCatalogue::GetBusName(const domain::Bus route)
    {
        return std::string_view(route.bus_number);
    }

    domain::Stop* TransportCatalogue::GetStopByName(std::string_view stop_name)
    {
        if (all_stops_map_.count(stop_name) == 0)
        {
            return nullptr;
        }
        else
        {
            return all_stops_map_.at(stop_name);
        }
    }

    std::deque<domain::Bus> TransportCatalogue::GetAllBuses() {
        return all_buses_;
    }

    std::deque<domain::Stop> TransportCatalogue::GetAllStops() {
        return all_stops_;
    }

    domain::Bus* TransportCatalogue::GetRouteByName(std::string_view bus_name)
    {
        if (all_buses_map_.count(bus_name) == 0)
        {
            return nullptr;
        }
        else
        {
            return all_buses_map_.at(bus_name);
        }
    }

    StopInfo TransportCatalogue::GetStopInfo(std::string_view stop_name)
    {
        auto* result = GetStopByName(stop_name);
        StopInfo stop_info;
        stop_info.name_ = stop_name;
        if (result != nullptr)
        {
            if (stop_to_bus_map_.count(result)) {
                auto set_buses = stop_to_bus_map_.at(result);
                for (const auto& bus : set_buses)
                {
                    stop_info.name_ = stop_name;
                    stop_info.bus_number_.push_back(bus->bus_number);
                }
                std::sort(stop_info.bus_number_.begin(), stop_info.bus_number_.end());
                return stop_info;
            }
            return stop_info;
        }
        stop_info.absent_ = true;
        return stop_info;
    }
}
