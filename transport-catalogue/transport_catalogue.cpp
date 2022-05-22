#include "transport_catalogue.h"

#include <algorithm>
#include <numeric>

namespace transport_catalogue
{
    void TransportCatalogue::AddStop(const domain::Stop& stop_data)
    {
        auto* stop                     = &stops_.emplace_back(stop_data);
        stopname_to_stop_[stop->name_] = stop;
    }

    domain::Stop* TransportCatalogue::FindStop(const std::string_view name) const
    {
        return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
    }

    void TransportCatalogue::AddBus(const domain::Bus& bus_data)
    {
        auto* bus = &buses_.emplace_back(bus_data);

        if(bus->route_type_ == domain::RouteType::CIRCLE)
        {
            bus->stops_.reserve(bus->stops_.size() * 2);
            std::copy(bus->stops_.rbegin() + 1, bus->stops_.rend(), std::back_inserter(bus->stops_));
        }
        busname_to_bus_[bus->name_] = bus;

        for(size_t i = 0; i < bus->stops_.size(); ++i)
        {
            stopname_to_bus_[bus->stops_[i]].insert(bus);
        }
    }

    domain::Bus* TransportCatalogue::FindBus(const std::string_view name) const
    {
        return busname_to_bus_.count(name) ? busname_to_bus_.at(name) : nullptr;
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::string_view from_stop,
                                                     const std::string_view to_stop,
                                                     const int              distance)
    {
        domain::Stop* from = FindStop(from_stop);
        domain::Stop* to   = FindStop(to_stop);

        stops_to_distance_[{from, to}] = distance;

        if(stops_to_distance_.count({to, from}))
        {
            stops_to_distance_.at({to, from});
        }
    }

    int TransportCatalogue::GetDistanceBetweenStops(const std::string_view from_stop, const std::string_view to_stop) const
    {
        domain::Stop* from = FindStop(from_stop);
        domain::Stop* to   = FindStop(to_stop);

        if(stops_to_distance_.count({from, to}))
        {
            return stops_to_distance_.at({from, to});
        }
        return stops_to_distance_.at({to, from});
    }

    domain::BusInfo TransportCatalogue::GetBusInfo(const std::string_view bus_name) const
    {
        if(busname_to_bus_.count(bus_name))
        {
            auto&  Bus            = busname_to_bus_.at(bus_name);
            int    route_length   = GetRouteLength(Bus->stops_);
            double route_distance = GetRouteDistance(Bus->stops_);
            return {Bus->name_, Bus->stops_.size(), GetUniqStopsCount(Bus->stops_), route_length, route_length / route_distance};
        }
        return {std::string(bus_name), 0, 0, 0, 0.0, false};
    }

    domain::StopInfo TransportCatalogue::GetStopInfo(const std::string_view stop_name) const
    {
        auto* Stop = FindStop(stop_name);

        domain::StopInfo stop_info;
        stop_info.name_ = stop_name;

        if(Stop != nullptr)
        {
            if(stopname_to_bus_.count(Stop))
            {
                auto buses = stopname_to_bus_.at(Stop);
                stop_info.buses_name_.reserve(buses.size());

                for(const auto* Bus : buses)
                {
                    stop_info.name_ = stop_name;
                    stop_info.buses_name_.push_back(Bus->name_);
                }
                std::sort(stop_info.buses_name_.begin(), stop_info.buses_name_.end());
                return stop_info;
            }
            return stop_info;
        }
        stop_info.found_ = false;
        return stop_info;
    }

    size_t TransportCatalogue::GetUniqStopsCount(const std::vector<const domain::Stop*> bus_stops) const
    {
        std::unordered_set<const domain::Stop*> unique_stops{bus_stops.begin(), bus_stops.end()};
        return unique_stops.size();
    }

    int TransportCatalogue::GetRouteLength(const std::vector<const domain::Stop*> bus_stops) const
    {
        return std::transform_reduce(std::next(bus_stops.begin()),
                                     bus_stops.end(),
                                     bus_stops.begin(),    // входной диапазон
                                     0,                    // начальное значение
                                     std::plus<>{},    // reduce-операция (группирующая функция)
                                     [this](const domain::Stop* lhs, const domain::Stop* rhs)
                                     { return GetDistanceBetweenStops(rhs->name_, lhs->name_); }    // map-операция
        );
    }

    double TransportCatalogue::GetRouteDistance(const std::vector<const domain::Stop*> bus_stops) const
    {
        return std::transform_reduce(std::next(bus_stops.begin()),
                                     bus_stops.end(),
                                     bus_stops.begin(),    // входной диапазон
                                     0.0,                  // начальное значение
                                     std::plus<>{},    // reduce-операция (группирующая функция)
                                     [](const domain::Stop* lhs, const domain::Stop* rhs)
                                     {
                                         return geo::ComputeDistance({lhs->coordinates_.lat, lhs->coordinates_.lng},
                                                                     {rhs->coordinates_.lat, rhs->coordinates_.lng});
                                     }    // map-операция
        );
    }
}    // namespace transport_catalogue