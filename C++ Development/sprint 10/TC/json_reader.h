#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h" // Include map_renderer to access RenderSettings

namespace transport_catalogue_app::io {

class JsonReader {
public:
    JsonReader(transport_catalogue_app::core::TransportCatalogue& catalogue);

    // Reads and processes base_requests to fill the catalogue.
    void ProcessBaseRequests(const json::Array& base_requests);

    // Reads and processes stat_requests, returning a JSON array of responses.
    json::Array ProcessStatRequests(const json::Array& stat_requests, const map_renderer::MapRenderer& renderer);

    // Parses render_settings from JSON and returns a RenderSettings structure
    map_renderer::RenderSettings ParseRenderSettings(const json::Node& render_settings_node) const;

private:
    transport_catalogue_app::core::TransportCatalogue& catalogue_;
    transport_catalogue_app::core::RequestHandler request_handler_;

    // Вспомогательные методы
    void AddStops(const json::Array& base_requests);
    void AddDistances(const json::Array& base_requests);
    void AddBusRoutes(const json::Array& base_requests);
    svg::Color ParseColor(const json::Node& color_node) const;
};

} // namespace transport_catalogue_app::io