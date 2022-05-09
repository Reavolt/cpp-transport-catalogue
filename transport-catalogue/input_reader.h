#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <queue>
#include <string>
#include <vector>

namespace input
{
    using namespace std::literals;

    class input_reader
    {
    private:
        enum class query_type
        {
            STOP,
            BUS,
        };

        struct query
        {
            query_type  type_;
            std::string request_{};

            query(query_type type, const std::string& request) : type_(type), request_(request) {}
        };

        std::deque<query> stop_queries_{};
        std::deque<query> bus_queries_{};

        transport_catalogue::TransportCatalogue* catalogue_ = nullptr;

    public:
        explicit input_reader(transport_catalogue::TransportCatalogue* catalogue);

        void fill_transport_catalogue();

        [[nodiscard]] std::string                                             parse_request_type(std::string& request);
        [[nodiscard]] std::string                                             parse_name(std::string& request);
        [[nodiscard]] Coordinates                                             parse_coords(std::string& request);
        [[nodiscard]] std::vector<std::pair<transport_catalogue::stop*, int>> parse_stops_distance(std::string& request);

        [[nodiscard]] std::vector<const transport_catalogue::stop*> parse_standart_route(std::string& request);
        [[nodiscard]] std::vector<const transport_catalogue::stop*> parse_ring_route(std::string& request);

        transport_catalogue::stop parse_stop(std::string& request);
        transport_catalogue::bus  parse_bus(std::string& request);

        friend std::istream& operator>>(std::istream& stream, input_reader& i_reader);
    };
}    // namespace input