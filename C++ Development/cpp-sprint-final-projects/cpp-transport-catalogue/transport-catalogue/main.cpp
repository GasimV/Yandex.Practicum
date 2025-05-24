#include "json_builder.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "svg.h"

#include <iostream>
#include <string>
#include <set>
#include <vector>

using namespace std;

int main() {
    using namespace transport_catalogue_app;

    core::TransportCatalogue catalogue;

    // Настройки маршрутизации по умолчанию (из domain)
    domain::RoutingSettings routing_settings;
    routing_settings.bus_wait_time = 6;
    routing_settings.bus_velocity  = 40;

    // Считываем JSON из stdin
    json::Document doc = json::Load(std::cin);
    const auto& root = doc.GetRoot().AsDict();

    // Если есть routing_settings, переопределим
    if (root.find("routing_settings") != root.end()) {
        const auto& rs_node = root.at("routing_settings").AsDict();
        routing_settings.bus_wait_time = rs_node.at("bus_wait_time").AsInt();
        routing_settings.bus_velocity  = rs_node.at("bus_velocity").AsDouble();
    }

    // Создаём JsonReader, передаём ему каталог и настройки
    io::JsonReader json_reader(catalogue, routing_settings);

    // Обрабатываем base_requests
    if (root.find("base_requests") != root.end()) {
        const auto& base_requests = root.at("base_requests").AsArray();
        json_reader.ProcessBaseRequests(base_requests);
    }

    // Теперь строим TransportRouter (через RequestHandler) после заполнения каталога
    json_reader.CreateRouterAfterBase();

    // Считываем настройки рендера
    map_renderer::RenderSettings render_settings;
    if (root.find("render_settings") != root.end()) {
        render_settings = json_reader.ParseRenderSettings(root.at("render_settings"));
    } else {
        render_settings = map_renderer::RenderSettings{ 
            600.0,
            400.0,
            50.0,
            14.0,
            5.0,
            20,
            {7.0, 15.0},
            20,
            {7.0, -3.0},
            svg::Rgba{255, 255, 255, 0.85},
            3.0,
            { svg::Color("green"), svg::Color(svg::Rgb{255, 160, 0}), svg::Color("red") }
        };
    }

    // Фильтруем остановки для рендера
    std::set<std::string> used_stop_names;
    for (const auto& route_pair : catalogue.GetAllRoutes()) {
        for (const auto* stop_ptr : route_pair.second->stops) {
            used_stop_names.insert(stop_ptr->name);
        }
    }

    // Создаём облегчённый каталог для рендера
    core::TransportCatalogue catalogue_for_map;
    for (const auto& stop : catalogue.GetAllStops()) {
        if (used_stop_names.count(stop.name)) {
            catalogue_for_map.AddStop(stop.name, stop.coordinates);
        }
    }
    for (const auto& route_pair : catalogue.GetAllRoutes()) {
        const auto* route_ptr = route_pair.second;
        std::vector<std::string_view> stop_names;
        for (auto* st : route_ptr->stops) {
            stop_names.push_back(st->name);
        }
        catalogue_for_map.AddRoute(std::string(route_pair.first), stop_names, route_ptr->is_cyclic);
    }

    // Обрабатываем stat_requests
    json::Array responses;
    if (root.find("stat_requests") != root.end()) {
        const auto& stat_requests = root.at("stat_requests").AsArray();
        map_renderer::MapRenderer renderer(render_settings, catalogue_for_map);
        responses = json_reader.ProcessStatRequests(stat_requests, renderer);
    }

    // Выводим результат в stdout
    json::Print(json::Document{json::Node{responses}}, std::cout);

    return 0;
}