#include "input_reader.h"
#include "input_tests.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>

//1 - DEQUE -> Для остановок. deque<Stops>
//2 - Второй контейнер для остановок по которому будет происходить поиск unordered_map<string_view, Stops*>
//3 - Маршрут в векторе std::vector<Stops*> A -> B -> C -> C -> D -> B -> A
//4 - Маршруты(набор маршрутов состоит из остановок) тоже в deque<Bus> buses
//5 - Поиск маршрутов через хеш таблицы unordered_map<string_view, Bus*> // Маршруты и остановки хранятся аналогично.
//6 - Расстояния хранятся в unordered_map<pair<stop1, stop2>, distance, hesher(длжен считать хеш от 2х указателей)>.
//7 - На вход методам лучше всего передавать готовые структуры распаршенные из запросов.

int main()
{
    transport_catalogue::tests::all_input_tests();

    transport_catalogue::TransportCatalogue catalogue;

    input::input_reader i_reader(&catalogue);
    std::cin >> i_reader;
    i_reader.fill_transport_catalogue();

    output::stat_reader s_reader(&catalogue);
    std::cin >> s_reader;
    s_reader.parse_transport_catalogue();
    std::cout << s_reader;

    return 0;
}