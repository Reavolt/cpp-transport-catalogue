#include "input_reader.h"

#include "detail.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

namespace input
{
    using namespace transport_catalogue;
    using namespace detail;

    input_reader::input_reader(TransportCatalogue* catalogue) : stop_queries_(), bus_queries_(), catalogue_(catalogue) {}

    void input_reader::fill_transport_catalogue()
    {
        std::vector<stop> stops_data;
        stops_data.reserve(stop_queries_.size());

        for(auto& stop_data : stop_queries_)
        {
            auto stop = parse_stop(stop_data.request_);
            catalogue_->add_stop(stop);
            stops_data.push_back(stop);
        }

        for(auto& stop_from : stops_data)
        {
            std::vector<std::pair<stop*, int>> distance_to_stop_ = parse_stops_distance(stop_queries_.front().request_);

            for(auto [stop_to, distance] : distance_to_stop_)
            {
                catalogue_->set_distance_between_stops(stop_from.name_, stop_to->name_, distance);
            }
            stop_queries_.pop_front();
        }

        for(auto& bus_data : bus_queries_)
        {
            bus data = parse_bus(bus_data.request_);
            catalogue_->add_bus(data);
        }
        bus_queries_.clear();
    }

    std::istream& operator>>(std::istream& stream, input_reader& i_reader)
    {
        size_t request_count = 0;
        stream >> request_count;
        stream.ignore();

        for(size_t i = 0; i < request_count; ++i)
        {
            std::string request;
            std::getline(stream, request);

            std::string request_type(i_reader.parse_request_type(request));

            if(request_type == "Stop"s)
            {
                i_reader.stop_queries_.emplace_back(input_reader::query_type::STOP, request);
            }

            if(request_type == "Bus"s)
            {
                i_reader.bus_queries_.emplace_back(input_reader::query_type::BUS, request);
            }
        }
        return stream;
    }

    std::string input_reader::parse_request_type(std::string& request)
    {
        std::string request_type = request.substr(0, request.find_first_of(' '));
        trim(request.erase(0, request.find_first_of(' ')));
        return request_type;
    }

    std::string input_reader::parse_name(std::string& request)
    {
        size_t      name_delimeter = request.find_first_of(':');
        std::string name           = trim_copy(request.substr(0, name_delimeter));
        trim(request.erase(0, name_delimeter + 1));
        return name;
    }

    Coordinates input_reader::parse_coords(std::string& request)
    {
        size_t coord_delimeter = request.find_first_of(',');
        double latitude        = std::stod(request.substr(0, coord_delimeter));
        double longitude       = 0.0;
        request                = trim_copy(request.erase(0, coord_delimeter + 1));

        if(request.find_first_of(',') != std::string::npos)
        {
            coord_delimeter = request.find_first_of(',');
            longitude       = std::stod(request.substr(0, coord_delimeter));
        }
        else
        {
            longitude = std::stod(request.substr(0, std::string::npos));
        }
        request = trim_copy(request.erase(0, coord_delimeter + 1));
        return {latitude, longitude};
    }

    std::vector<std::pair<stop*, int>> input_reader::parse_stops_distance(std::string& request)
    {
        std::stringstream                  str_stream(request);
        std::vector<std::pair<stop*, int>> stops_distances;

        stops_distances.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == ','; }) + 1);

        std::string stop_distances;
        while(std::getline(str_stream, stop_distances, ','))
        {
            int distance = std::stoi(stop_distances.substr(0, stop_distances.find_first_of('m')));
            stop_distances.erase(0, stop_distances.find_first_of("to"));
            std::string name = trim_copy(stop_distances.substr(stop_distances.find_first_not_of("to"), std::string::npos));

            stops_distances.emplace_back(catalogue_->find_stop(name), distance);
        }
        return stops_distances;
    }

    std::vector<const stop*> input_reader::parse_standart_route(std::string& request)
    {
        std::stringstream        str_stream(request);
        std::vector<const stop*> standart_route;

        standart_route.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == '>'; }) + 1);

        std::string stop;
        while(std::getline(str_stream, stop, '>'))
        {
            standart_route.push_back(catalogue_->find_stop(trim(stop)));
        }
        return standart_route;
    }

    std::vector<const stop*> input_reader::parse_ring_route(std::string& request)
    {
        std::stringstream        str_stream(request);
        std::vector<const stop*> ring_route;

        ring_route.reserve(std::count_if(request.begin(), request.end(), [&](char c) { return c == '-'; }) * 2);

        std::string stop;
        while(std::getline(str_stream, stop, '-'))
        {
            ring_route.push_back(catalogue_->find_stop(trim(stop)));
        }
        std::copy(ring_route.rbegin() + 1, ring_route.rend(), std::back_inserter(ring_route));
        return ring_route;
    }

    stop input_reader::parse_stop(std::string& request)
    {
        return {parse_name(request), parse_coords(request)};
    }

    bus input_reader::parse_bus(std::string& request)
    {
        std::string name(parse_name(request));

        std::vector<const stop*> route;
        if(request.find('>') != std::string::npos)
        {
            route = parse_standart_route(request);
        }
        else
        {
            route = parse_ring_route(request);
        }
        return {name, route};
    }
}    // namespace input