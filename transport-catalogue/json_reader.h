#pragma once

#include "domain.h"
#include "json.h"
#include "map_renderer.h"
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

        std::optional<renderer::RenderSettings> ParseRenderSettings();

        template<typename Stream>
        void Deserialize(Stream& output);

    private:
        void        FillCatalogue();
        json::Array ReadCatalogue();

        bool IsStop(const json::Node& node) const;
        void ParseStops(const json::Array& data);
        bool IsRoute(const json::Node& node) const;
        void ParseRoutes(const json::Array& data);
        void ParseDistances(const json::Array& data);

        bool IsRouteRequest(const json::Node& node) const;
        Dict ParseRouteAnswer(const json::Dict& request) const;
        bool IsStopRequest(const json::Node& node) const;
        Dict ParseStopAnswer(const json::Dict& request) const;

        svg::Color               ReadColor(const json::Node& color) const;
        svg::Point               ReadOffset(const json::Array& offset) const;
        renderer::RenderSettings ParseSettings(const json::Dict& data);

        bool IsMapRequest(const json::Node& node);
        Dict ParseMapAnswer(const json::Dict& request);

        Dict ErrorMessage(int id) const;

    private:
        json::Document           json_data_;
        TransportCatalogue       catalogue_;
        renderer::RenderSettings render_settings_;
    };

    template<typename Stream>
    void Reader::Serialize(Stream& stream)
    {
        json_data_ = std::move(json::Load(stream));
        FillCatalogue();
    }

    template<typename Stream>
    void Reader::Deserialize(Stream& output)
    {
        const auto& answers = ReadCatalogue();
        json::Print(json::Document(json::Node{answers}), output);
    }
}    // namespace json