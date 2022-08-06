#include "transport_catalogue.h"
#include "json_reader.h"
#include <iostream>

int main() {

    transport_db::TransportCatalogue catalogue;
    transport_router::TransportRouter router;
    std::string result_map_render;

    json_pro::JSONreader json_reader(catalogue, result_map_render, router);

    json_reader.LoadJSON( std::cin);
    json_reader.PrintAnswer();

}

