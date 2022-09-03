#pragma once

#include "router.h"
#include "graph.h"
#include "transport_catalogue.h"

namespace transport_router {


    class TransportRouter {

    public:

        explicit TransportRouter() = default;

        explicit TransportRouter(int wait_time, double velocity)
            : wait_time_(wait_time)
            , velocity_(velocity) {
        }

        void SetWaitTime(int wait_time);
        void SetVelocity(double velocity);

        int GetWaitTime() const;
        double GetVelocity() const;
        double FormulaForFindTime(double distance, double weight_time, double velocity);
        void ConstructGraph(graph::DirectedWeightedGraph<double>& graph, double distance, double& weight, double velocity, size_t a, size_t b,
            size_t& span, const std::vector<domain::Stop*>& stops, std::string bus_num);
        void FillCatalogueGraph(transport_db::TransportCatalogue& t_c, graph::DirectedWeightedGraph<double>& graph);

    private:
        int wait_time_ = 0;
        double velocity_ = 0.;
        graph::Router<double>* router_ = nullptr;
    };
}
