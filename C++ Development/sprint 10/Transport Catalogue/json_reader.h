#pragma once

#include "transport_catalogue.h"
#include "json.h"

namespace json_reader {

void ProcessBaseRequests(transport_catalogue_app::core::TransportCatalogue& catalogue, const json::Node& base_requests);
json::Node ProcessStatRequests(const transport_catalogue_app::core::TransportCatalogue& catalogue, const json::Node& stat_requests);

} // namespace json_reader
