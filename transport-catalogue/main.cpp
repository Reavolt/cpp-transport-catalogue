#include "input_reader.h"
#include "stat_reader.h"
#include "json_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <string_view>

int main()
{
    transport_catalogue::TransportCatalogue test_catalogue;

    json::Reader test(&test_catalogue);
    test.Serialize(std::cin);
    test.Deserialize(std::cout);

    // transport_catalogue::TransportCatalogue catalogue;

    // input::Reader in_reader(&catalogue);
    // in_reader.ReadFromStream(std::cin);

    // output::Reader out_reader(&catalogue);
    // out_reader.ReadFromStream(std::cin);
    // out_reader.WriteToStream(std::cout);
}