#include "stat_reader.h"

#include "detail.h"

#include <iomanip>

namespace output
{
    using namespace transport_catalogue;
    using namespace detail;

    stat_reader::stat_reader(TransportCatalogue* catalogue) : buses_info_(), request_data_(), catalogue_(catalogue) {}

    void stat_reader::parse_transport_catalogue()
    {
        for(const auto& data : request_data_)
        {
            const auto request_info = data;

            if(request_info.type_ == stat_reader::query_type::BUS)
            {
                const auto bus_info = catalogue_->get_bus_info(request_info.request_);
                buses_info_.push(bus_info);
            }
            if(request_info.type_ == stat_reader::query_type::STOP)
            {
                const auto bus_info = catalogue_->get_stop_info(request_info.request_);
                stops_info_.push(bus_info);
            }
        }
    }

    [[nodiscard]] std::string stat_reader::parse_request_type(std::string& request)
    {
        std::string request_type = request.substr(0, request.find_first_of(' '));
        trim(request.erase(0, request.find_first_of(' ')));
        return request_type;
    }

    std::string stat_reader::parse_name(const std::string& request)
    {
        return request;
    }

    std::ostream& operator<<(std::ostream& stream, stat_reader& s_reader)
    {
        while(!s_reader.request_data_.empty())
        {
            const auto& request_info = s_reader.request_data_.front();

            if(request_info.type_ == stat_reader::query_type::BUS)
            {
                const auto& bus_info = s_reader.buses_info_.front();

                if(bus_info.stops_count_)
                {
                    stream << "Bus " << bus_info.name_ << ": " << bus_info.stops_count_ << " stops on route, "
                           << bus_info.uniq_stops_count_ << " unique stops, " << bus_info.bus_length_ << " route length"
                           << ", " << std::setprecision(6) << bus_info.curvature_ << " curvature" << std::endl;
                }
                else
                {
                    stream << "Bus " << bus_info.name_ << ": "
                           << "not found" << std::endl;
                }
                s_reader.buses_info_.pop();
            }
            if(request_info.type_ == stat_reader::query_type::STOP)
            {
                const auto& stop_info = s_reader.stops_info_.front();

                if(!stop_info.not_found_)
                {
                    if(stop_info.buses_name_.size())
                    {
                        stream << "Stop " << stop_info.name_ << ": "
                               << "buses ";

                        for(auto& bus : stop_info.buses_name_)
                        {
                            stream << bus << " ";
                        }
                        stream << std::endl;
                    }
                    else
                    {
                        stream << "Stop " << stop_info.name_ << ": "
                               << "no buses" << std::endl;
                    }
                }
                else
                {
                    stream << "Stop " << stop_info.name_ << ": "
                           << "not found" << std::endl;
                }
                s_reader.stops_info_.pop();
            }
            s_reader.request_data_.pop_front();
        }
        return stream;
    }

    std::istream& operator>>(std::istream& stream, stat_reader& s_reader)
    {
        size_t request_count = 0;
        stream >> request_count;
        stream.ignore();

        for(size_t i = 0; i < request_count; ++i)
        {
            std::string request;
            std::getline(stream, request);

            std::string request_type = s_reader.parse_request_type(request);

            if(request_type == "Bus")
            {
                std::string name = s_reader.parse_name(request);
                s_reader.request_data_.push_back({stat_reader::query_type::BUS, name});
            }
            if(request_type == "Stop")
            {
                std::string name = s_reader.parse_name(request);
                s_reader.request_data_.push_back({stat_reader::query_type::STOP, name});
            }
        }
        return stream;
    }
}    // namespace output