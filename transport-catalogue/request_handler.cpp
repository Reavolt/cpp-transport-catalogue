// #include "request_handler.h"

// RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer) :
//     catalogue_(catalogue_),
//     renderer_(renderer)
// {
// }

// std::optional<const domain::Bus*> RequestHandler::GetBusStat(const std::string_view& bus_name) const
// {
//     const domain::Bus* bus = catalogue_.FindBus(bus_name);
//     if(bus)
//     {
//         return bus;
//     }
//     return std::nullopt;
// }

// const std::unordered_set<domain::Bus>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const
// {
//     // const domain::Stop* stop = db_.FindStop(stop_name);
//     // if(!stop->.empty())
//     // {
//     //     return &(stop->buses);
//     // }
//     // else
//     // {
//     //     return nullptr;
//     // }
// }