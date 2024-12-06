#pragma once

#include "transport_catalogue.h"
#include "json.h"

namespace transport_catalogue_app::core {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& catalogue);

    // Processes a "Stop" request and returns a JSON Node with response.
    json::Node HandleStopRequest(const json::Dict& request) const;

    // Processes a "Bus" request and returns a JSON Node with response.
    json::Node HandleBusRequest(const json::Dict& request) const;

private:
    const TransportCatalogue& catalogue_;
};

} // namespace transport_catalogue_app::core