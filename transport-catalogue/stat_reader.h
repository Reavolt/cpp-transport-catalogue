#pragma once

#include "transport_catalogue.h"

#include <iomanip>
#include <iostream>
#include <queue>

namespace output
{
    class Reader
    {
    private:
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

        std::deque<Query> request_data_{};
        transport_catalogue::TransportCatalogue* catalogue_ = nullptr;

    private:
        void        ParseCatalogue();
        std::string PrintBus(const transport_catalogue::BusInfo& bus);
        std::string PrintStop(const transport_catalogue::StopInfo& bus);

    public:
        explicit Reader(transport_catalogue::TransportCatalogue* catalogue);

        [[nodiscard]] std::string ParseRequestType(std::string& request);

        template<typename Stream>
        void ReadFromStream(Stream& stream);
        template<typename Stream>
        void WriteToStream(Stream& stream);
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

            std::string request_type = ParseRequestType(request);

            if(request_type == "Bus")
            {
                request_data_.push_back({Reader::QueryType::BUS, request});
            }
            if(request_type == "Stop")
            {
                request_data_.push_back({Reader::QueryType::STOP, request});
            }
        }
    }

    template<typename Stream>
    void Reader::WriteToStream(Stream& stream)
    {
        for(const auto& data : request_data_)
        {
            const auto request_info = data;

            if(request_info.type_ == Reader::QueryType::BUS)
            {
                const auto bus_info = catalogue_->GetBusInfo(request_info.request_);
                stream << PrintBus(bus_info);
            }
            if(request_info.type_ == Reader::QueryType::STOP)
            {
                const auto stop_info = catalogue_->GetStopInfo(request_info.request_);
                stream << PrintStop(stop_info);
            }
        }
        request_data_.clear();
    }
}    // namespace output