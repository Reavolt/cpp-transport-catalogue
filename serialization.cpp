#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>
#include <algorithm>
#include <map>

#include "serialization.h"
#include "transport_catalogue.pb.h"
#include "map_renderer.pb.h"
#include "geo.h"

namespace serialize {

    Serialization::Serialization(transport_db::TransportCatalogue& transport_catalogue)
        : transport_catalogue_(transport_catalogue) {}

    void Serialization::SetSetting(const std::filesystem::path& path_to_base) {
        path_ = path_to_base;
    }

    void Serialization::Serialize(transport_db::TransportCatalogue& cat)
    {
        std::ofstream out_file(path_, std::ios::binary);
        SerializeStops();
        SerializeDistanceFromTo();
        SerializeBuses();
        base_.SerializeToOstream(&out_file);
    }

    void Serialization::DeserializeCatalogue(transport_db::TransportCatalogue& cat) {
        std::ifstream in_file(path_, std::ios::binary);
        base_.ParseFromIstream(&in_file);
        DeserializeStops();
        DeserializeDistanceFromTo();
        DeserializeBuses();
    }

    proto_catalogue::Stop Serialization::SaveStop(const domain::Stop& stop) const {
        proto_catalogue::Stop proto_stop;
        proto_stop.set_id(stop.edge_id);
        proto_stop.set_stop_name(stop.name);
        proto_stop.mutable_coordinates()->set_lat(stop.coords.lat);
        proto_stop.mutable_coordinates()->set_lon(stop.coords.lng);
        return proto_stop;
    }
    
    proto_catalogue::DistanceFromTo Serialization::SaveDistanceFromTo(domain::Stop* from, domain::Stop* to, size_t distance) const {
        proto_catalogue::DistanceFromTo proto_dist;
        proto_dist.set_from(from->name);
        proto_dist.set_to(to->name);
        proto_dist.set_distance(distance);
        return proto_dist;
    }

    proto_catalogue::Bus Serialization::SaveBus(const domain::Bus& bus) const {
        proto_catalogue::Bus proto_bus;
        proto_bus.set_is_roundtrip(bus.is_roundtrip);
        proto_bus.set_bus_name(bus.bus_number);
        for (const auto& stop : bus.stops) {
            proto_bus.add_names_of_stops(stop->name);
        }
        return proto_bus;
    }

    void Serialization::SerializeStops() {
        for (const auto& stop : transport_catalogue_.GetAllStops()) {
            *base_.mutable_transport_base()->add_stops() = std::move(SaveStop(stop));
        }
    }

    void Serialization::SerializeDistanceFromTo() {
        for (const auto& [from_to, distance] : transport_catalogue_.GetStopsFromTo()) {
            *base_.mutable_transport_base()->add_distance_from_to() = std::move(SaveDistanceFromTo(from_to.first, from_to.second, distance));
        }
    }

    void Serialization::SerializeBuses() {
        for (const auto& bus : transport_catalogue_.GetAllBuses()) {
            *base_.mutable_transport_base()->add_buses() = std::move(SaveBus(bus));
        }
    }

    void Serialization::SerializeRouterSetVelosity(std::map <std::string, double> router_settings_velosity)
    {
        base_.mutable_router_set()->set_bus_velocity(router_settings_velosity.at("bus_velocity"));
    }

    void Serialization::SerializeRouterSetTime(std::map <std::string, int> router_settings_time)
    {
        base_.mutable_router_set()->set_bus_wait_time(router_settings_time.at("bus_wait_time"));
    }

    void Serialization::SerializeMapRender(std::string map) {
        base_.mutable_map_ren()->set_str_of_result_map_render(map);
    }

    void Serialization::LoadStop(const proto_catalogue::Stop& stop) {
        geo::Coordinates coordinates;
        coordinates.lat = stop.coordinates().lat();
        coordinates.lng = stop.coordinates().lon();
        transport_catalogue_.AddStop(coordinates, stop.stop_name());
    }

    void Serialization::LoadDistanceFromTo(const proto_catalogue::DistanceFromTo& distance_from_to) {
        const auto& from = transport_catalogue_.GetStopByName(distance_from_to.from());
        const auto& to = transport_catalogue_.GetStopByName(distance_from_to.to());
        transport_catalogue_.SetDistance(from, to, distance_from_to.distance());
    }

    void Serialization::LoadBus(const proto_catalogue::Bus& bus) {
        domain::Bus tmp;
        tmp.is_roundtrip = bus.is_roundtrip();
        tmp.bus_number = bus.bus_name();
        for (int i = 0; i < bus.names_of_stops_size(); ++i) {
            tmp.stops.push_back(transport_catalogue_.GetStopByName(bus.names_of_stops(i)));
        }
        transport_catalogue_.AddRoute(tmp);
    }

    void Serialization::DeserializeStops() {
        for (int i = 0; i < base_.transport_base().stops_size(); ++i) {
            LoadStop(base_.transport_base().stops(i));
        }
    }

    void Serialization::DeserializeDistanceFromTo() {
        for (int i = 0; i < base_.transport_base().distance_from_to_size(); ++i) {
            LoadDistanceFromTo(base_.transport_base().distance_from_to(i));
        }
    }

    void Serialization::DeserializeBuses() {
        for (int i = 0; i < base_.transport_base().buses_size(); ++i) {
            LoadBus(base_.transport_base().buses(i));
        }
    }

    std::string Serialization::DeserializeMapRenderer() {
        return base_.map_ren().str_of_result_map_render();
    }

    double Serialization::DeserializeRouterSetVelosity() {
        return base_.router_set().bus_velocity();
    }

   int Serialization::DeserializeRouterSetTime() {
       return base_.router_set().bus_wait_time();
    }
}