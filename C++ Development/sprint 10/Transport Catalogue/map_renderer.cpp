#include "map_renderer.h"

#include <algorithm>
#include <cmath>

namespace transport_catalogue_app::map_renderer {

// Collects all coordinates from all routes
std::vector<transport_catalogue_app::detail::Coordinates> MapRenderer::CollectRouteCoordinates() const {
    std::vector<transport_catalogue_app::detail::Coordinates> coordinates;
    const auto& all_routes = catalogue_.GetAllRoutes();
    for (const auto& [route_name, route_ptr] : all_routes) {
        for (const auto* stop : route_ptr->stops) {
            coordinates.emplace_back(stop->coordinates);
        }
    }
    return coordinates;
}

// Assigns colors to routes based on lex order and color palette
void MapRenderer::AssignRouteColors() {
    const auto& all_routes = catalogue_.GetAllRoutes();
    // Extract route names
    sorted_route_names_.reserve(all_routes.size());
    for (const auto& [route_name, _] : all_routes) {
        sorted_route_names_.emplace_back(route_name);
    }

    // Sort route names lexicographically
    std::sort(sorted_route_names_.begin(), sorted_route_names_.end());

    // Assign colors from the palette, cycling if necessary
    size_t palette_size = settings_.color_palette.size();
    route_colors_.reserve(sorted_route_names_.size());
    for (size_t i = 0; i < sorted_route_names_.size(); ++i) {
        route_colors_.emplace_back(settings_.color_palette[i % palette_size]);
    }
}

// Renders the map by drawing polylines for each route
svg::Document MapRenderer::RenderMap() const {
    svg::Document doc;

    for (size_t i = 0; i < sorted_route_names_.size(); ++i) {
        const std::string& route_name = sorted_route_names_[i];
        const auto* route = catalogue_.GetRouteInfo(route_name);
        if (!route || route->stops.empty()) {
            continue; // Skip routes with no stops
        }

        svg::Polyline polyline;
        polyline.SetFillColor(svg::NoneColor)
                .SetStrokeColor(route_colors_[i])
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        // Add projected points for the route
        for (const auto* stop : route->stops) {
            polyline.AddPoint((*projector_)(stop->coordinates));
        }

        // If the route is not a roundtrip, add the return path
        if (!route->is_cyclic && route->stops.size() > 1) {
            for (auto it = route->stops.rbegin() + 1; it != route->stops.rend(); ++it) {
                polyline.AddPoint((*projector_)((*it)->coordinates));
            }
        }

        doc.Add(polyline);
    }

    return doc;
}

} // namespace transport_catalogue_app::map_renderer