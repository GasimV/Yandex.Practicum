#pragma once

#include "transport_catalogue.h"
#include "json.h"

namespace transport_catalogue_app::core {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& catalogue);

    // Processes a "Stop" request and returns a JSON Node with response.
    json::Node HandleStopRequest(const std::string& stop_name, int request_id) const;

    // Processes a "Bus" request and returns a JSON Node with response.
    json::Node HandleBusRequest(const std::string& bus_name, int request_id) const;

private:
    const TransportCatalogue& catalogue_;
};

} // namespace transport_catalogue_app::core