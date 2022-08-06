#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"
#include "graph.h"

#include <sstream>
#include <optional>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>

namespace json_pro
{
    JSONreader::JSONreader(
        transport_db::TransportCatalogue& t_c,
        std::string result_map_render,
        transport_router::TransportRouter& transport_router)
        :  t_c_(t_c)
        , result_map_render_(result_map_render)
        , transport_router_(transport_router) {}

    void JSONreader::LoadJSON( std::istream& input) {
        json::Document doc = json::Load(input);
        route_requests_ = doc.GetRoot().AsDict().at("routing_settings").AsDict();
                stat_requests_ = doc.GetRoot().AsDict().at("stat_requests").AsArray();
                stat_answer_ = doc.GetRoot().AsDict().at("stat_requests").AsArray();
        FillCatalogueStop( doc);
        FillCatalogueBus( doc);
        //auto& route_set = doc.GetRoot().AsDict().at("routing_settings").AsDict();
        auto& render_set = doc.GetRoot().AsDict().at("render_settings").AsDict();
        renderer::RenderSettings renset(render_set);
        renderer::MapRenderer ren(renset, t_c_);
        result_map_render_ = ren.DocumentMapToPrint();
        //graph::DirectedWeightedGraph<double> transport_graph(t_c_.GetAllStops().size());
        //SetGraphInfo(route_set, transport_router_);
        //transport_router_.FillCatalogueGraph(t_c_, transport_graph);
        //graph::Router transport_router(transport_graph);
        //PrintAnswer(/*result_map_render_, transport_router_*/);
    }

    void JSONreader::FillCatalogueStop( json::Document& doc) {
        for (const auto& node_map : doc.GetRoot().AsDict().at("base_requests").AsArray()) {
            if (node_map.AsDict().at("type").AsString() == "Stop") {
                t_c_.AddStop(geo::Coordinates{ node_map.AsDict().at("latitude").AsDouble(),  node_map.AsDict().at("longitude").AsDouble() }, node_map.AsDict().at("name").AsString());;
            }
        }
        for (const auto& node_map : doc.GetRoot().AsDict().at("base_requests").AsArray()) {
            const auto& map = node_map.AsDict();
            if (map.at("type").AsString() == "Stop") {
                for (const auto& [key, val] : map.at("road_distances").AsDict()) {
                    t_c_.SetDistance(t_c_.GetStopByName(map.at("name").AsString()), t_c_.GetStopByName(key), val.AsInt());
                }
            }
        }
    }

    void JSONreader::FillCatalogueBus( json::Document& doc) {
        for (const auto& node_map : doc.GetRoot().AsDict().at("base_requests").AsArray()) {
            const auto& map = node_map.AsDict();
            if (map.at("type").AsString() == "Bus") {
                domain::Bus bs;
                bs.bus_number = map.at("name").AsString();
                bs.is_roundtrip = map.at("is_roundtrip").AsBool();
                int j = map.at("stops").AsArray().size();
                for (size_t i = 0; i < j; ++i) {
                    bs.stops.push_back(t_c_.GetStopByName(map.at("stops").AsArray()[i].AsString()));
                }
                if (map.at("is_roundtrip").AsBool() == false && j >= 2) {
                    for (j = j - 2; j >= 0; ) {
                        bs.stops.push_back(t_c_.GetStopByName(map.at("stops").AsArray()[j].AsString()));
                        --j;
                    }
                }
                t_c_.AddRoute(bs);
            }
        }
    }

    void JSONreader::SetGraphInfo(const json::Dict& route_set, transport_router::TransportRouter& router)
    {
        int bus_wait_time = 0;
        double velocity = 0.0;
        for (const auto& [key, value] : route_set) {
            if (key == "bus_velocity") {
                velocity = value.AsDouble();
                router.SetVelocity(velocity);
            }
            else if (key == "bus_wait_time") {
                bus_wait_time = value.AsInt();
                router.SetWaitTime(bus_wait_time);
            }
        }
    }
}

void json_pro::JSONreader::PrintAnswer( 
    /*std::string result_map_render, transport_router::TransportRouter router*/)
{
    using namespace std::literals;
    graph::DirectedWeightedGraph<double> transport_graph(t_c_.GetAllStops().size());
    SetGraphInfo(route_requests_, transport_router_);
    transport_router_.FillCatalogueGraph(t_c_, transport_graph);
    graph::Router transport_router(transport_graph);
    json::Array arr{};
    //if (doc.GetRoot().AsDict().count("stat_requests")) {
        for (const auto& node_map : stat_answer_) {
            int id_q = node_map.AsDict().at("id").AsInt();
            if (node_map.AsDict().at("type").AsString()[0] == 'B') {
                arr.emplace_back(json_pro::JSONreader::PrintBus( node_map, id_q));
            }
            if (node_map.AsDict().at("type").AsString()[0] == 'S') {
                arr.emplace_back(json_pro::JSONreader::PrintStop( node_map, id_q));
            }
            if (node_map.AsDict().at("type").AsString()[0] == 'M') {
                arr.emplace_back(json_pro::JSONreader::PrintVisual(result_map_render_, id_q));
            }
            if (node_map.AsDict().at("type").AsString()[0] == 'R') {
                arr.emplace_back(json_pro::JSONreader::PrintGraph( node_map, id_q, transport_router, transport_router_));
            }
        }
    //}
    json::Print(
        json::Document{
        json::Builder{}
        .Value(arr)
        .Build()
        },
        std::cout
    );
}

