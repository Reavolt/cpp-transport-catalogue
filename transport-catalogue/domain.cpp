#include "domain.h"

namespace domain
{
    Stop::Stop(const std::string& name, const geo::Coordinates& coordinates) : name_(name), coordinates_(coordinates) {}

    Bus::Bus(const std::string& name, const std::vector<const Stop*>& stops, RouteType route_type_) :
        name_(name),
        stops_(stops),
        route_type_(route_type_)
    {
    }
}    // namespace domain
