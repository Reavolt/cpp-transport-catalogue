#include "input_reader.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

namespace input
{
    Reader::Reader(transport_catalogue::TransportCatalogue* catalogue) :
        stop_queries_(),
        bus_queries_(),
        catalogue_(catalogue)
    {
    }

    void Reader::FillCatalogue()
    {
        std::vector<domain::Stop> stops_data;
        stops_data.reserve(stop_queries_.size());

        for(auto& stop_data : stop_queries_)
        {
            auto Stop = ParseStop(stop_data.request_);
            catalogue_->AddStop(Stop);
            stops_data.push_back(Stop);
        }

        for(auto& stop_from : stops_data)
        {
            std::vector<std::pair<domain::Stop*, int>> distance_to_stop_ = ParseStopsDistance(stop_queries_.front().request_);

            for(auto [stop_to, distance] : distance_to_stop_)
            {
                catalogue_->SetDistanceBetweenStops(stop_from.name_, stop_to->name_, distance);
            }
            stop_queries_.pop_front();
        }

        for(auto& bus_data : bus_queries_)
        {
            domain::Bus data = ParseBus(bus_data.request_);
            catalogue_->AddBus(data);
        }
        bus_queries_.clear();
    }

    std::string Reader::ParseRequestType(std::string& request)
    {
        std::string request_type = request.substr(0, request.find_first_of(' '));
        detail::trim(request.erase(0, request.find_first_of(' ')));
        return request_type;
    }

    std::string Reader::ParseName(std::string& request)
    {
        size_t      name_delimeter = request.find_first_of(':');
        std::string name           = detail::trim_copy(request.substr(0, name_delimeter));
        detail::trim(request.erase(0, name_delimeter + 1));
        return name;
    }

    geo::Coordinates Reader::ParseCoords(std::string& request)
    {
        size_t coord_delimeter = request.find_first_of(',');
        double latitude        = std::stod(request.substr(0, coord_delimeter));
        double longitude       = 0.0;
        request                = detail::trim_copy(request.erase(0, coord_delimeter + 1));

        if(request.find_first_of(',') != std::string::npos)
        {
            coord_delimeter = request.find_first_of(',');
            longitude       = std::stod(request.substr(0, coord_delimeter));
        }
        else
        {
            longitude = std::stod(request.substr(0, std::string::npos));
        }
        request = detail::trim_copy(request.erase(0, coord_delimeter + 1));
        return {latitude, longitude};
    }

    std::vector<std::pair<domain::Stop*, int>> Reader::ParseStopsDistance(std::string& request)
    {
        std::stringstream                          str_stream(request);
        std::vector<std::pair<domain::Stop*, int>> stops_distances;

        stops_distances.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == ','; }) + 1);

        std::string stop_distances;
        while(std::getline(str_stream, stop_distances, ','))
        {
            int distance = std::stoi(stop_distances.substr(0, stop_distances.find_first_of('m')));
            stop_distances.erase(0, stop_distances.find_first_of("to"));
            std::string name =
              detail::trim_copy(stop_distances.substr(stop_distances.find_first_not_of("to"), std::string::npos));

            stops_distances.emplace_back(catalogue_->FindStop(name), distance);
        }
        return stops_distances;
    }

    std::vector<const domain::Stop*> Reader::ParseStandartRoute(std::string& request)
    {
        std::stringstream                str_stream(request);
        std::vector<const domain::Stop*> standart_route;

        standart_route.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == '>'; }) + 1);

        std::string Stop;
        while(std::getline(str_stream, Stop, '>'))
        {
            standart_route.push_back(catalogue_->FindStop(detail::trim(Stop)));
        }
        return standart_route;
    }

    std::vector<const domain::Stop*> Reader::ParseRingRoute(std::string& request)
    {
        std::stringstream                str_stream(request);
        std::vector<const domain::Stop*> ring_route;

        ring_route.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == '-'; }) + 1);

        std::string Stop;
        while(std::getline(str_stream, Stop, '-'))
        {
            ring_route.push_back(catalogue_->FindStop(detail::trim(Stop)));
        }
        return ring_route;
    }

    domain::Stop Reader::ParseStop(std::string& request)
    {
        return {ParseName(request), ParseCoords(request)};
    }

    domain::Bus Reader::ParseBus(std::string& request)
    {
        std::string name(ParseName(request));

        std::vector<const domain::Stop*> route;
        if(request.find('>') != std::string::npos)
        {
            route = ParseStandartRoute(request);
            return {name, route, domain::RouteType::LINEAR};
        }
        route = ParseRingRoute(request);
        return {name, route, domain::RouteType::CIRCLE};
    }
}    // namespace input