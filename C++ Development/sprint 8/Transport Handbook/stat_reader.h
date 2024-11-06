#pragma once

#include "stat_reader.h"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);