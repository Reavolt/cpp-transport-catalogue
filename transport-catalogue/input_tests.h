#include "input_reader.h"
#include "test_framework.h"
#include "transport_catalogue.h"

namespace transport_catalogue::tests
{
    using namespace std::string_literals;

    void ParseRequestType()
    {
        TransportCatalogue catalogue;
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Stop Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Stop", i_reader.ParseRequestType(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo: 55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Bus test: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("Bus", i_reader.ParseRequestType(request));
            ASSERT_EQUAL(request, "test: Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Stop  Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Stop", i_reader.ParseRequestType(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo: 55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Bus    test: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("Bus", i_reader.ParseRequestType(request));
            ASSERT_EQUAL(request, "test: Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        std::cout << "ParseRequestType: PASSED" << std::endl;
    }

    void ParseName()
    {
        TransportCatalogue catalogue;
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("750: Tolstopaltsevo - Marushkino - Rasskazovka"s);
            ASSERT_EQUAL("750", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "Tolstopaltsevo - Marushkino - Rasskazovka");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request(" Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Tolstopaltsevo : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("   Tolstopaltsevo: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Tolstopaltsevo   : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Tolstopaltsevo", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("Biryulyovo Tovarnaya: 55.611087, 37.208290"s);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request("   Biryulyovo Tovarnaya   : 55.611087, 37.208290"s);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", i_reader.ParseName(request));
            ASSERT_EQUAL(request, "55.611087, 37.208290");
        }
        std::cout << "ParseName: PASSED" << std::endl;
    }

    void ParseCoords()
    {
        TransportCatalogue catalogue;
        {
            input::Reader    i_reader(&catalogue);
            std::string      request("55.611087, 37.208290"s);
            geo::Coordinates coordinates = i_reader.ParseCoords(request);
            ASSERT_EQUAL(55.611087, coordinates.lat);
            ASSERT_EQUAL(37.208290, coordinates.lng);
            ASSERT_EQUAL(request, "");
        }
        std::cout << "ParseCoords: PASSED" << std::endl;
    }

    void ParseStopsDistance()
    {
        TransportCatalogue catalogue;
        catalogue.AddStop({"Tolstopaltsevo", {55.574371, 37.6517}});
        catalogue.AddStop({"Marushkino", {55.574371, 37.6517}});
        {
            input::Reader                              i_reader(&catalogue);
            std::string                                request("9900m to Tolstopaltsevo, 100m to Marushkino"s);
            std::vector<std::pair<domain::Stop*, int>> stops_distance = i_reader.ParseStopsDistance(request);
            ASSERT_EQUAL("Tolstopaltsevo", stops_distance[0].first->name_);
            ASSERT_EQUAL(9900, stops_distance[0].second);
            ASSERT_EQUAL("Marushkino", stops_distance[1].first->name_);
            ASSERT_EQUAL(100, stops_distance[1].second);
        }
        std::cout << "ParseStopsDistance: PASSED" << std::endl;
    }

    void ParseStandartRoute()
    {
        TransportCatalogue catalogue;
        catalogue.AddStop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});
        catalogue.AddStop({"Biryusinka", {55.581065, 37.64839}});
        catalogue.AddStop({"Universam", {55.587655, 37.645687}});
        catalogue.AddStop({"Biryulyovo Tovarnaya", {55.592028, 37.653656}});
        catalogue.AddStop({"Biryulyovo Passazhirskaya", {55.580999, 37.659164}});
        catalogue.AddStop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});

        catalogue.AddStop({"Biryulyovo   Zapadnoye", {55.574371, 37.6517}});
        catalogue.AddStop({"Biryulyovo      Tovarnaya", {55.574371, 37.6517}});
        catalogue.AddStop({"Biryulyovo       Passazhirskaya", {55.574371, 37.6517}});

        {
            input::Reader i_reader(&catalogue);
            std::string   request(
              "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseStandartRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request(
              " Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseStandartRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request(
              "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseStandartRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request(
              "   Biryulyovo Zapadnoye    >   Biryusinka   >  Universam  >    Biryulyovo Tovarnaya    >   Biryulyovo Passazhirskaya   >  Biryulyovo Zapadnoye "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseStandartRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[5]->name_);
        }
        {
            input::Reader i_reader(&catalogue);
            std::string   request(
              "   Biryulyovo   Zapadnoye    >   Biryusinka   >  Universam  >    Biryulyovo      Tovarnaya    >   Biryulyovo       Passazhirskaya   >  Biryulyovo   Zapadnoye "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseStandartRoute(request);
            ASSERT_EQUAL("Biryulyovo   Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
            ASSERT_EQUAL("Biryulyovo      Tovarnaya", parsed_data[3]->name_);
            ASSERT_EQUAL("Biryulyovo       Passazhirskaya", parsed_data[4]->name_);
            ASSERT_EQUAL("Biryulyovo   Zapadnoye", parsed_data[5]->name_);
        }
        std::cout << "ParseStandartRoute: PASSED" << std::endl;
    }

    void ParseRingRoute()
    {
        TransportCatalogue catalogue;
        catalogue.AddStop({"Biryulyovo Zapadnoye", {55.574371, 37.6517}});
        catalogue.AddStop({"Biryusinka", {55.581065, 37.64839}});
        catalogue.AddStop({"Universam", {55.587655, 37.645687}});

        catalogue.AddStop({"Biryulyovo     Zapadnoye", {55.587655, 37.645687}});

        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request("Biryulyovo Zapadnoye - Biryusinka - Universam"s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
        }
        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request(" Biryulyovo Zapadnoye - Biryusinka - Universam"s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
        }
        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request("Biryulyovo Zapadnoye - Biryusinka - Universam "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
        }
        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request("   Biryulyovo Zapadnoye    -   Biryusinka   -  Universam "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
        }
        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request("   Biryulyovo     Zapadnoye    -   Biryusinka   -  Universam "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo     Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
            ASSERT_EQUAL("Universam", parsed_data[2]->name_);
        }
        {
            input::Reader                    i_reader(&catalogue);
            std::string                      request("   Biryulyovo     Zapadnoye    -   Biryusinka    "s);
            std::vector<const domain::Stop*> parsed_data = i_reader.ParseRingRoute(request);
            ASSERT_EQUAL("Biryulyovo     Zapadnoye", parsed_data[0]->name_);
            ASSERT_EQUAL("Biryusinka", parsed_data[1]->name_);
        }
        std::cout << "ParseRingRoute: PASSED" << std::endl;
    }

    void all_input_tests()
    {
        ParseRequestType();
        ParseName();
        ParseCoords();
        ParseStopsDistance();
        ParseStandartRoute();
        ParseRingRoute();
    }
}    // namespace transport_catalogue::tests