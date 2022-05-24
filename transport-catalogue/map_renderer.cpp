#include "map_renderer.h"

namespace renderer
{
    bool IsZero(double value)
    {
        return std::abs(value) < EPSILON;
    }

    svg::Point SphereProjector::operator()(geo::Coordinates coords) const
    {
        return {(coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
    }

    MapRenderer::MapRenderer() {}

    void MapRenderer::SetSettings(const RenderSettings& settings)
    {
        settings_ = settings;
    }

    void MapRenderer::SetStops(const Stops* stops)
    {
        stops_ = stops;
    }

    void MapRenderer::SetRoutes(const Buses* buses)
    {
        buses_ = buses;
    }

    void MapRenderer::SetStopNameToBus(const StopNameToBus* stopname_to_bus)
    {
        stopname_to_bus_ = stopname_to_bus;
    }

    svg::Document MapRenderer::RenderMap()
    {
        std::vector<geo::Coordinates> coordinates;
        for(const auto& [name, data] : *stops_)
        {
            if(!stopname_to_bus_->at(data).empty())
            {
                coordinates.push_back(data->coordinates_);
            }
        }
        SphereProjector projector(
          coordinates.begin(), coordinates.end(), settings_.width_, settings_.height_, settings_.padding_);

        svg::Document document;
        RenderLines(projector, document);
        return document;
    }

    void MapRenderer::RenderLines(const renderer::SphereProjector& projector, svg::Document& document) const
    {
        size_t color_index = 0;

        for(const auto& [name, data] : *buses_)
        {
            if(data->stops_.size() > 0)
            {
                svg::Polyline line;
                line.SetStrokeColor(settings_.color_palette_.at(color_index % settings_.color_palette_.size()));
                line.SetFillColor(svg::NoneColor);
                line.SetStrokeWidth(settings_.line_width_);
                line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                for(const auto& stop : data->stops_)
                {
                    line.AddPoint(projector(stop->coordinates_));
                }
                document.Add(line);
                ++color_index;
            }
        }
    }
}    // namespace renderer