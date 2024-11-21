#pragma once

#include "stat_reader.h"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "transport_catalogue.h"

namespace transport_catalogue_app::io {

void ParseAndPrintStat(const core::TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);

} // namespace transport_catalogue_app::io