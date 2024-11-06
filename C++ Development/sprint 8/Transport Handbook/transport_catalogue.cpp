#include "transport_catalogue.h"
#include <unordered_set>
#include <cmath>
#include <iostream>  // For debugging (optional)

void TransportCatalogue::AddStop(const std::string& name, Coordinates coords) {
    stops_.emplace_back(Stop{name, coords});
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddRoute(const std::string& name, const std::vector<std::string_view>& stop_names, bool is_cyclic) {
    Route route{name, {}, is_cyclic};
    
    for (const auto& stop_name : stop_names) {
        if (stopname_to_stop_.count(stop_name) == 0) {
            std::cerr << "Warning: Stop " << stop_name << " not found when adding route " << name << std::endl;
            continue; // Skip undefined stops or handle as needed
        }
        route.stops.push_back(stopname_to_stop_.at(stop_name));
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