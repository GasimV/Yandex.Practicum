#pragma once

#include "transport_catalogue.h"
#include "geo.h"
#include "svg.h"

#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <memory>

namespace transport_catalogue_app::map_renderer {

// Structure to hold rendering settings
struct RenderSettings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    svg::Point bus_label_offset;
    int stop_label_font_size;
    svg::Point stop_label_offset;
    svg::Color underlayer_color;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
};

// Helper function to check if a value is approximately zero
inline const double EPSILON = 1e-6;
inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

// Class responsible for projecting spherical coordinates to SVG points
class SphereProjector {
public:
    // Template constructor to accept a range of transport_catalogue_app::detail::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                   double max_width, double max_height, double padding)
        : padding_(padding)
    {
        if (points_begin == points_end) {
            return;
        }

        // Find min and max longitude
        auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](const transport_catalogue_app::detail::Coordinates& lhs,
               const transport_catalogue_app::detail::Coordinates& rhs) {
                return lhs.lng < rhs.lng;
            });
        min_lon_ = left_it->lng;
        double max_lon = right_it->lng;

        // Find min and max latitude
        auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](const transport_catalogue_app::detail::Coordinates& lhs,
               const transport_catalogue_app::detail::Coordinates& rhs) {
                return lhs.lat < rhs.lat;
            });
        double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Compute zoom coefficients
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
        }
    }

    // Projects transport_catalogue_app::detail::Coordinates to svg::Point
    svg::Point operator()(const transport_catalogue_app::detail::Coordinates& coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_;
    double max_lat_;
    double zoom_coeff_ = 0;
};

// Class responsible for rendering the map
class MapRenderer {
public:
    MapRenderer(const RenderSettings& settings, const transport_catalogue_app::core::TransportCatalogue& catalogue)
        : settings_(settings), catalogue_(catalogue)
    {
        // Collect all coordinates used in the routes
        auto coordinates = CollectRouteCoordinates();

        // Initialize the SphereProjector with the collected coordinates
        projector_ = std::make_unique<SphereProjector>(
            coordinates.begin(), coordinates.end(),
            settings.width, settings.height, settings.padding
        );

        // Assign colors to the routes
        AssignRouteColors();
    }

    // Renders the map and returns the SVG document
    svg::Document RenderMap() const;

private:
    RenderSettings settings_;
    const transport_catalogue_app::core::TransportCatalogue& catalogue_;
    std::unique_ptr<SphereProjector> projector_;
    std::vector<std::string> sorted_route_names_;
    std::vector<svg::Color> route_colors_;

    // Assigns colors to routes based on lexicographical order
    void AssignRouteColors();

    // Collects all coordinates used in the routes
    std::vector<transport_catalogue_app::detail::Coordinates> CollectRouteCoordinates() const;
    
    // Helper method to render a single route
    void RenderRoute(svg::Document& doc, const std::string& route_name, svg::Color color) const;
    
    // Methods for additional rendering layers
    void RenderRouteNames(svg::Document& doc) const;
    void RenderStopSymbols(svg::Document& doc) const;
    void RenderStopNames(svg::Document& doc) const;
};

} // namespace transport_catalogue_app::map_renderer