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
            if(stopname_to_bus_->count(data) && !stopname_to_bus_->at(data).empty())
            {
                coordinates.push_back(data->coordinates_);
            }
        }
        SphereProjector projector(
          coordinates.begin(), coordinates.end(), settings_.width_, settings_.height_, settings_.padding_);

        std::set<const domain::Stop*, LexSort> stops_to_render;
        for(const auto& data : *stops_)
        {
            if(data.second)
            {
                stops_to_render.insert(data.second);
            }
        }

        std::set<const domain::Bus*, LexSort> buses_to_render;
        for(const auto& data : *buses_)
        {
            if(data.second)
            {
                buses_to_render.insert(data.second);
            }
        }

        svg::Document document;
        RenderLines(projector, document, buses_to_render);
        RenderRouteNames(projector, document, buses_to_render);
        RenderStopsPoint(projector, document, stops_to_render);
        RenderStopNames(projector, document, stops_to_render);
        return document;
    }

    void MapRenderer::RenderLines(const renderer::SphereProjector&             projector,
                                  svg::Document&                               document,
                                  const std::set<const domain::Bus*, LexSort>& buses_to_render) const
    {
        size_t color_index = 0;

        for(const auto& bus : buses_to_render)
        {
            if(bus->stops_.size() > 0)
            {
                svg::Polyline line;
                line.SetStrokeColor(settings_.color_palette_.at(color_index % settings_.color_palette_.size()));
                line.SetFillColor(svg::NoneColor);
                line.SetStrokeWidth(settings_.line_width_);
                line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                for(const auto& stop : bus->stops_)
                {
                    line.AddPoint(projector(stop->coordinates_));
                }

                if(bus->route_type_ == domain::RouteType::LINEAR)
                {
                    for(auto iter = std::next(bus->stops_.rbegin()); iter < bus->stops_.rend(); ++iter)
                    {
                        line.AddPoint(projector((*iter)->coordinates_));
                    }
                }
                document.Add(line);
                ++color_index;
            }
        }
    }

    void MapRenderer::RenderRouteNames(const renderer::SphereProjector&             projector,
                                       svg::Document&                               document,
                                       const std::set<const domain::Bus*, LexSort>& buses_to_render) const
    {
        size_t color_index = 0;
        for(const auto& bus : buses_to_render)
        {
            if(bus->stops_.size() > 0)
            {
                svg::Text text;
                text.SetData(bus->name_);
                text.SetPosition(projector(bus->stops_.front()->coordinates_));
                text.SetOffset(settings_.bus_label_offset_);
                text.SetFontSize(static_cast<std::uint32_t>(settings_.bus_label_font_size_));
                text.SetFontFamily("Verdana");
                text.SetFontWeight("bold");

                svg::Text underlayer_text(text);

                text.SetFillColor(settings_.color_palette_.at(color_index % settings_.color_palette_.size()));

                underlayer_text.SetFillColor(settings_.underlayer_color_);
                underlayer_text.SetStrokeColor(settings_.underlayer_color_);
                underlayer_text.SetStrokeWidth(settings_.underlayer_width_);
                underlayer_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                underlayer_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                document.Add(underlayer_text);
                document.Add(text);

                if(bus->route_type_ == domain::RouteType::LINEAR && !bus->stops_.empty() &&
                   bus->stops_.front() != bus->stops_.back())
                {
                    text.SetPosition(projector(bus->stops_.back()->coordinates_));
                    underlayer_text.SetPosition(projector(bus->stops_.back()->coordinates_));
                    document.Add(underlayer_text);
                    document.Add(text);
                }
                ++color_index;
            }
        }
    }

    void MapRenderer::RenderStopsPoint(const renderer::SphereProjector&              projector,
                                       svg::Document&                                document,
                                       const std::set<const domain::Stop*, LexSort>& stops_to_render) const
    {
        svg::Circle circle;
        for(const auto& stop : stops_to_render)
        {
            if(stopname_to_bus_->count(stop) && !stopname_to_bus_->at(stop).empty())
            {
                circle.SetRadius(settings_.stop_radius_).SetFillColor("white").SetCenter(projector(stop->coordinates_));
                document.Add(circle);
            }
        }
    }

    void MapRenderer::RenderStopNames(const renderer::SphereProjector&              projector,
                                      svg::Document&                                document,
                                      const std::set<const domain::Stop*, LexSort>& stops_to_render) const
    {
        for(const auto& stop : stops_to_render)
        {
            if(stopname_to_bus_->count(stop) && !stopname_to_bus_->at(stop).empty())
            {
                svg::Text text;
                text.SetData(std::string(stop->name_));
                text.SetPosition(projector(stop->coordinates_));
                text.SetOffset(settings_.stop_label_offset_);
                text.SetFontSize(static_cast<std::uint32_t>(settings_.stop_label_font_size_));
                text.SetFontFamily("Verdana");

                svg::Text underlayer_text(text);
                text.SetFillColor("black");

                underlayer_text.SetFillColor(settings_.underlayer_color_);
                underlayer_text.SetStrokeColor(settings_.underlayer_color_);
                underlayer_text.SetStrokeWidth(settings_.underlayer_width_);
                underlayer_text.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                underlayer_text.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                document.Add(underlayer_text);
                document.Add(text);
            }
        }
    }
}    // namespace renderer