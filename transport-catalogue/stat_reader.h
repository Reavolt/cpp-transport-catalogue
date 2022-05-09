#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <queue>

namespace output
{
    class stat_reader
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

        std::queue<transport_catalogue::bus_info>  buses_info_{};
        std::queue<transport_catalogue::stop_info> stops_info_{};

        std::deque<query> request_data_{};

        transport_catalogue::TransportCatalogue* catalogue_ = nullptr;

    public:
        explicit stat_reader(transport_catalogue::TransportCatalogue* catalogue);

        void parse_transport_catalogue();

        [[nodiscard]] std::string parse_request_type(std::string& request);
        [[nodiscard]] std::string parse_name(const std::string& request);

        friend std::ostream& operator<<(std::ostream& input_stream, stat_reader& s_reader);
        friend std::istream& operator>>(std::istream& input_stream, stat_reader& s_reader);
    };
}    // namespace output