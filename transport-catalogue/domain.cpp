#include "domain.h"

namespace domain
{
    using namespace geo;

    Stop::Stop(const std::string& name, const Coordinates& coordinates) : name_(name), coordinates_(coordinates) {}
    Bus::Bus(const std::string& name, const std::vector<const Stop*>& stops) : name_(name), stops_(stops) {}
}    // namespace domain