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

    size_t StopPtrHasher::operator()(const std::pair<const domain::Stop*, const domain::Stop*>& p) const
    {
        return pointer_hasher(p.first) * SIMPLE_NUMBER + pointer_hasher(p.second) * SIMPLE_NUMBER * SIMPLE_NUMBER;
    }

    size_t StopPtrHasher::operator()(const domain::Stop* p) const
    {
        return pointer_hasher(p) * SIMPLE_NUMBER;
    }
}    // namespace domain