#pragma once

#include "transport_catalogue.h"
#include "detail.h"

#include <iostream>
#include <queue>
#include <string>
#include <vector>

namespace input
{
    using namespace std::literals;

    class Reader
    {
    public:    //types
        using StopsDistance = std::vector<std::pair<domain::Stop*, int>>;
        using StopsData     = std::vector<const domain::Stop*>;

    private:    //types
        enum class QueryType
        {
            STOP,
            BUS,
        };

        struct Query
        {
            QueryType   type_;
            std::string request_{};

            Query(QueryType type, const std::string& request) : type_(type), request_(request) {}
        };

        std::deque<Query> stop_queries_{};
        std::deque<Query> bus_queries_{};

        transport_catalogue::TransportCatalogue* catalogue_ = nullptr;

    private:    //methods
        void FillCatalogue();

    public:    //methods
        explicit Reader(transport_catalogue::TransportCatalogue* catalogue);

        [[nodiscard]] std::string      ParseRequestType(std::string& request);
        [[nodiscard]] std::string      ParseName(std::string& request);
        [[nodiscard]] geo::Coordinates ParseCoords(std::string& request);
        [[nodiscard]] StopsDistance    ParseStopsDistance(std::string& request);

        [[nodiscard]] StopsData ParseStandartRoute(std::string& request);
        [[nodiscard]] StopsData ParseRingRoute(std::string& request);

        [[nodiscard]] domain::Stop ParseStop(std::string& request);
        [[nodiscard]] domain::Bus  ParseBus(std::string& request);

        template<typename Stream>
        void ReadFromStream(Stream& stream);
    };

    template<typename Stream>
    void Reader::ReadFromStream(Stream& stream)
    {
        size_t request_count = 0;
        stream >> request_count;
        stream.ignore();

        for(size_t i = 0; i < request_count; ++i)
        {
            std::string request;
            std::getline(stream, request);

            std::string request_type(ParseRequestType(request));

            if(request_type == "Stop"s)
            {
                stop_queries_.emplace_back(Reader::QueryType::STOP, request);
            }

            if(request_type == "Bus"s)
            {
                bus_queries_.emplace_back(Reader::QueryType::BUS, request);
            }
        }
        FillCatalogue();
    }
}    // namespace input