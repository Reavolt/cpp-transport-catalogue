#include "input_reader.h"
#include "test_framework.h"
#include "transport_catalogue.h"

namespace transport_catalogue::tests
{
    using namespace input;
    using namespace transport_catalogue;

    void parse_request_type()
    {
        TransportCatalogue catalogue;
        {
            input_reader i_reader(&catalogue);
            std::string  request("Stop Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Stop", i_reader.parse_request_type(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo: 55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Bus test: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("Bus", i_reader.parse_request_type(request));
            ASSERT_EQUAL(request, "test: Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Stop  Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Stop", i_reader.parse_request_type(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo: 55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Bus    test: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("Bus", i_reader.parse_request_type(request));
            ASSERT_EQUAL(request, "test: Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        std::cout << "parse_request_type: PASSED" << std::endl;
    }

    void parse_name()
    {
        TransportCatalogue catalogue;
        {
            input_reader i_reader(&catalogue);
            std::string  request("Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("750: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("750", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request(" Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Tolstopaltsevo : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("   Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Tolstopaltsevo   : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("Biryulyovo Tovarnaya: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request("   Biryulyovo Tovarnaya   : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", i_reader.parse_name(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        std::cout << "parse_name: PASSED" << std::endl;
    }

    void parse_coords()
    {
        TransportCatalogue catalogue;
        {
            input_reader i_reader(&catalogue);
            std::string  request("55.611087, 37.208290"s);
            Coordinates  coordinates = i_reader.parse_coords(request);
            ASSERT_EQUAL(55.611087, coordinates.lat);
            ASSERT_EQUAL(37.208290, coordinates.lng);
            ASSERT_EQUAL(request, "");
        }
        std::cout << "parse_coords: PASSED" << std::endl;
    }

    void parse_stops_distance()
    {
        TransportCatalogue catalogue;
        catalogue.add_stop({"Tolstopaltsevo", {55.574371, 37.6517}});
        catalogue.add_stop({"Marushkino", {55.574371, 37.6517}});
        {
            input_reader                       i_reader(&catalogue);
            std::string                        request("9900m to Tolstopaltsevo, 100m to Marushkino"s);
            std::vector<std::pair<stop*, int>> stops_distance = i_reader.parse_stops_distance(request);
            ASSERT_EQUAL("Tolstopaltsevo", stops_distance[0].first->name_);
            ASSERT_EQUAL(9900, stops_distance[0].second);
            ASSERT_EQUAL("Marushkino", stops_distance[1].first->name_);
            ASSERT_EQUAL(100, stops_distance[1].second);
        }
        std::cout << "parse_stops_distance: PASSED" << std::endl;
    }

    void parse_standart_route()
    {
        TransportCatalogue catalogue;
        catalogue.add_stop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});
        catalogue.add_stop({"Biryusinka", {55.581065, 37.64839}});
        catalogue.add_stop({"Universam", {55.587655, 37.645687}});
        catalogue.add_stop({"Biryulyovo Tovarnaya", {55.592028, 37.653656}});
        catalogue.add_stop({"Biryulyovo Passazhirskaya", {55.580999, 37.659164}});
        catalogue.add_stop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});

        catalogue.add_stop({"Biryulyovo   Zapadnoye", {55.574371, 37.6517}});
        catalogue.add_stop({"Biryulyovo      Tovarnaya", {55.574371, 37.6517}});
        catalogue.add_stop({"Biryulyovo       Passazhirskaya", {55.574371, 37.6517}});

        {
            input_reader i_reader(&catalogue);
            std::string  request(
              "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s);
            std::vector<const stop*> parsed_data = i_reader.parse_standart_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request(
              " Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s);
            std::vector<const stop*> parsed_data = i_reader.parse_standart_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request(
              "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye "s);
            std::vector<const stop*> parsed_data = i_reader.parse_standart_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request(
              "   Biryulyovo Zapadnoye    >   Biryusinka   >  Universam  >    Biryulyovo Tovarnaya    >   Biryulyovo Passazhirskaya   >  Biryulyovo Zapadnoye "s);
            std::vector<const stop*> parsed_data = i_reader.parse_standart_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input_reader i_reader(&catalogue);
            std::string  request(
              "   Biryulyovo   Zapadnoye    >   Biryusinka   >  Universam  >    Biryulyovo      Tovarnaya    >   Biryulyovo       Passazhirskaya   >  Biryulyovo   Zapadnoye "s);
            std::vector<const stop*> parsed_data = i_reader.parse_standart_route(request);
            ASSERT_EQUAL("Biryulyovo   Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo      Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo       Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo   Zapadnoye", parsed_data[5]->name_);
        }
        std::cout << "parse_standart_route: PASSED" << std::endl;
    }

    void parse_ring_route()
    {
        TransportCatalogue catalogue;
        catalogue.add_stop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});
        catalogue.add_stop({"Biryusinka", {55.581065, 37.64839}});
        catalogue.add_stop({"Universam", {55.587655, 37.645687}});

        catalogue.add_stop({"Biryulyovo     Zapadnoye", {55.587655, 37.645687}});

        {
            input_reader             i_reader(&catalogue);
            std::string              request("Biryulyovo Zapadnoye - Biryusinka - Universam"s);
            std::vector<const stop*> parsed_data = i_reader.parse_ring_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[4]->name_);
        }
        {
            input_reader             i_reader(&catalogue);
            std::string              request(" Biryulyovo Zapadnoye - Biryusinka - Universam"s);
            std::vector<const stop*> parsed_data = i_reader.parse_ring_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[4]->name_);
        }
        {
            input_reader             i_reader(&catalogue);
            std::string              request("Biryulyovo Zapadnoye - Biryusinka - Universam "s);
            std::vector<const stop*> parsed_data = i_reader.parse_ring_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[4]->name_);
        }
        {
            input_reader             i_reader(&catalogue);
            std::string              request("   Biryulyovo Zapadnoye    -   Biryusinka   -  Universam "s);
            std::vector<const stop*> parsed_data = i_reader.parse_ring_route(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[4]->name_);
        }
        {
            input_reader             i_reader(&catalogue);
            std::string              request("   Biryulyovo     Zapadnoye    -   Biryusinka   -  Universam "s);
            std::vector<const stop*> parsed_data = i_reader.parse_ring_route(request);
            ASSERT_EQUAL("Biryulyovo     Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo     Zapadnoye", parsed_data[4]->name_);
        }
        std::cout << "parse_ring_route: PASSED" << std::endl;
    }

    void all_input_tests()
    {
        parse_request_type();
        parse_name();
        parse_coords();
        parse_stops_distance();
        parse_standart_route();
        parse_ring_route();
    }
}    // namespace transport_catalogue::tests