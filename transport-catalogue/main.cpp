#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

#include <iostream>
#include <string_view>

int main()
{
    transport_catalogue::TransportCatalogue test_catalogue;

    json::Reader test(&test_catalogue);
    test.Serialize(std::cin);
    test.Deserialize(std::cout);

    // input::Reader in_reader(&catalogue);
    // in_reader.ReadFromStream(std::cin);

    // output::Reader out_reader(&catalogue);
    // out_reader.ReadFromStream(std::cin);
    // out_reader.WriteToStream(std::cout);
}