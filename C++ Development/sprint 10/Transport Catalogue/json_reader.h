#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace transport_catalogue_app::io {

class JsonReader {
public:
    JsonReader(transport_catalogue_app::core::TransportCatalogue& catalogue);

    // Reads and processes base_requests to fill the catalogue.
    void ProcessBaseRequests(const json::Array& base_requests);

    // Reads and processes stat_requests, returning a JSON array of responses.
    json::Array ProcessStatRequests(const json::Array& stat_requests);

private:
    transport_catalogue_app::core::TransportCatalogue& catalogue_;
    transport_catalogue_app::core::RequestHandler request_handler_;
};

} // namespace transport_catalogue_app::io