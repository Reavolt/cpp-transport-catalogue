#include "transport_catalogue.h"

#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace transport_catalogue
{
    void TransportCatalogue::add_stop(const stop& stop_data)
    {
        auto* stop                     = &stops_.emplace_back(stop_data);
        stopname_to_stop_[stop->name_] = stop;
    }

    stop* TransportCatalogue::find_stop(const std::string_view name) const
    {
        return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
    }

    void TransportCatalogue::add_bus(const bus& bus_data)
    {
        auto* bus                   = &buses_.emplace_back(bus_data);
        busname_to_bus_[bus->name_] = bus;

        for(size_t i = 0; i < bus->stops_.size(); ++i)
        {
            stopname_to_bus_[bus->stops_[i]].insert(bus);
        }
    }

    bus* TransportCatalogue::find_bus(const std::string_view name) const
    {
        return busname_to_bus_.count(name) ? busname_to_bus_.at(name) : nullptr;
    }

    void TransportCatalogue::set_distance_between_stops(const std::string_view from_stop,
                                                        const std::string_view to_stop,
                                                        const int              distance)
    {
        stop* from = find_stop(from_stop);
        stop* to   = find_stop(to_stop);

        stops_to_distance_[{from, to}] = distance;

        if(stops_to_distance_.count({to, from}))
        {
            stops_to_distance_.at({to, from});
        }
    }

    int TransportCatalogue::get_distance_between_stops(const std::string_view from_stop, const std::string_view to_stop) const
    {
        stop* from = find_stop(from_stop);
        stop* to   = find_stop(to_stop);

        if(stops_to_distance_.count({from, to}))
        {
            return stops_to_distance_.at({from, to});
        }
        return stops_to_distance_.at({to, from});
    }

    bus_info TransportCatalogue::get_bus_info(const std::string_view bus_name) const
    {
        if(busname_to_bus_.count(bus_name))
        {
            auto&  bus            = busname_to_bus_.at(bus_name);
            int    route_length   = get_route_length(bus->stops_);
            double route_distance = get_route_distance(bus->stops_);
            return {
              bus->name_, bus->stops_.size(), get_uniq_stops_count(bus->stops_), route_length, route_length / route_distance};
        }
        return {std::string(bus_name), 0, 0, 0, 0.0};
    }

    stop_info TransportCatalogue::get_stop_info(const std::string_view stop_name) const
    {
        auto* stop = find_stop(stop_name);

        stop_info stop_info;
        stop_info.name_ = stop_name;

        if(stop != nullptr)
        {
            if(stopname_to_bus_.count(stop))
            {
                auto buses = stopname_to_bus_.at(stop);
                stop_info.buses_name_.reserve(buses.size());

                for(const auto* bus : buses)
                {
                    stop_info.name_ = stop_name;
                    stop_info.buses_name_.push_back(bus->name_);
                }
                std::sort(stop_info.buses_name_.begin(), stop_info.buses_name_.end());
                return stop_info;
            }
            return stop_info;
        }
        stop_info.not_found_ = true;
        return stop_info;
    }

    size_t TransportCatalogue::get_uniq_stops_count(const std::vector<const stop*> bus_stops) const
    {
        std::unordered_set<const stop*> unique_stops{bus_stops.begin(), bus_stops.end()};
        return unique_stops.size();
    }

    int TransportCatalogue::get_route_length(const std::vector<const stop*> bus_stops) const
    {
        return std::transform_reduce(std::next(bus_stops.begin()),
                                     bus_stops.end(),
                                     bus_stops.begin(),    // входной диапазон
                                     0,                    // начальное значение
                                     std::plus<>{},    // reduce-операция (группирующая функция)
                                     [this](const stop* lhs, const stop* rhs)
                                     { return get_distance_between_stops(rhs->name_, lhs->name_); }    // map-операция
        );
    }

    double TransportCatalogue::get_route_distance(const std::vector<const stop*> bus_stops) const
    {
        return std::transform_reduce(std::next(bus_stops.begin()),
                                     bus_stops.end(),
                                     bus_stops.begin(),    // входной диапазон
                                     0.0,                  // начальное значение
                                     std::plus<>{},    // reduce-операция (группирующая функция)
                                     [](const stop* lhs, const stop* rhs)
                                     {
                                         return ComputeDistance({lhs->coordinates_.lat, lhs->coordinates_.lng},
                                                                {rhs->coordinates_.lat, rhs->coordinates_.lng});
                                     }    // map-операция
        );
    }
}    // namespace transport_catalogue