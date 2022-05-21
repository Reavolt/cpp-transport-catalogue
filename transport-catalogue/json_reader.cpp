#include "json_reader.h"

#include "domain.h"

namespace json
{
    using namespace std::string_literals;

    Reader::Reader(TransportCatalogue catalogue) : json_data_({}), catalogue_(catalogue) {}

    void Reader::FillCatalogue()
    {
        if(json_data_.GetRoot().IsMap() && json_data_.GetRoot().AsMap().count("base_requests"s) > 0)
        {
            const auto& base_requests = json_data_.GetRoot().AsMap().at("base_requests"s);
            if(base_requests.IsArray())
            {
                ParseStops(base_requests.AsArray());
                ParseRoutes(base_requests.AsArray());
                ParseDistances(base_requests.AsArray());
            }
        }
    }

    bool Reader::IsStop(const json::Node& node)
    {
        if(!node.IsMap())
        {
            return false;
        }

        const auto& stop = node.AsMap();
        if(stop.count("type"s) == 0 || stop.at("type"s) != "Stop"s)
        {
            return false;
        }
        if(stop.count("name"s) == 0 || !(stop.at("name"s).IsString()))
        {
            return false;
        }
        if(stop.count("latitude"s) == 0 || !(stop.at("latitude"s).IsDouble()))
        {
            return false;
        }
        if(stop.count("longitude"s) == 0 || !(stop.at("longitude"s).IsDouble()))
        {
            return false;
        }
        if(stop.count("road_distances"s) == 0 || (stop.at("longitude"s).IsMap()))
        {
            return false;
        }
        return true;
    }

    void Reader::ParseStops(const json::Array& data)
    {
        for(const auto& value : data)
        {
            if(IsStop(value))
            {
                const auto& name = value.AsMap().at("name"s).AsString();
                const auto  lat  = value.AsMap().at("latitude"s).AsDouble();
                const auto  lng  = value.AsMap().at("longitude"s).AsDouble();
                catalogue_->AddStop({name, {lat, lng}});
            }
        }
    }

    bool Reader::IsRoute(const json::Node& node)
    {
        if(!node.IsMap())
        {
            return false;
        }
        const auto& bus = node.AsMap();
        if(bus.count("type"s) == 0 || bus.at("type"s) != "Bus"s)
        {
            return false;
        }
        if(bus.count("name"s) == 0 || !(bus.at("name"s).IsString()))
        {
            return false;
        }
        if(bus.count("is_roundtrip"s) == 0 || !(bus.at("is_roundtrip"s).IsBool()))
        {
            return false;
        }
        if(bus.count("stops"s) == 0 || !(bus.at("stops"s).IsArray()))
        {
            return false;
        }
        return true;
    }

    void Reader::ParseRoutes(const json::Array& data)
    {
        for(const auto& value : data)
        {
            if(IsRoute(value))
            {
                const auto& name = value.AsMap().at("name"s).AsString();
                // const auto  is_roundtrip = value.AsMap().at("is_roundtrip"s).AsBool();
                const auto stops = value.AsMap().at("stops"s).AsArray();

                std::vector<const domain::Stop*> stops_names;
                for(const auto& stop_name : stops)
                {
                    if(stop_name.IsString())
                    {
                        stops_names.push_back(catalogue_->FindStop(stop_name.AsString()));
                    }
                }
                catalogue_->AddBus({name, stops_names});
            }
        }
    }

    void Reader::ParseDistances(const json::Array& data)
    {
        for(const auto& value : data)
        {
            if(IsStop(value))
            {
                const auto& name_from = value.AsMap().at("name"s).AsString();
                const auto  distances = value.AsMap().at("road_distances"s).AsMap();

                for(const auto& [name_to, distance] : distances)
                {
                    if(distance.IsInt())
                    {
                        catalogue_->SetDistanceBetweenStops(name_from, name_to, distance.AsInt());
                    }
                }
            }
        }
    }

    json::Array Reader::ParseAnswer(const json::Array& requests) const
    {
        json::Array result;
        for(const auto& request : requests)
        {
            if(IsRouteRequest(request))
            {
                result.push_back(LoadRouteAnswer(request.AsMap()));
            }
            else if(IsStopRequest(request))
            {
                result.push_back(LoadStopAnswer(request.AsMap()));
            }
            else if(IsMapRequest(request))
            {
                result.push_back(LoadMapAnswer(request.AsMap()));
            }
            else if(IsRouteBuildRequest(request))
            {
                result.push_back(LoadRouteBuildAnswer(request.AsMap(), catalogue, router));
            }
        }
        return result;
    }

    json::Array Reader::ReadCatalogue()
    {
        if(json_data_.GetRoot().IsMap() && json_data_.GetRoot().AsMap().count("stat_requests"s) > 0)
        {
            const auto& requests = json_data_.GetRoot().AsMap().at("stat_requests"s);

            if(requests.IsArray())
            {
                return ParseAnswer(requests.AsArray());
            }
        }
    }
}    // namespace json