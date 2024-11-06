#include "transport_catalogue.h"
#include <unordered_set>
#include <cmath>
#include <algorithm>

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

    routes_.push_back(std::move(route));
    routename_to_route_[routes_.back().name] = &routes_.back();
}

std::optional<Route> TransportCatalogue::GetRouteInfo(const std::string& name) const {
    if (routename_to_route_.count(name) > 0) {
        return *routename_to_route_.at(name);
    }
    return std::nullopt;
}

std::optional<std::set<std::string>> TransportCatalogue::GetBusesForStop(const std::string& stop_name) const {
    if (stopname_to_stop_.count(stop_name) == 0) {
        return std::nullopt; // Stop not found
    }

    const Stop* stop = stopname_to_stop_.at(stop_name);
    if (stop_to_buses_.count(stop) == 0) {
        return std::set<std::string>(); // Stop exists but no buses
    }

    return stop_to_buses_.at(stop); // Return buses passing through the stop
}