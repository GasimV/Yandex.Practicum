#pragma once 
#include "geo.h" 
#include <unordered_map> 
#include <string> 
#include <deque> 
#include <vector> 
#include <string_view> 
#include <optional> 
#include <set>
#include "domain.h"

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
        double curvature;
    };
     
    void AddStop(const std::string& name, Coordinates coords);
    void AddRoute(const std::string& name, const std::vector<std::string_view>& stops, bool is_cyclic);
     
    const Route* GetRouteInfo(std::string_view name) const;
    const std::set<std::string_view>* GetBusesForStop(std::string_view stop_name) const;
    const Stop* GetStopInfo(std::string_view name) const;
    RouteStats GetRouteStatistics(std::string_view name) const;
    void SetDistance(const Stop* from, const Stop* to, int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
     
    // Доступ ко всем остановкам/маршрутам без копирования
    const std::deque<Stop>& GetAllStops() const;
    const std::unordered_map<std::string_view, const Route*>& GetAllRoutes() const;
     
    // Методы для RequestHandler
    transport_catalogue_app::domain::StopInfoResult GetStopInfoResult(std::string_view stop_name) const;
    transport_catalogue_app::domain::BusInfoResult GetBusInfoResult(std::string_view bus_name) const;
     
private:
    std::deque<Stop> stops_;
    std::deque<Route> routes_;
    std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, const Route*> routename_to_route_;
    std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_buses_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> stop_distances_;
     
    double CalculateRouteDistance(const Route* route) const;
};

} // namespace transport_catalogue_app::core