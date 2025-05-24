#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include "domain.h"  // теперь используем доменные типы
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <string>

namespace transport_catalogue_app::core {

class TransportRouter {
public:
    // Конструктор строит маршрутизирующий граф по данным каталога и настройкам
    TransportRouter(const TransportCatalogue& catalogue, int bus_wait_time, double bus_velocity);
    
    // Построение маршрута между остановками
    std::optional<transport_catalogue_app::domain::RouteResult> BuildRoute(const Stop* from, const Stop* to) const;

private:
    void BuildGraph();
    void AddWaitEdges();
    void AddBusEdges();

    const TransportCatalogue& catalogue_;
    int bus_wait_time_;
    double bus_velocity_;

    // Граф и данные для маршрутизации
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::vector<transport_catalogue_app::domain::EdgeInfo> edge_infos_;
    std::unordered_map<const core::Stop*, int> stop_to_index_;
};

} // namespace transport_catalogue_app::core