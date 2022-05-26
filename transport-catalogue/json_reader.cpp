#include "json_reader.h"

#include "domain.h"
#include "json.h"

#include <sstream>

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

    bool Reader::IsStop(const json::Node& node) const
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

    bool Reader::IsRoute(const json::Node& node) const
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
                const auto& name         = value.AsMap().at("name"s).AsString();
                const auto  is_roundtrip = value.AsMap().at("is_roundtrip"s).AsBool();
                const auto  stops        = value.AsMap().at("stops"s).AsArray();

                domain::RouteType route_type = domain::RouteType::NONE;
                if(is_roundtrip)
                {
                    route_type = domain::RouteType::CIRCLE;
                }
                else
                {
                    route_type = domain::RouteType::LINEAR;
                }

                std::vector<const domain::Stop*> stops_names;
                for(const auto& stop_name : stops)
                {
                    if(stop_name.IsString())
                    {
                        stops_names.push_back(catalogue_->FindStop(stop_name.AsString()));
                    }
                }
                catalogue_->AddBus({name, stops_names, route_type});
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

    bool Reader::IsRouteRequest(const json::Node& node) const
    {
        if(!node.IsMap())
        {
            return false;
        }
        const auto& request = node.AsMap();
        if(request.count("type"s) == 0 || request.at("type"s) != "Bus"s)
        {
            return false;
        }
        if(request.count("id"s) == 0 || !(request.at("id"s).IsInt()))
        {
            return false;
        }
        if(request.count("name"s) == 0 || !(request.at("name"s).IsString()))
        {
            return false;
        }
        return true;
    }

    json::Dict Reader::ParseRouteAnswer(const json::Dict& request) const
    {
        int         id   = request.at("id"s).AsInt();
        const auto& name = request.at("name"s).AsString();

        auto answer = catalogue_->GetBusInfo(name);
        if(answer.found_)
        {
            auto map                 = json::Dict();
            map["request_id"]        = id;
            map["curvature"]         = answer.curvature_;
            map["route_length"]      = answer.bus_length_;
            map["stop_count"]        = static_cast<int>(answer.stops_count_);
            map["unique_stop_count"] = static_cast<int>(answer.uniq_stops_count_);
            return map;
        }
        return ErrorMessage(id);
    }

    bool Reader::IsStopRequest(const json::Node& node) const
    {
        if(!node.IsMap())
        {
            return false;
        }
        const auto& request = node.AsMap();
        if(request.count("type"s) == 0 || request.at("type"s) != "Stop"s)
        {
            return false;
        }
        if(request.count("id"s) == 0 || !(request.at("id"s).IsInt()))
        {
            return false;
        }
        if(request.count("name"s) == 0 || !(request.at("name"s).IsString()))
        {
            return false;
        }
        return true;
    }

    json::Dict Reader::ParseStopAnswer(const json::Dict& request) const
    {
        int         id   = request.at("id"s).AsInt();
        const auto& name = request.at("name"s).AsString();

        auto answer = catalogue_->GetStopInfo(name);

        json::Array buses;
        if(answer.found_)
        {
            for(const auto& bus_name : answer.buses_name_)
            {
                buses.push_back(bus_name);
            }
            auto map          = json::Dict();
            map["request_id"] = id;
            map["buses"]      = buses;
            return map;
        }
        return ErrorMessage(id);
    }

    svg::Color Reader::ReadColor(const json::Node& color) const
    {
        if(color.IsString())
        {
            return color.AsString();
        }

        if(color.IsArray() && color.AsArray().size() == 3)
        {
            auto result_color = svg::Rgb(static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
                                         static_cast<uint8_t>(color.AsArray().at(1).AsInt()),
                                         static_cast<uint8_t>(color.AsArray().at(2).AsInt()));
            return result_color;
        }

        if(color.IsArray() && color.AsArray().size() == 4)
        {
            auto result_color = svg::Rgba(static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
                                          static_cast<uint8_t>(color.AsArray().at(1).AsInt()),
                                          static_cast<uint8_t>(color.AsArray().at(2).AsInt()),
                                          color.AsArray().at(3).AsDouble());
            return result_color;
        }
        return svg::NoneColor;
    }

    svg::Point Reader::ReadOffset(const json::Array& offset) const
    {
        svg::Point result;
        if(offset.size() > 1)
        {
            if(offset.at(0).IsDouble())
            {
                result.x = offset.at(0).AsDouble();
            }
            if(offset.at(1).IsDouble())
            {
                result.y = offset.at(1).AsDouble();
            }
        }
        return result;
    }

    renderer::RenderSettings Reader::ParseSettings(const json::Dict& json_data)
    {
        renderer::RenderSettings result;
        if(json_data.count("width"s) != 0 && json_data.at("width"s).IsDouble())
        {
            result.width_ = json_data.at("width"s).AsDouble();
        }
        if(json_data.count("height"s) != 0 && json_data.at("height"s).IsDouble())
        {
            result.height_ = json_data.at("height"s).AsDouble();
        }
        if(json_data.count("padding"s) != 0 && json_data.at("padding"s).IsDouble())
        {
            result.padding_ = json_data.at("padding"s).AsDouble();
        }
        if(json_data.count("line_width"s) != 0 && json_data.at("line_width"s).IsDouble())
        {
            result.line_width_ = json_data.at("line_width"s).AsDouble();
        }
        if(json_data.count("stop_radius"s) != 0 && json_data.at("stop_radius"s).IsDouble())
        {
            result.stop_radius_ = json_data.at("stop_radius"s).AsDouble();
        }
        if(json_data.count("bus_label_font_size"s) != 0 && json_data.at("bus_label_font_size"s).IsInt())
        {
            result.bus_label_font_size_ = json_data.at("bus_label_font_size"s).AsInt();
        }
        if(json_data.count("bus_label_offset"s) != 0 && json_data.at("bus_label_offset"s).IsArray())
        {
            result.bus_label_offset_ = ReadOffset(json_data.at("bus_label_offset"s).AsArray());
        }
        if(json_data.count("stop_label_font_size"s) != 0 && json_data.at("stop_label_font_size"s).IsInt())
        {
            result.stop_label_font_size_ = json_data.at("stop_label_font_size"s).AsInt();
        }
        if(json_data.count("stop_label_offset"s) != 0 && json_data.at("stop_label_offset"s).IsArray())
        {
            result.stop_label_offset_ = ReadOffset(json_data.at("stop_label_offset"s).AsArray());
        }
        if(json_data.count("underlayer_color"s) != 0)
        {
            result.underlayer_color_ = ReadColor(json_data.at("underlayer_color"s));
        }
        if(json_data.count("underlayer_width"s) != 0 && json_data.at("underlayer_width"s).IsDouble())
        {
            result.underlayer_width_ = json_data.at("underlayer_width"s).AsDouble();
        }
        if(json_data.count("color_palette"s) != 0 && json_data.at("color_palette"s).IsArray())
        {
            for(auto& color : json_data.at("color_palette"s).AsArray())
            {
                result.color_palette_.push_back(ReadColor(color));
            }
        }
        return result;
    }

    std::optional<renderer::RenderSettings> Reader::ParseRenderSettings()
    {
        if(json_data_.GetRoot().IsMap() && json_data_.GetRoot().AsMap().count("render_settings"s) > 0)
        {
            auto& render_settings = json_data_.GetRoot().AsMap().at("render_settings"s);
            if(render_settings.IsMap())
            {
                render_settings_ = ParseSettings(render_settings.AsMap());
                return render_settings_;
            }
        }
        return std::nullopt;
    }

    bool Reader::IsMapRequest(const json::Node& node)
    {
        if(!node.IsMap())
        {
            return false;
        }
        const auto& request = node.AsMap();
        if(request.count("type"s) == 0 || request.at("type"s) != "Map"s)
        {
            return false;
        }
        if(request.count("id"s) == 0 || !(request.at("id"s).IsInt()))
        {
            return false;
        }
        return true;
    }

    json::Dict Reader::ParseMapAnswer(const json::Dict& request)
    {
        int                id = request.at("id").AsInt();
        std::ostringstream out;

        renderer::MapRenderer renderer;
        renderer.SetRoutes(catalogue_->GetBuses());
        renderer.SetStops(catalogue_->GetStops());
        renderer.SetStopNameToBus(catalogue_->GetStopNameToBus());
        renderer.SetSettings(ParseRenderSettings().value());
        renderer.RenderMap().Render(out);

        auto map          = json::Dict();
        map["request_id"] = id;
        map["map"]        = out.str();
        return map;
    }

    json::Array Reader::ReadCatalogue()
    {
        json::Array result;

        if(json_data_.GetRoot().IsMap() && json_data_.GetRoot().AsMap().count("stat_requests"s) > 0)
        {
            const auto& requests = json_data_.GetRoot().AsMap().at("stat_requests"s);

            if(requests.IsArray())
            {
                for(const auto& request : requests.AsArray())
                {
                    if(IsRouteRequest(request))
                    {
                        result.push_back(ParseRouteAnswer(request.AsMap()));
                    }
                    if(IsStopRequest(request))
                    {
                        result.push_back(ParseStopAnswer(request.AsMap()));
                    }
                    if(IsMapRequest(request))
                    {
                        result.push_back(ParseMapAnswer(request.AsMap()));
                    }
                }
            }
        }
        return result;
    }

    json::Dict Reader::ErrorMessage(int id) const
    {
        auto map             = json::Dict();
        map["request_id"]    = id;
        map["error_message"] = "not found"s;
        return map;
    }
}    // namespace json