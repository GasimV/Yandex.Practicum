#include "transport_catalogue.h"
#include <unordered_set>
#include <cmath>
#include <algorithm>

namespace transport_catalogue_app::core {

void TransportCatalogue::AddStop(const std::string& name, Coordinates coords) {
    stops_.emplace_back(Stop{name, coords});
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddRoute(const std::string& name, const std::vector<std::string_view>& stop_names, bool is_cyclic) {
    Route route{name, {}, is_cyclic};
     
    for (const auto& stop_name : stop_names) {
        auto it = stopname_to_stop_.find(stop_name);
        if (it == stopname_to_stop_.end()) {
            continue; // Пропускаем несуществующие остановки
        }
        const Stop* stop = it->second;
        route.stops.push_back(stop);
        stop_to_buses_[stop].insert(name);
    }
    routes_.emplace_back(std::move(route));
    routename_to_route_[routes_.back().name] = &routes_.back();
}

const Route* TransportCatalogue::GetRouteInfo(std::string_view name) const {
    auto it = routename_to_route_.find(name);
    return it != routename_to_route_.end() ? it->second : nullptr;
}

const std::set<std::string_view>* TransportCatalogue::GetBusesForStop(std::string_view stop_name) const {
    auto it = stopname_to_stop_.find(stop_name);
    if (it == stopname_to_stop_.end())
        return nullptr;
    const Stop* stop = it->second;
    auto it2 = stop_to_buses_.find(stop);
    if (it2 == stop_to_buses_.end())
        return nullptr;
    return &(it2->second);
}

const Stop* TransportCatalogue::GetStopInfo(std::string_view name) const {
    auto it = stopname_to_stop_.find(name);
    return it != stopname_to_stop_.end() ? it->second : nullptr;
}

double TransportCatalogue::CalculateRouteDistance(const Route* route) const {
    double total_distance = 0.0;
    size_t stop_count = route->stops.size();
    for (size_t i = 0; i + 1 < stop_count; ++i) {
        const Stop* from = route->stops[i];
        const Stop* to = route->stops[i + 1];
        total_distance += static_cast<double>(GetDistance(from, to));
    }
    if (!route->is_cyclic && stop_count > 1) {
        for (size_t i = stop_count - 1; i > 0; --i) {
            const Stop* from = route->stops[i];
            const Stop* to = route->stops[i - 1];
            total_distance += static_cast<double>(GetDistance(from, to));
        }
    }
    return total_distance;
}

TransportCatalogue::RouteStats TransportCatalogue::GetRouteStatistics(std::string_view name) const {
    const auto* route = GetRouteInfo(name);
    if (!route) {
        return {0, 0, 0.0, 0.0};
    }
    std::vector<const Stop*> stop_sequence = route->stops;
    if (!route->is_cyclic && stop_sequence.size() > 1) {
        stop_sequence.insert(stop_sequence.end(), route->stops.rbegin() + 1, route->stops.rend());
    }
    int total_stops = static_cast<int>(stop_sequence.size());
    std::unordered_set<std::string_view> unique_stops;
    double geo_distance = 0.0;
    double actual_distance = 0.0;
    for (size_t i = 0; i + 1 < stop_sequence.size(); ++i) {
        const Stop* from = stop_sequence[i];
        const Stop* to = stop_sequence[i + 1];
        geo_distance += transport_catalogue_app::detail::ComputeDistance(from->coordinates, to->coordinates);
        actual_distance += GetDistance(from, to);
        unique_stops.insert(from->name);
    }
    if (!stop_sequence.empty()) {
        unique_stops.insert(stop_sequence.back()->name);
    }
    double curvature = (geo_distance > 0) ? (actual_distance / geo_distance) : 1.0;
    return {total_stops, static_cast<int>(unique_stops.size()), actual_distance, curvature};
}

void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
    stop_distances_[{from, to}] = distance;
}

int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    auto it = stop_distances_.find({from, to});
    if (it != stop_distances_.end()) {
        return it->second;
    }
    it = stop_distances_.find({to, from});
    if (it != stop_distances_.end()) {
        return it->second;
    }
    return 0;
}

const std::deque<Stop>& TransportCatalogue::GetAllStops() const {
    return stops_;
}

const std::unordered_map<std::string_view, const Route*>& TransportCatalogue::GetAllRoutes() const {
    return routename_to_route_;
}

transport_catalogue_app::domain::StopInfoResult TransportCatalogue::GetStopInfoResult(std::string_view stop_name) const {
    transport_catalogue_app::domain::StopInfoResult result;
    const Stop* stop = GetStopInfo(stop_name);
    if (!stop) {
        result.found = false;
        return result;
    }
    result.found = true;
    if (const auto* buses = GetBusesForStop(stop_name)) {
        result.buses.insert(result.buses.end(), buses->begin(), buses->end());
    }
    return result;
}

transport_catalogue_app::domain::BusInfoResult TransportCatalogue::GetBusInfoResult(std::string_view bus_name) const {
    transport_catalogue_app::domain::BusInfoResult result;
    auto stats = GetRouteStatistics(bus_name);
    if (stats.total_stops == 0) {
        result.found = false;
        return result;
    }
    result.found = true;
    result.curvature = stats.curvature;
    result.route_length = stats.route_length;
    result.stop_count = stats.total_stops;
    result.unique_stop_count = stats.unique_stops;
    return result;
}

} // namespace transport_catalogue_app::core