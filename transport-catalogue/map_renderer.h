#pragma once

#include "svg.h"
#include "json_reader.h"
#include "geo.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <tuple>

namespace sphere
{
	inline const double EPSILON = 1e-6;
	bool IsZero(double value);

	class SphereProjector
	{
	public:
		SphereProjector() = default;
		// points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding)
			: padding_(padding) //
		{
			// Если точки поверхности сферы не заданы, вычислять нечего
			if (points_begin == points_end) {
				return;
			}
			// Находим точки с минимальной и максимальной долготой
			const auto [left_it, right_it]
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
					{
						return lhs.lng < rhs.lng;
					});
			min_lon_ = left_it->lng;
			const double max_lon = right_it->lng;
			// Находим точки с минимальной и максимальной широтой
			const auto [bottom_it, top_it] = std::minmax_element(
				points_begin, points_end,
				[](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
			const double min_lat = bottom_it->lat;
			max_lat_ = top_it->lat;
			// Вычисляем коэффициент масштабирования вдоль координаты x
			std::optional<double> width_zoom;
			if (!IsZero(max_lon - min_lon_)) {
				width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
			}
			// Вычисляем коэффициент масштабирования вдоль координаты y
			std::optional<double> height_zoom;
			if (!IsZero(max_lat_ - min_lat)) {
				height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
			}
			if (width_zoom && height_zoom) {
				// Коэффициенты масштабирования по ширине и высоте ненулевые,
				// берём минимальный из них
				zoom_coeff_ = std::min(*width_zoom, *height_zoom);
			}
			else if (width_zoom) {
				// Коэффициент масштабирования по ширине ненулевой, используем его
				zoom_coeff_ = *width_zoom;
			}
			else if (height_zoom) {
				// Коэффициент масштабирования по высоте ненулевой, используем его
				zoom_coeff_ = *height_zoom;
			}
		}

		// Проецирует широту и долготу в координаты внутри SVG-изображения
		svg::Point operator()(geo::Coordinates coords) const;

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	};
}//namespace

namespace renderer
{
	struct Doc
	{
		std::vector<svg::Polyline> shape_buses;
		std::vector<svg::Text> shape_name_buses;
		std::map<std::string, svg::Circle> shape_circle_stops;
		std::map<std::string, std::pair<svg::Text, svg::Text>> shape_name_stops;
	};

	struct ShapeTextNameStop
	{
		std::string name_stop;
		svg::Text shape_name_stop;
		svg::Text shape_name_stop_substr;
	};

	struct ShapeCircleStop
	{
		std::string name_stop;
		svg::Circle shape_stop;
	};

	struct BusSVG
	{
		svg::Polyline shape_route_bus;
		std::vector<svg::Text> shape_name_bus;
		std::vector<ShapeCircleStop> shape_circle_stops;
		std::vector<ShapeTextNameStop> shape_name_stops;
	};

	class RenderSettings {
	public:
		explicit RenderSettings(const json::Dict& render_settings);

		double width = 0.;
		double height = 0.;
		double padding = 0.;
		double line_width = 0.;
		double stop_radius = 0.;

		double bus_label_font_size = 0.;
		geo::Coordinates bus_label_offset = { 0.0, 0.0 };

		double stop_label_font_size = 0.;
		geo::Coordinates stop_label_offset = { 0.0, 0.0 };

		svg::Color underlayer_color{};
		double underlayer_width = 0.;

		std::vector<svg::Color> color_palette{};

		inline svg::Color AddColor(const json::Node& node);
	};

	class MapRenderer
	{
	public:
		explicit MapRenderer() = default;
		explicit MapRenderer(const RenderSettings& settings, transport_db::TransportCatalogue& t_c);

		svg::Color ColorSetting(uint32_t index);

		inline svg::Polyline AddRoute(const domain::Bus& bus, const svg::Color& color);

		inline void PushBusSVG(const domain::Bus& bus, const svg::Color& color);

		void SetSphereProjector(transport_db::TransportCatalogue& tc);

		void AddBusSVG(transport_db::TransportCatalogue& tc);

		std::string DocumentMapToPrint()const;

		inline svg::Text TextSvgForBus(const svg::Point& pos, const std::string& data);

		inline svg::Text CreateSVGTextForBus(const svg::Point& pos, const svg::Color& color, const std::string& data);

		inline svg::Text CreateSVGTextForBus(const svg::Point& pos, const std::string& data);

		inline std::vector<svg::Text> AddNameBus(const domain::Bus& bus, const svg::Color& color);

		inline svg::Text TextSvgForStop(const svg::Point& pos, const std::string& data);

		inline svg::Text CreateSVGTextForStop(const svg::Point& pos, const svg::Color& color, const std::string& data);

		inline svg::Text CreateSVGTextForStop(const svg::Point& pos, const std::string& data);

		inline std::vector<ShapeTextNameStop> AddNameStops(const domain::Bus& bus);

		inline std::vector<ShapeCircleStop> AddCircleStops(const domain::Bus& bus);

		inline Doc PrepareDoc(const std::vector<BusSVG>& buses)const;

	private:

		const RenderSettings& render_settings_;

		sphere::SphereProjector s_;
		std::vector<BusSVG > shape_bus_route;
	};

}//namespace