json::Dict json_pro::JSONreader::PrintGraph( const json::Node& node_map, int id,
    graph::Router<double> transport_router, transport_router::TransportRouter router)
{
    using namespace std::literals;
    std::string tmp_from = node_map.AsDict().at("from").AsString();
    std::string tmp_to = node_map.AsDict().at("to").AsString();
    const domain::Stop* stop_from = t_c_.GetStopByName(tmp_from);
    const domain::Stop* stop_to = t_c_.GetStopByName(tmp_to);
    if (stop_from == stop_to) {
        return
            json::Builder{}
            .StartDict()
            .Key("total_time"s).Value(0)
            .Key("request_id"s).Value(id)
            .Key("items"s)
            .StartArray()
            .EndArray()
            .EndDict()
            .Build()
            .AsDict();
    }
    else
    {
        const auto route_info = transport_router.graph::Router<double>::BuildRoute(stop_from->edge_id, stop_to->edge_id);
        if (route_info.has_value()) {
            const auto& elem = route_info.value().edges;
            json::Array rout_arr;
            int wait_time = router.GetWaitTime();
            for (const auto& el : elem) {
                const auto& edge = transport_router.GetGraph().GetEdge(el);
                std::string stop_name{ t_c_.GetAllStops()[edge.from].name };
                json::Dict wait = json::Builder{}
                    .StartDict()
                    .Key("time"s).Value(wait_time)
                    .Key("type"s).Value("Wait"s)
                    .Key("stop_name"s).Value(stop_name)
                    .EndDict()
                    .Build()
                    .AsDict();
                rout_arr.push_back(wait);
                int span = edge.span_count;
                std::string bus_name{ edge.bus };
                json::Dict ride = json::Builder{}
                    .StartDict()
                    .Key("time"s).Value(edge.weight - wait_time)
                    .Key("span_count"s).Value(span)
                    .Key("bus"s).Value(bus_name)
                    .Key("type"s).Value("Bus"s)
                    .EndDict()
                    .Build()
                    .AsDict();
                rout_arr.push_back(ride);
            }
            return json::Builder{}
                .StartDict()
                .Key("total_time"s).Value(route_info.value().weight)
                .Key("request_id"s).Value(id).Key("items"s).Value(rout_arr)
                .EndDict()
                .Build()
                .AsDict();
        }
        else {
            return
                json::Builder{}
                .StartDict()
                .Key("request_id").Value(id)
                .Key("error_message").Value("not found"s)
                .EndDict()
                .Build()
                .AsDict();
        }
    }
    return
        json::Builder{}
        .StartDict()
        .Key("request_id").Value(id)
        .Key("error_message").Value("not found"s)
        .EndDict()
        .Build()
        .AsDict();
}

json::Dict json_pro::JSONreader::PrintVisual(std::string result_map_render, int id) {
    return
        json::Builder{}
        .StartDict()
        .Key("map").Value(result_map_render)
        .Key("request_id").Value(id)
        .EndDict()
        .Build()
        .AsDict();
}

json::Dict json_pro::JSONreader::PrintBus( const json::Node& node_map, int id) {
    using namespace std::literals;
    std::string tmp = node_map.AsDict().at("name").AsString();
    if (t_c_.GetRouteByName(tmp) != nullptr) {
        return
            json::Builder{}
            .StartDict()
            .Key("curvature").Value(t_c_.GetBusInfo(tmp).curvature_)
            .Key("request_id").Value(id)
            .Key("route_length").Value(t_c_.GetBusInfo(tmp).meters_route_length_)
            .Key("stop_count").Value(t_c_.GetBusInfo(tmp).stops_count_)
            .Key("unique_stop_count").Value(t_c_.GetBusInfo(tmp).unique_stops_)
            .EndDict()
            .Build()
            .AsDict();
    }
    else {
        return
            json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found"s)
            .EndDict()
            .Build()
            .AsDict();
    }
}

json::Dict json_pro::JSONreader::PrintStop( const json::Node& node_map, int id) {
    using namespace std::literals;
    std::string tmp = node_map.AsDict().at("name").AsString();
    if (t_c_.GetStopByName(tmp) != nullptr) {
        json::Array arr_bus{};
        for (const auto& elem : t_c_.GetStopInfo(tmp).bus_number_) {
            arr_bus.emplace_back(elem);
        }
        return
            json::Builder{}
            .StartDict()
            .Key("buses").Value(arr_bus)
            .Key("request_id").Value(id)
            .EndDict()
            .Build()
            .AsDict();
    }
    else {
        return
            json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found"s)
            .EndDict()
            .Build()
            .AsDict();
    }
}



