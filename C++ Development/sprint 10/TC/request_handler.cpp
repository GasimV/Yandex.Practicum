#include "request_handler.h" 
#include "json_reader.h"
 
namespace transport_catalogue_app::core { 
 
RequestHandler::RequestHandler(const TransportCatalogue& catalogue) 
    : catalogue_(catalogue) {} 
 
json::Node RequestHandler::HandleStopRequest(const std::string& stop_name, int request_id) const { 
    const auto* buses = catalogue_.GetBusesForStop(stop_name); 
    if (!buses) { 
        return json::Dict{ 
            {"request_id", request_id}, 
            {"error_message", std::string("not found")} 
        }; 
    } 
    json::Array bus_list; 
    for (const auto& bus : *buses) { 
        bus_list.emplace_back(std::string(bus)); 
    } 
    return json::Dict{ 
        {"request_id", request_id}, 
        {"buses", std::move(bus_list)} 
    }; 
} 
 
json::Node RequestHandler::HandleBusRequest(const std::string& bus_name, int request_id) const { 
    const auto stats = catalogue_.GetRouteStatistics(bus_name); 
    if (stats.total_stops == 0) { 
        return json::Dict{ 
            {"request_id", request_id}, 
            {"error_message", std::string("not found")} 
        }; 
    } 
    return json::Dict{ 
        {"request_id", request_id}, 
        {"curvature", stats.curvature}, 
        {"route_length", static_cast<int>(stats.route_length)}, 
        {"stop_count", stats.total_stops}, 
        {"unique_stop_count", stats.unique_stops} 
    }; 
} 
 
} // namespace transport_catalogue_app::core