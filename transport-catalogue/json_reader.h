#pragma once

#include "transport_catalogue.h"
#include "json_builder.h"
#include "router.h"
#include "transport_router.h"
#include "router.h"

#include <iostream>

namespace json_pro {
	class JSONreader {
	private:

		transport_db::TransportCatalogue& t_c_;
		std::string result_map_render_;
		transport_router::TransportRouter& transport_router_;
		json::Dict route_requests_;
		json::Array stat_requests_;
		json::Array stat_answer_;

	public:
		explicit JSONreader(
			transport_db::TransportCatalogue& t_c,
			std::string result_map_render,
			transport_router::TransportRouter& transport_router);

		void LoadJSON( std::istream& input);

		void FillCatalogueStop( json::Document& doc);
		void FillCatalogueBus( json::Document& doc);
		void SetGraphInfo(const json::Dict& doc, transport_router::TransportRouter& router);

		void PrintAnswer();
		json::Dict PrintStop( const json::Node& node_map, int id);
		json::Dict PrintBus( const json::Node& node_map, int id);
		json::Dict PrintGraph( const json::Node& node_map, int id, graph::Router<double> transport_router, transport_router::TransportRouter router);
		json::Dict PrintVisual(std::string result_map_render, int id);
	};
}

