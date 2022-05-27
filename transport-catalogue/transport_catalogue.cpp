#include "transport_catalogue.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace transport_catalogue
{
    void TransportCatalogue::AddStop(const domain::Stop& stop_data)
    {
        auto* stop                     = &stops_.emplace_back(stop_data);
        stopname_to_stop_[stop->name_] = stop;
    }

    const TransportCatalogue::StopnameToStop* TransportCatalogue::GetStops() const
    {
        return &stopname_to_stop_;
    }

    const TransportCatalogue::StopNameToBus* TransportCatalogue::GetStopNameToBus() const
    {
        return &stopname_to_bus_;
    }

    domain::Stop* TransportCatalogue::FindStop(const std::string_view name) const
    {
        return stopname_to_stop_.count(name) ? stopname_to_stop_.at(name) : nullptr;
    }

    void TransportCatalogue::AddBus(const domain::Bus& bus_data)
    {
        if(bus_data.route_type_ == domain::RouteType::CIRCLE)
        {
            if(bus_data.stops_.front() != bus_data.stops_.back())
            {
                throw std::invalid_argument("In circle route first and last stops must be equal!");
            }
        }

        auto* bus                   = &buses_.emplace_back(bus_data);
        busname_to_bus_[bus->name_] = bus;

        for(size_t i = 0; i < bus->stops_.size(); ++i)
        {
            stopname_to_bus_[bus->stops_[i]].insert(bus);
        }
    }

    const TransportCatalogue::BusnameToBus* TransportCatalogue::GetBuses() const
    {
        return &busname_to_bus_;
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
        domain::BusInfo bus_info;
        const auto*     bus = FindBus(bus_name);

        if(bus != nullptr)
        {
            bus_info.name_             = bus->name_;
            bus_info.stops_count_      = GetStopCount(bus);
            bus_info.uniq_stops_count_ = GetUniqStopsCount(bus);
            bus_info.bus_length_       = GetRouteLength(bus);
            bus_info.curvature_        = bus_info.bus_length_ / GetRouteDistance(bus);
            return bus_info;
        }
        bus_info.found_ = false;
        return bus_info;
    }

    domain::StopInfo TransportCatalogue::GetStopInfo(const std::string_view stop_name) const
    {
        domain::StopInfo stop_info;
        auto*            Stop = FindStop(stop_name);

        stop_info.name_ = stop_name;
        if(Stop != nullptr)
        {
            if(stopname_to_bus_.count(Stop))
            {
                auto buses = stopname_to_bus_.at(Stop);
                stop_info.buses_name_.reserve(buses.size());

                for(const auto* Bus : buses)
                {
                    stop_info.buses_name_.push_back(Bus->name_);
                }
                std::sort(stop_info.buses_name_.begin(), stop_info.buses_name_.end());
                return stop_info;
            }
            stop_info.found_ = true;
            return stop_info;
        }
        stop_info.found_ = false;
        return stop_info;
    }

    size_t TransportCatalogue::GetStopCount(const domain::Bus* bus_data) const
    {
        int result = static_cast<int>(bus_data->stops_.size());
        return bus_data->route_type_ == domain::RouteType::LINEAR ? result = result * 2 - 1 : result;
    }

    size_t TransportCatalogue::GetUniqStopsCount(const domain::Bus* bus_data) const
    {
        std::unordered_set<const domain::Stop*> unique_stops{bus_data->stops_.begin(), bus_data->stops_.end()};
        return unique_stops.size();
    }

    int TransportCatalogue::GetRouteLength(const domain::Bus* bus_data) const
    {
        int result = 0;
        for(auto iter1 = bus_data->stops_.begin(), iter2 = iter1 + 1; iter2 < bus_data->stops_.end(); ++iter1, ++iter2)
        {
            result += GetDistanceBetweenStops((*iter1)->name_, (*iter2)->name_);
        }

        if(bus_data->route_type_ == domain::RouteType::LINEAR)
        {
            for(auto iter1 = bus_data->stops_.rbegin(), iter2 = iter1 + 1; iter2 < bus_data->stops_.rend(); ++iter1, ++iter2)
            {
                result += GetDistanceBetweenStops((*iter1)->name_, (*iter2)->name_);
            }
        }
        return result;
    }

    double TransportCatalogue::GetRouteDistance(const domain::Bus* bus_data) const
    {
        double result = 0.0;
        for(auto iter1 = bus_data->stops_.begin(), iter2 = iter1 + 1; iter2 < bus_data->stops_.end(); ++iter1, ++iter2)
        {
            result += ComputeDistance((*iter1)->coordinates_, (*iter2)->coordinates_);
        }
        return bus_data->route_type_ == domain::RouteType::LINEAR ? result *= 2 : result;
    }
}    // namespace transport_catalogue