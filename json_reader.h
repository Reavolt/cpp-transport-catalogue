#pragma once

#include "transport_catalogue.h"
#include "json_builder.h"
#include "router.h"
#include "transport_router.h"
#include "router.h"
#include "serialization.h"

#include <iostream>

namespace json_pro {
	class JSONreader {
	private:
		serialize::Serialization& serializator_;
		transport_db::TransportCatalogue& t_c_;
		std::string result_map_render_{};
		transport_router::TransportRouter& transport_router_;
		json::Array base_requests_{};
		json::Dict route_requests_{};
		json::Array stat_requests_{};
		json::Dict render_requests_{};

	public:
		explicit JSONreader(serialize::Serialization& serializator,
			transport_db::TransportCatalogue& t_c,
			std::string result_map_render,
			transport_router::TransportRouter& transport_router);

		void LoadJSON( std::istream& input);
		void ReadRequests(std::istream& input);

		void FillCatalogueStop( json::Document& doc);
		void FillCatalogueBus( json::Document& doc);
		void SetGraphInfo(const json::Dict& doc, transport_router::TransportRouter& router);

		void PrintAnswer();

		std::filesystem::path DoSerialization(const json::Dict& queryset);
		json::Dict PrintStop( const json::Node& node_map, int id);
		json::Dict PrintBus( const json::Node& node_map, int id);
		json::Dict PrintGraph( const json::Node& node_map, int id, graph::Router<double> transport_router, transport_router::TransportRouter router);
		json::Dict PrintVisual(std::string result_map_render, int id);
	};
}

