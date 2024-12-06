#include "json_reader.h"

namespace transport_catalogue_app::io {

JsonReader::JsonReader(transport_catalogue_app::core::TransportCatalogue& catalogue)
    : catalogue_(catalogue), request_handler_(catalogue) {}

void JsonReader::ProcessBaseRequests(const json::Array& base_requests) {
    // First pass: add stops and coordinates
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

    // Second pass: add distances between stops
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

    // Third pass: add bus routes
    for (const auto& base_request : base_requests) {
        const auto& request_map = base_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Bus") {
            const std::string& name = request_map.at("name").AsString();
            std::vector<std::string_view> stops;
            for (const auto& stop_node : request_map.at("stops").AsArray()) {
                stops.push_back(stop_node.AsString());
            }
            bool is_roundtrip = request_map.at("is_roundtrip").AsBool();
            catalogue_.AddRoute(name, stops, is_roundtrip);
        }
    }
}

json::Array JsonReader::ProcessStatRequests(const json::Array& stat_requests) {
    json::Array responses;
    for (const auto& stat_request : stat_requests) {
        const auto& request_map = stat_request.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Stop") {
            responses.push_back(request_handler_.HandleStopRequest(request_map));
        } else if (type == "Bus") {
            responses.push_back(request_handler_.HandleBusRequest(request_map));
        }
    }
    return responses;
}

} // namespace transport_catalogue_app::io