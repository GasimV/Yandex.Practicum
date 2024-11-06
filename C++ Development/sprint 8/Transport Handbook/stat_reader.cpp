#include "stat_reader.h"
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include "geo.h"

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    if (request.substr(0, 4) == "Bus ") {
        std::string bus_name{request.substr(4)};
        auto route = transport_catalogue.GetRouteInfo(bus_name);
        if (!route) {
            output << "Bus " << bus_name << ": not found\n";
            return;
        }

        const auto& stops = route->stops;
        int num_stops = stops.size();
        std::unordered_set<std::string_view> unique_stops;
        double length = 0.0;
        
        for (size_t i = 0; i + 1 < stops.size(); ++i) {
            length += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
            unique_stops.insert(stops[i]->name);
        }
        unique_stops.insert(stops.back()->name);

        output << "Bus " << bus_name << ": " << num_stops << " stops on route, "
               << unique_stops.size() << " unique stops, "
               << std::fixed << std::setprecision(6) << length << " route length\n";
    }
    else if (request.substr(0, 5) == "Stop ") {
        std::string stop_name{request.substr(5)};
        auto buses = transport_catalogue.GetBusesForStop(stop_name);
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
}