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
        if (stopname_to_stop_.count(stop_name) == 0) {
            continue; // Skip undefined stops
        }
        const Stop* stop = stopname_to_stop_.at(stop_name);
        route.stops.push_back(stop);
        stop_to_buses_[stop].insert(name); // Update the stop-to-buses index
    }

    routes_.emplace_back(std::move(route));
    routename_to_route_[routes_.back().name] = &routes_.back();
}

const Route* TransportCatalogue::GetRouteInfo(std::string_view name) const {
    if (routename_to_route_.count(name) > 0) {
        return routename_to_route_.at(name);
    }
    return nullptr;
}

const std::set<std::string_view>* TransportCatalogue::GetBusesForStop(std::string_view stop_name) const {
    if (stopname_to_stop_.count(stop_name) == 0) {
        return nullptr; // Stop not found
    }

    const Stop* stop = stopname_to_stop_.at(stop_name);
    if (stop_to_buses_.count(stop) == 0) {
        static const std::set<std::string_view> empty_set; // Return empty set for existing stops with no buses
        return &empty_set;
    }

    return &stop_to_buses_.at(stop);
}

const Stop* TransportCatalogue::GetStopInfo(std::string_view name) const {
    auto it = stopname_to_stop_.find(name);
    return it != stopname_to_stop_.end() ? it->second : nullptr;
}

double TransportCatalogue::CalculateRouteDistance(const Route* route) const {
    double total_distance = 0;
    for (size_t i = 0; i + 1 < route->stops.size(); ++i) {
        const Stop* from = route->stops[i];
        const Stop* to = route->stops[i + 1];
        total_distance += GetDistance(from, to);
    }
    return total_distance;
}

TransportCatalogue::RouteStats TransportCatalogue::GetRouteStatistics(std::string_view name) const {
    const auto* route = GetRouteInfo(name);
    if (!route) {
        return {0, 0, 0.0, 0.0};
    }

    std::unordered_set<std::string_view> unique_stops;
    double geo_distance = 0.0;
    double actual_distance = CalculateRouteDistance(route);

    for (size_t i = 0; i + 1 < route->stops.size(); ++i) {
        const Stop* from = route->stops[i];
        const Stop* to = route->stops[i + 1];
        geo_distance += ComputeDistance(from->coordinates, to->coordinates);
        unique_stops.insert(from->name);
    }
    unique_stops.insert(route->stops.back()->name);

    double curvature = (geo_distance > 0) ? actual_distance / geo_distance : 1.0;

    return {
        static_cast<int>(route->stops.size()),
        static_cast<int>(unique_stops.size()),
        actual_distance,
        curvature
    };
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

// Implementation of GetAllRoutes
const std::unordered_map<std::string_view, const Route*>& TransportCatalogue::GetAllRoutes() const {
    return routename_to_route_;
}

} // namespace transport_catalogue_app::core