#include "json_reader.h"
#include "json_builder.h"
#include "map_renderer.h"

#include <sstream>

namespace transport_catalogue_app::io {

using namespace transport_catalogue_app::core;

JsonReader::JsonReader(TransportCatalogue& catalogue,
                       const transport_catalogue_app::domain::RoutingSettings& routing_settings)
    : catalogue_(catalogue)
    , routing_settings_(routing_settings)
    // Пока не создаём router, чтобы он строился уже после заполнения данных
    , request_handler_(nullptr) 
{
}

void JsonReader::ProcessBaseRequests(const json::Array& base_requests) {
    AddStops(base_requests);
    AddDistances(base_requests);
    AddBusRoutes(base_requests);
}

void JsonReader::CreateRouterAfterBase() {
    // Именно тут создаём RequestHandler, значит здесь же внутри него строится TransportRouter
    request_handler_ = std::make_unique<RequestHandler>(catalogue_, routing_settings_);
}

void JsonReader::AddStops(const json::Array& base_requests) {
    for (const auto& base_request : base_requests) {
        const auto& request_map = base_request.AsDict();
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
        const auto& request_map = base_request.AsDict();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Stop") {
            const std::string& name = request_map.at("name").AsString();
            const auto* from_stop = catalogue_.GetStopInfo(name);
            for (const auto& [neighbor, distance_node] : request_map.at("road_distances").AsDict()) {
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
        const auto& request_map = base_request.AsDict();
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

json::Array JsonReader::ProcessStatRequests(const json::Array& stat_requests,
                                            const map_renderer::MapRenderer& renderer) 
{
    // Проверяем, что request_handler_ уже создан:
    if (!request_handler_) {
        request_handler_ = std::make_unique<RequestHandler>(catalogue_, routing_settings_);
    }

    json::Array responses;
    responses.reserve(stat_requests.size());

    for (const auto& stat_request : stat_requests) {
        const auto& request_map = stat_request.AsDict();
        const std::string& type = request_map.at("type").AsString();
        int id = request_map.at("id").AsInt();

        json::Builder builder;
        builder.StartDict().Key("request_id").Value(id);

        if (type == "Stop") {
            auto stop_result = request_handler_->GetStopInfo(request_map.at("name").AsString());
            if (!stop_result.found) {
                builder.Key("error_message").Value("not found");
            } else {
                builder.Key("buses").StartArray();
                for (auto bus_name : stop_result.buses) {
                    builder.Value(std::string(bus_name));
                }
                builder.EndArray();
            }
        } 
        else if (type == "Bus") {
            auto bus_result = request_handler_->GetBusInfo(request_map.at("name").AsString());
            if (!bus_result.found) {
                builder.Key("error_message").Value("not found");
            } else {
                builder.Key("curvature").Value(bus_result.curvature)
                       .Key("route_length").Value(static_cast<int>(bus_result.route_length))
                       .Key("stop_count").Value(bus_result.stop_count)
                       .Key("unique_stop_count").Value(bus_result.unique_stop_count);
            }
        }
        else if (type == "Map") {
            svg::Document svg_map = renderer.RenderMap();
            std::ostringstream oss;
            svg_map.Render(oss);
            builder.Key("map").Value(oss.str());
        }
        else if (type == "Route") {
            auto route_result = request_handler_->GetRoute(
                request_map.at("from").AsString(),
                request_map.at("to").AsString()
            );
            if (!route_result.found) {
                builder.Key("error_message").Value("not found");
            } else {
                builder.Key("total_time").Value(route_result.total_time)
                       .Key("items").StartArray();
                for (const auto& item : route_result.items) {
                    if (item.type == transport_catalogue_app::domain::EdgeType::WAIT) {
                        builder.StartDict()
                            .Key("type").Value("Wait")
                            .Key("stop_name").Value(item.stop_name)
                            .Key("time").Value(item.time)
                        .EndDict();
                    } else {
                        builder.StartDict()
                            .Key("type").Value("Bus")
                            .Key("bus").Value(item.bus)
                            .Key("span_count").Value(item.span_count)
                            .Key("time").Value(item.time)
                        .EndDict();
                    }
                }
                builder.EndArray();
            }
        }
        responses.push_back(builder.EndDict().Build());
    }
    return responses;
}

map_renderer::RenderSettings JsonReader::ParseRenderSettings(const json::Node& render_settings_node) const {
    const auto& settings_map = render_settings_node.AsDict();
    map_renderer::RenderSettings settings;

    settings.width  = settings_map.at("width").AsDouble();
    settings.height = settings_map.at("height").AsDouble();
    settings.padding = settings_map.at("padding").AsDouble();
    settings.line_width = settings_map.at("line_width").AsDouble();
    settings.stop_radius = settings_map.at("stop_radius").AsDouble();
    settings.bus_label_font_size = settings_map.at("bus_label_font_size").AsInt();

    const auto& bus_label_offset = settings_map.at("bus_label_offset").AsArray();
    settings.bus_label_offset = {
        bus_label_offset[0].AsDouble(),
        bus_label_offset[1].AsDouble()
    };

    settings.stop_label_font_size = settings_map.at("stop_label_font_size").AsInt();
    const auto& stop_label_offset = settings_map.at("stop_label_offset").AsArray();
    settings.stop_label_offset = {
        stop_label_offset[0].AsDouble(),
        stop_label_offset[1].AsDouble()
    };

    const auto& underlayer_color_node = settings_map.at("underlayer_color");
    settings.underlayer_color = ParseColor(underlayer_color_node);

    settings.underlayer_width = settings_map.at("underlayer_width").AsDouble();

    const auto& palette = settings_map.at("color_palette").AsArray();
    for (const auto& color_node : palette) {
        settings.color_palette.emplace_back(ParseColor(color_node));
    }

    return settings;
}

svg::Color JsonReader::ParseColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();
    } else if (color_node.IsArray()) {
        const auto& arr = color_node.AsArray();
        if (arr.size() == 3) {
            return svg::Rgb{
                static_cast<uint8_t>(arr[0].AsInt()),
                static_cast<uint8_t>(arr[1].AsInt()),
                static_cast<uint8_t>(arr[2].AsInt())
            };
        } else if (arr.size() == 4) {
            return svg::Rgba{
                static_cast<uint8_t>(arr[0].AsInt()),
                static_cast<uint8_t>(arr[1].AsInt()),
                static_cast<uint8_t>(arr[2].AsInt()),
                arr[3].AsDouble()
            };
        }
    }
    return {};
}

} // namespace transport_catalogue_app::io