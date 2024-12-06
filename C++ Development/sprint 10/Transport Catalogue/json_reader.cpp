#include "json_reader.h"
#include "geo.h"
#include <string_view>
#include <iostream>

namespace json_reader {

using namespace transport_catalogue_app::core;

void ProcessBaseRequests(TransportCatalogue& catalogue, const json::Node& base_requests) {
    for (const auto& request_node : base_requests.AsArray()) {
        const auto& request = request_node.AsMap();
        std::string type = request.at("type").AsString();

        if (type == "Stop") {
            std::string name = request.at("name").AsString();
            double latitude = request.at("latitude").AsDouble();
            double longitude = request.at("longitude").AsDouble();

            catalogue.AddStop(name, {latitude, longitude});

            if (request.count("road_distances")) {
                const auto& distances = request.at("road_distances").AsMap();
                for (const auto& [neighbour_name, distance_node] : distances) {
                    const Stop* from = catalogue.GetStopInfo(name);
                    const Stop* to = catalogue.GetStopInfo(neighbour_name);
                    int distance = distance_node.AsInt();
                    if (from && to) {
                        catalogue.SetDistance(from, to, distance);
                    }
                }
            }
        } else if (type == "Bus") {
            std::string name = request.at("name").AsString();
            const auto& stops = request.at("stops").AsArray();
            std::vector<std::string_view> stop_names;
            for (const auto& stop_node : stops) {
                stop_names.push_back(stop_node.AsString());
            }
            bool is_roundtrip = request.at("is_roundtrip").AsBool();

            catalogue.AddRoute(name, stop_names, is_roundtrip);
        }
    }
}

json::Node ProcessStatRequests(const TransportCatalogue& catalogue, const json::Node& stat_requests) {
    json::Array responses;

    for (const auto& request_node : stat_requests.AsArray()) {
        const auto& request = request_node.AsMap();
        int request_id = request.at("id").AsInt();
        std::string type = request.at("type").AsString();

        if (type == "Bus") {
            std::string bus_name = request.at("name").AsString();
            const Route* route = catalogue.GetRouteInfo(bus_name);
            if (route) {
                TransportCatalogue::RouteStats stats = catalogue.GetRouteStatistics(bus_name);
                json::Dict response = {
                    {"request_id", request_id},
                    {"route_length", stats.route_length},
                    {"stop_count", stats.total_stops},
                    {"unique_stop_count", stats.unique_stops},
                    {"curvature", stats.curvature}
                };
                responses.push_back(std::move(response));
            } else {
                responses.push_back(json::Dict{
                    {"request_id", request_id},
                    {"error_message", "not found"}
                });
            }
        } else if (type == "Stop") {
            std::string stop_name = request.at("name").AsString();
            const auto* buses = catalogue.GetBusesForStop(stop_name);
            if (buses) {
                json::Array bus_list;
                for (const auto& bus_name : *buses) {
                    bus_list.push_back(std::string(bus_name));
                }
                responses.push_back(json::Dict{
                    {"request_id", request_id},
                    {"buses", std::move(bus_list)}
                });
            } else {
                responses.push_back(json::Dict{
                    {"request_id", request_id},
                    {"error_message", "not found"}
                });
            }
        }
    }

    return json::Node(std::move(responses));
}

} // namespace json_reader