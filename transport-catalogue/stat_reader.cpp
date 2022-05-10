#include "stat_reader.h"

#include "detail.h"

namespace output
{
    using namespace transport_catalogue;
    using namespace detail;

    Reader::Reader(TransportCatalogue* catalogue) :
        request_data_(),
        catalogue_(catalogue)
    {
    }

    std::string Reader::PrintBus(const transport_catalogue::BusInfo& bus)
    {
        std::stringstream stream;
        if(bus.stops_count_)
        {
            stream << "Bus " << bus.name_ << ": " << bus.stops_count_ << " stops on route, "
                   << bus.uniq_stops_count_ << " unique stops, " << bus.bus_length_ << " route length"
                   << ", " << std::setprecision(6) << bus.curvature_ << " curvature" << std::endl;
        }
        else
        {
            stream << "Bus " << bus.name_ << ": "
                   << "not found" << std::endl;
        }
        return stream.str();
    }

    std::string Reader::PrintStop(const transport_catalogue::StopInfo& stop)
    {
        std::stringstream stream;
        if(!stop.not_found_)
        {
            if(stop.buses_name_.size())
            {
                stream << "Stop " << stop.name_ << ": "
                       << "buses ";

                for(auto& bus : stop.buses_name_)
                {
                    stream << bus << " ";
                }
                stream << std::endl;
            }
            else
            {
                stream << "Stop " << stop.name_ << ": "
                       << "no buses" << std::endl;
            }
        }
        else
        {
            stream << "Stop " << stop.name_ << ": "
                   << "not found" << std::endl;
        }
        return stream.str();
    }

    [[nodiscard]] std::string Reader::ParseRequestType(std::string& request)
    {
        std::string request_type = request.substr(0, request.find_first_of(' '));
        trim(request.erase(0, request.find_first_of(' ')));
        return request_type;
    }
}    // namespace output