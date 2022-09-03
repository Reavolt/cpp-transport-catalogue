#include <fstream>
#include <iostream>
#include <string_view>

#include "json_reader.h" 
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include "serialization.h"

using namespace std;
using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    transport_db::TransportCatalogue catalogue;
    transport_router::TransportRouter router;
    std::string result_map_render;
    serialize::Serialization serializator(catalogue);
    json_pro::JSONreader json_reader(serializator, catalogue, result_map_render, router);

    if (mode == "make_base"sv) {

        json_reader.LoadJSON(std::cin);

    }
    else if (mode == "process_requests"sv) {

        json_reader.ReadRequests(std::cin);
        json_reader.PrintAnswer();
    }
    else {
        PrintUsage();
        return 1;
    }
}

   