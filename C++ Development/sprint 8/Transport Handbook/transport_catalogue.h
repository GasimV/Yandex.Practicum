#pragma once

#include "geo.h"
#include <unordered_map>
#include <string>
#include <deque>
#include <vector>
#include <string_view>
#include <optional>
#include <set>

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Route {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_cyclic;
};

class TransportCatalogue {
public:
    void AddStop(const std::string& name, Coordinates coords);
    void AddRoute(const std::string& name, const std::vector<std::string_view>& stops, bool is_cyclic);
    std::optional<Route> GetRouteInfo(const std::string& name) const;
    std::optional<std::set<std::string>> GetBusesForStop(const std::string& stop_name) const;

private:
    std::deque<Stop> stops_;
    std::deque<Route> routes_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, const Route*> routename_to_route_;
    std::unordered_map<const Stop*, std::set<std::string>> stop_to_buses_; // New index for stop-to-buses
};