// #pragma once

// #include "domain.h"
// #include "map_renderer.h"
// #include "transport_catalogue.h"

// class RequestHandler
// {
// public:
//     // MapRenderer понадобится в следующей части итогового проекта
//     RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

//     // Возвращает информацию о маршруте (запрос Bus)
//     std::optional<const domain::Bus*> GetBusStat(const std::string_view& bus_name) const;

//     // Возвращает маршруты, проходящие через
//     const std::unordered_set<domain::Bus>* GetBusesByStop(const std::string_view& stop_name) const;

//     // Этот метод будет нужен в следующей части итогового проекта
//     svg::Document RenderMap() const;

// private:
//     // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
//     const transport_catalogue::TransportCatalogue& catalogue_;
//     const renderer::MapRenderer&                   renderer_;
// };