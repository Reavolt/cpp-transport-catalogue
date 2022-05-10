#include "input_reader.h"
#include "input_tests.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>

int main()
{
    transport_catalogue::tests::all_input_tests();

    transport_catalogue::TransportCatalogue catalogue;

    input::Reader in_reader(&catalogue);
    in_reader.ReadFromStream(std::cin);

    output::Reader out_reader(&catalogue);
    out_reader.ReadFromStream(std::cin);
    out_reader.WriteToStream(std::cout);

    return 0;
}