#include "transport_router.h"
#include <vector>
#include <cassert>
#include <optional>

namespace transport_catalogue_app::core {

TransportRouter::TransportRouter(const TransportCatalogue& catalogue, int bus_wait_time, double bus_velocity)
    : catalogue_(catalogue)
    , bus_wait_time_(bus_wait_time)
    , bus_velocity_(bus_velocity)
{
    BuildGraph();
    router_ = std::make_unique<graph::Router<double>>(graph_);
}

void TransportRouter::BuildGraph() {
    const auto& stops = catalogue_.GetAllStops();
    int stop_count = static_cast<int>(stops.size());
    int vertex_count = stop_count * 2; // две вершины на остановку: "ожидание" и "после ожидания"
    graph_ = graph::DirectedWeightedGraph<double>(vertex_count);

    int index = 0;
    for (const auto& stop : stops) {
        // Ключ – адрес остановки
        stop_to_index_[&stop] = index++;
    }

    AddWaitEdges();
    AddBusEdges();
}

void TransportRouter::AddWaitEdges() {
    for (const auto& [stop_ptr, idx] : stop_to_index_) {
        int wait_vertex = idx * 2;
        int ride_vertex = idx * 2 + 1;
        transport_catalogue_app::domain::EdgeInfo info;
        info.type = transport_catalogue_app::domain::EdgeType::WAIT;
        info.stop_name = stop_ptr->name;
        info.time = static_cast<double>(bus_wait_time_);

        graph::Edge<double> edge{
            static_cast<graph::VertexId>(wait_vertex),
            static_cast<graph::VertexId>(ride_vertex),
            static_cast<double>(bus_wait_time_)
        };
        graph_.AddEdge(edge);
        edge_infos_.push_back(info);
    }
}

void TransportRouter::AddBusEdges() {
    const auto& routes = catalogue_.GetAllRoutes();
    for (const auto& [route_name, route_ptr] : routes) {
        const auto& route_stops = route_ptr->stops;
        if (route_stops.empty())
            continue;

        std::vector<const Stop*> stops_seq(route_stops.begin(), route_stops.end());
        if (!route_ptr->is_cyclic && route_stops.size() > 1) {
            for (auto it = route_stops.rbegin() + 1; it != route_stops.rend(); ++it) {
                stops_seq.push_back(*it);
            }
        }

        for (size_t i = 0; i < stops_seq.size(); ++i) {
            double cumulative_distance = 0.0;
            for (size_t j = i + 1; j < stops_seq.size(); ++j) {
                if (route_ptr->is_cyclic && i > 0 && stops_seq[j] == stops_seq[i])
                    break;

                int d = catalogue_.GetDistance(stops_seq[j - 1], stops_seq[j]);
                cumulative_distance += d;
                double travel_time = (cumulative_distance / 1000.0) / bus_velocity_ * 60.0;

                int from_idx = stop_to_index_.at(stops_seq[i]);
                int to_idx = stop_to_index_.at(stops_seq[j]);
                int from_vertex = from_idx * 2 + 1; // после ожидания
                int to_vertex = to_idx * 2;         // ожидание

                transport_catalogue_app::domain::EdgeInfo info;
                info.type = transport_catalogue_app::domain::EdgeType::BUS;
                info.bus = route_name;
                info.span_count = static_cast<int>(j - i);
                info.time = travel_time;

                graph::Edge<double> edge{
                    static_cast<graph::VertexId>(from_vertex),
                    static_cast<graph::VertexId>(to_vertex),
                    travel_time
                };
                graph_.AddEdge(edge);
                edge_infos_.push_back(info);
            }
        }
    }
}

std::optional<transport_catalogue_app::domain::RouteResult> TransportRouter::BuildRoute(const Stop* from, const Stop* to) const {
    auto it_from = stop_to_index_.find(from);
    auto it_to = stop_to_index_.find(to);
    if (it_from == stop_to_index_.end() || it_to == stop_to_index_.end()) {
        return std::nullopt;
    }
    int start_vertex = it_from->second * 2;  // состояние "ожидание"
    int finish_vertex = it_to->second * 2;     // состояние "ожидание"

    auto graph_route_opt = router_->BuildRoute(start_vertex, finish_vertex);
    if (!graph_route_opt) {
        return std::nullopt;
    }
    const auto& graph_route = *graph_route_opt;
    transport_catalogue_app::domain::RouteResult result;
    result.total_time = graph_route.weight;
    for (auto edge_id : graph_route.edges) {
        result.items.push_back(edge_infos_[edge_id]);
    }
    return result;
}

} // namespace transport_catalogue_app::core