#pragma once
#include "geo.h"
#include <unordered_map>
#include <string>
#include <deque>
#include <vector>
#include <string_view>
#include <optional>
#include <set>

namespace transport_catalogue_app::core {

using transport_catalogue_app::detail::Coordinates;

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Route {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_cyclic;
};

struct StopPairHasher {
    size_t operator()(const std::pair<const Stop*, const Stop*>& pair) const {
        return std::hash<const void*>()(pair.first) ^ std::hash<const void*>()(pair.second);
    }
};


class TransportCatalogue {
public:
    struct RouteStats {
        int total_stops;
        int unique_stops;
        double route_length;
        double curvature; // Добавляем извилистость
    };
    
    void AddStop(const std::string& name, Coordinates coords);
    void AddRoute(const std::string& name, const std::vector<std::string_view>& stops, bool is_cyclic);

    const Route* GetRouteInfo(std::string_view name) const;
    const std::set<std::string_view>* GetBusesForStop(std::string_view stop_name) const;
    const Stop* GetStopInfo(std::string_view name) const;
    RouteStats GetRouteStatistics(std::string_view name) const;
    void SetDistance(const Stop* from, const Stop* to, int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
    double CalculateRouteDistance(const Route* route) const;

private:
    std::deque<Stop> stops_;
    std::deque<Route> routes_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, const Route*> routename_to_route_;
    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_buses_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> stop_distances_;
};

} // namespace transport_catalogue_app::core