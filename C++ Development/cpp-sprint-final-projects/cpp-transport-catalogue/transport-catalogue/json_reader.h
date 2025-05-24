#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "domain.h"
#include <memory>

namespace transport_catalogue_app::io {

class JsonReader {
public:
    JsonReader(transport_catalogue_app::core::TransportCatalogue& catalogue,
               const transport_catalogue_app::domain::RoutingSettings& routing_settings);

    // Заполняем каталог данными из base_requests
    void ProcessBaseRequests(const json::Array& base_requests);

    // Создаём маршрутизатор (через RequestHandler) — делаем это после заполнения каталога
    void CreateRouterAfterBase();

    // Обрабатываем stat_requests, возвращая массив JSON-ответов
    json::Array ProcessStatRequests(const json::Array& stat_requests, const map_renderer::MapRenderer& renderer);

    // Читаем настройки рендера
    map_renderer::RenderSettings ParseRenderSettings(const json::Node& render_settings_node) const;

private:
    transport_catalogue_app::core::TransportCatalogue& catalogue_;
    transport_catalogue_app::domain::RoutingSettings routing_settings_;

    // Храним не сам объект, а unique_ptr, чтобы можно было пересоздать его
    std::unique_ptr<transport_catalogue_app::core::RequestHandler> request_handler_;

    // Вспомогательные методы
    void AddStops(const json::Array& base_requests);
    void AddDistances(const json::Array& base_requests);
    void AddBusRoutes(const json::Array& base_requests);

    svg::Color ParseColor(const json::Node& color_node) const;
};

} // namespace transport_catalogue_app::io