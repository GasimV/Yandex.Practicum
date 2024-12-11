#include "json_reader.h"
#include "map_renderer.h"

#include <sstream>

namespace transport_catalogue_app::io {

JsonReader::JsonReader(transport_catalogue_app::core::TransportCatalogue& catalogue)
    : catalogue_(catalogue), request_handler_(catalogue) {}

void JsonReader::ProcessBaseRequests(const json::Array& base_requests) {
    AddStops(base_requests);
    AddDistances(base_requests);
    AddBusRoutes(base_requests);
}

void JsonReader::AddStops(const json::Array& base_requests) {
    for (const auto& base_request : base_requests) {
        const auto& request_map = base_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Stop") {
            const std::string& name = request_map.at("name").AsString();
            double latitude = request_map.at("latitude").AsDouble();
            double longitude = request_map.at("longitude").AsDouble();
            catalogue_.AddStop(name, {latitude, longitude});
        }
    }
}

void JsonReader::AddDistances(const json::Array& base_requests) {
    for (const auto& base_request : base_requests) {
        const auto& request_map = base_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Stop") {
            const std::string& name = request_map.at("name").AsString();
            const auto* from_stop = catalogue_.GetStopInfo(name);
            for (const auto& [neighbor, distance_node] : request_map.at("road_distances").AsMap()) {
                const int distance = distance_node.AsInt();
                const auto* to_stop = catalogue_.GetStopInfo(neighbor);
                if (to_stop) {
                    catalogue_.SetDistance(from_stop, to_stop, distance);
                }
            }
        }
    }
}

void JsonReader::AddBusRoutes(const json::Array& base_requests) {
    for (const auto& base_request : base_requests) {
        const auto& request_map = base_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Bus") {
            const std::string& name = request_map.at("name").AsString();
            std::vector<std::string_view> stops;
            for (const auto& stop_node : request_map.at("stops").AsArray()) {
                stops.emplace_back(stop_node.AsString());
            }
            bool is_roundtrip = request_map.at("is_roundtrip").AsBool();
            catalogue_.AddRoute(name, stops, is_roundtrip);
        }
    }
}

json::Array JsonReader::ProcessStatRequests(const json::Array& stat_requests, const map_renderer::MapRenderer& renderer) {
    json::Array responses;
    for (const auto& stat_request : stat_requests) {
        const auto& request_map = stat_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        int id = request_map.at("id").AsInt();
        if (type == "Stop") {
            const std::string& stop_name = request_map.at("name").AsString();
            responses.emplace_back(request_handler_.HandleStopRequest(stop_name, id));
        } else if (type == "Bus") {
            const std::string& bus_name = request_map.at("name").AsString();
            responses.emplace_back(request_handler_.HandleBusRequest(bus_name, id));
        } else if (type == "Map") {
            // Генерация SVG-карты
            svg::Document svg_map = renderer.RenderMap();
            std::ostringstream oss;
            svg_map.Render(oss);
            std::string svg_str = oss.str();

            // Создание JSON-ответа для "Map"
            responses.emplace_back(json::Dict{
                {"map", std::move(svg_str)},
                {"request_id", id}
            });
        }
    }
    return responses;
}

map_renderer::RenderSettings JsonReader::ParseRenderSettings(const json::Node& render_settings_node) const {
    const auto& settings_map = render_settings_node.AsMap();
    map_renderer::RenderSettings settings;

    // Parse each required field
    settings.width = settings_map.at("width").AsDouble();
    settings.height = settings_map.at("height").AsDouble();
    settings.padding = settings_map.at("padding").AsDouble();
    settings.line_width = settings_map.at("line_width").AsDouble();
    settings.stop_radius = settings_map.at("stop_radius").AsDouble();
    settings.bus_label_font_size = settings_map.at("bus_label_font_size").AsInt();

    const auto& bus_label_offset = settings_map.at("bus_label_offset").AsArray();
    settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };

    settings.stop_label_font_size = settings_map.at("stop_label_font_size").AsInt();
    const auto& stop_label_offset = settings_map.at("stop_label_offset").AsArray();
    settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    // Parse underlayer_color
    const auto& underlayer_color_node = settings_map.at("underlayer_color");
    settings.underlayer_color = ParseColor(underlayer_color_node);

    settings.underlayer_width = settings_map.at("underlayer_width").AsDouble();

    // Parse color_palette
    const auto& palette = settings_map.at("color_palette").AsArray();
    for (const auto& color_node : palette) {
        settings.color_palette.emplace_back(ParseColor(color_node));
    }

    return settings;
}

svg::Color JsonReader::ParseColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();
    } else if (color_node.AsArray().size() == 3) {
        const auto& color_array = color_node.AsArray();
        return svg::Rgb{
            static_cast<uint8_t>(color_array[0].AsInt()),
            static_cast<uint8_t>(color_array[1].AsInt()),
            static_cast<uint8_t>(color_array[2].AsInt())
        };
    } else if (color_node.AsArray().size() == 4) {
        const auto& color_array = color_node.AsArray();
        return svg::Rgba{
            static_cast<uint8_t>(color_array[0].AsInt()),
            static_cast<uint8_t>(color_array[1].AsInt()),
            static_cast<uint8_t>(color_array[2].AsInt()),
            color_array[3].AsDouble()
        };
    }
    return {};
}

} // namespace transport_catalogue_app::io