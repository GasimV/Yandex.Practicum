#include "stat_reader.h"
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include "geo.h"

namespace transport_catalogue_app::io {

using transport_catalogue_app::core::TransportCatalogue;
using transport_catalogue_app::detail::ComputeDistance;

// Helper function to print bus information
void PrintBusInfo(const TransportCatalogue& catalogue, std::string_view bus_name, std::ostream& output) {
    auto stats = catalogue.GetRouteStatistics(bus_name);
    if (stats.total_stops == 0) {
        output << "Bus " << bus_name << ": not found\n";
        return;
    }

    output << "Bus " << bus_name << ": "
           << stats.total_stops << " stops on route, "
           << stats.unique_stops << " unique stops, "
           << static_cast<int>(stats.route_length) << " route length, "
           << std::fixed << std::setprecision(5) << stats.curvature << " curvature\n";
}

// Helper function to print stop information
void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view stop_name, std::ostream& output) {
    const auto* buses = transport_catalogue.GetBusesForStop(stop_name);
    if (!buses) {
        output << "Stop " << stop_name << ": not found\n";
    } else if (buses->empty()) {
        output << "Stop " << stop_name << ": no buses\n";
    } else {
        output << "Stop " << stop_name << ": buses";
        for (const auto& bus : *buses) {
            output << " " << bus;
        }
        output << "\n";
    }
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    if (request.substr(0, 4) == "Bus ") {
        PrintBusInfo(transport_catalogue, request.substr(4), output);
    } else if (request.substr(0, 5) == "Stop ") {
        PrintStopInfo(transport_catalogue, request.substr(5), output);
    }
}

} // namespace transport_catalogue_app::io