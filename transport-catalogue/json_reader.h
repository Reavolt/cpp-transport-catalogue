#pragma once

#include "domain.h"
#include "json.h"
#include "transport_catalogue.h"

namespace json
{
    class Reader
    {
        using TransportCatalogue = transport_catalogue::TransportCatalogue*;

    public:
        explicit Reader(transport_catalogue::TransportCatalogue* catalogue);

        template<typename Stream>
        void Serialize(Stream& stream);

        template<typename InStream, typename OutStream>
        void Deserialize(InStream& input, OutStream& output);

    private:
        void        FillCatalogue();
        json::Array ReadCatalogue();

        bool IsStop(const json::Node& node);
        void ParseStops(const json::Array& data);

        bool IsRoute(const json::Node& node);
        void ParseRoutes(const json::Array& data);

        void ParseDistances(const json::Array& data);

    private:
        json::Document     json_data_;
        TransportCatalogue catalogue_ = nullptr;
    };

    template<typename Stream>
    void Reader::Serialize(Stream& stream)
    {
        json_data_ = std::move(json::Load(stream));
        FillCatalogue();
    }

    template<typename InStream, typename OutStream>
    void Reader::Deserialize(InStream& input, OutStream& output)
    {
        const auto& answers = ReadCatalogue();
        json::Print(json::Document(json::Node{answers}), output);
    }
}    // namespace json