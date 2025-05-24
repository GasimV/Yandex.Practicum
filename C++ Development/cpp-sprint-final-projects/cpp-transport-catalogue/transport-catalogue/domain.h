#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace transport_catalogue_app::domain {

// Настройки маршрутизации (доступны во всем приложении)
struct RoutingSettings {
    int bus_wait_time = 0;
    double bus_velocity = 0.0;
};

// Результат для информации об остановке
struct StopInfoResult {
    bool found = false;
    std::vector<std::string_view> buses;
};

// Результат для информации о маршруте автобуса
struct BusInfoResult {
    bool found = false;
    double curvature = 0.0;
    double route_length = 0.0;
    int stop_count = 0;
    int unique_stop_count = 0;
};

// Тип шага маршрута
enum class EdgeType {
    WAIT,
    BUS
};

// Информация об одном шаге маршрута (ожидание или переезд)
struct EdgeInfo {
    EdgeType type;
    std::string stop_name; // для WAIT
    std::string bus;       // для BUS
    int span_count = 0;    // число остановок на автобусе
    double time = 0.0;     // время шага
};

// Результат для информации о построенном маршруте
struct RouteResult {
    bool found = false;
    double total_time = 0.0;
    std::vector<EdgeInfo> items;
};

} // namespace transport_catalogue_app::domain
