#include "json.h"
#include "json_reader.h"
#include <iostream>

using namespace transport_catalogue_app;

int main() {
    try {
        // Load input JSON from stdin.
        auto doc = json::Load(std::cin);
        const auto& root = doc.GetRoot().AsMap();

        // Create the transport catalogue.
        core::TransportCatalogue catalogue;

        // Create a JSON reader to parse requests.
        io::JsonReader json_reader(catalogue);

        // Process base_requests.
        json_reader.ProcessBaseRequests(root.at("base_requests").AsArray());

        // Process stat_requests and get responses.
        auto responses = json_reader.ProcessStatRequests(root.at("stat_requests").AsArray());

        // Print the responses as JSON to stdout.
        json::Print(json::Document{responses}, std::cout);
    } catch (const json::ParsingError& e) {
        std::cerr << "Error parsing input: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
