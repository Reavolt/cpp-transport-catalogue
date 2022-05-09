#pragma once

#include "geo.h"

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue
{
    struct stop
    {
        std::string name_{};
        Coordinates coordinates_{};

        stop() = default;
        stop(const std::string& name, const Coordinates& coordinates) : name_(name), coordinates_(coordinates) {}
    };

    struct bus
    {
        std::string              name_{};
        std::vector<const stop*> stops_{};

        bus() = default;
        bus(const std::string& name, const std::vector<const stop*>& stops) : name_(name), stops_(stops) {}
    };

    struct bus_info
    {
        std::string name_{};
        size_t      stops_count_      = 0;
        size_t      uniq_stops_count_ = 0;
        int         bus_length_       = 0;
        double      curvature_        = 0.0;
    };

    struct stop_info
    {
        std::string              name_{};
        std::vector<std::string> buses_name_{};
        bool                     not_found_ = false;
    };

    struct stop_ptr_hasher
    {
        size_t operator()(const std::pair<const stop*, const stop*>& p) const
        {
            return pointer_hasher(p.first) * 37 + pointer_hasher(p.second) * 37 * 37;
        }
        size_t operator()(const stop* p) const
        {
            return pointer_hasher(p) * 37;
        }

    private:
        std::hash<const void*> pointer_hasher{};
    };

    class TransportCatalogue
    {
    public:
        void                add_stop(const stop& stop_data);
        [[nodiscard]] stop* find_stop(const std::string_view name) const;
        void                add_bus(const bus& bus_data);
        [[nodiscard]] bus*  find_bus(const std::string_view name) const;

        void set_distance_between_stops(const std::string_view from_stop, const std::string_view to_stop, const int distance);
        [[nodiscard]] int get_distance_between_stops(const std::string_view from_stop, const std::string_view to_stop) const;

        [[nodiscard]] bus_info  get_bus_info(const std::string_view bus_name) const;
        [[nodiscard]] stop_info get_stop_info(const std::string_view bus_name) const;

    private:
        [[nodiscard]] size_t get_uniq_stops_count(const std::vector<const stop*> bus_stops) const;
        [[nodiscard]] int    get_route_length(const std::vector<const stop*> bus_stops) const;
        [[nodiscard]] double get_route_distance(const std::vector<const stop*> bus_stops) const;

    private:
        std::deque<stop>                                                           stops_{};
        std::unordered_map<std::string_view, stop*>                                stopname_to_stop_{};
        std::unordered_map<const stop*, std::unordered_set<bus*>, stop_ptr_hasher> stopname_to_bus_{};

        std::deque<bus>                            buses_{};
        std::unordered_map<std::string_view, bus*> busname_to_bus_{};

        std::unordered_map<std::pair<stop*, stop*>, int, stop_ptr_hasher> stops_to_distance_{};
    };
}    // namespace transport_catalogue