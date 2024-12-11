#include "json_reader.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "svg.h"

#include <iostream>
#include <memory>
#include <string>

int main() {
    using namespace transport_catalogue_app;

    transport_catalogue_app::core::TransportCatalogue catalogue;
    io::JsonReader json_reader(catalogue);

    // Load JSON from stdin
    json::Document doc = json::Load(std::cin);
    const auto& root = doc.GetRoot().AsMap();

    // Process base_requests
    if (root.find("base_requests") != root.end()) {
        const auto& base_requests = root.at("base_requests").AsArray();
        json_reader.ProcessBaseRequests(base_requests);
    }

    // Parse render_settings
    map_renderer::RenderSettings render_settings;
    if (root.find("render_settings") != root.end()) {
        const auto& render_settings_node = root.at("render_settings");
        render_settings = json_reader.ParseRenderSettings(render_settings_node);
    }
    else {
        // Set default render settings if not provided
        render_settings = map_renderer::RenderSettings{
            600.0, // width
            400.0, // height
            50.0,  // padding
            14.0,  // line_width
            5.0,   // stop_radius
            20,    // bus_label_font_size
            svg::Point{7.0, 15.0},   // bus_label_offset
            20,    // stop_label_font_size
            svg::Point{7.0, -3.0},  // stop_label_offset
            svg::Rgba{255, 255, 255, 0.85}, // underlayer_color
            3.0,   // underlayer_width
            {svg::Color("green"), svg::Color(svg::Rgb{255, 160, 0}), svg::Color("red")} // color_palette
        };
    }

    // Initialize MapRenderer
    map_renderer::MapRenderer renderer(render_settings, catalogue);

    // Process stat_requests and collect responses
    json::Array responses;
    if (root.find("stat_requests") != root.end()) {
        const auto& stat_requests = root.at("stat_requests").AsArray();
        responses = json_reader.ProcessStatRequests(stat_requests, renderer);
    }

    // Output JSON responses
    json::Print(json::Document{json::Node{responses}}, std::cout);

    return 0;
}