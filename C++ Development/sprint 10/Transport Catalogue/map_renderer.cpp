#include "map_renderer.h"

#include <algorithm>
#include <cmath>

using transport_catalogue_app::core::Stop;
using std::string_literals::operator""s; // Для использования "s" литералов

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

// Renders the entire map with all layers
svg::Document MapRenderer::RenderMap() const {
    svg::Document doc;

    // 1. Draw polylines for all routes
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

    // 2. Draw route names
    RenderRouteNames(doc);

    // 3. Draw stop symbols (circles)
    RenderStopSymbols(doc);

    // 4. Draw stop names
    RenderStopNames(doc);

    return doc;
}

// Helper method to render route names
void MapRenderer::RenderRouteNames(svg::Document& doc) const {
    // Iterate over sorted route names
    for (size_t i = 0; i < sorted_route_names_.size(); ++i) {
        const std::string& route_name = sorted_route_names_[i];
        const auto* route = catalogue_.GetRouteInfo(route_name);
        if (!route || route->stops.empty()) {
            continue; // Skip routes with no stops
        }

        // Determine final stops
        std::vector<const Stop*> final_stops;
        if (route->is_cyclic) {
            final_stops.push_back(route->stops.front());
        } else {
            final_stops.push_back(route->stops.front());
            final_stops.push_back(route->stops.back());
            // Если первый и последний остановки совпадают, избегаем дублирования
            if (route->stops.front()->name == route->stops.back()->name) {
                final_stops.pop_back();
            }
        }

        // For each final stop, draw background and label
        for (const auto* stop : final_stops) {
            svg::Point stop_point = (*projector_)(stop->coordinates);

            // Background text
            svg::Text background_text;
            background_text.SetPosition(stop_point)
                           .SetOffset(settings_.bus_label_offset)
                           .SetFontSize(settings_.bus_label_font_size)
                           .SetFontFamily("Verdana"s)
                           .SetFontWeight("bold"s)
                           .SetFillColor(settings_.underlayer_color)
                           .SetStrokeColor(settings_.underlayer_color)
                           .SetStrokeWidth(settings_.underlayer_width)
                           .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                           .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                           .SetData(route_name);
            doc.Add(background_text);

            // Label text
            svg::Text label_text;
            label_text.SetPosition(stop_point)
                      .SetOffset(settings_.bus_label_offset)
                      .SetFontSize(settings_.bus_label_font_size)
                      .SetFontFamily("Verdana"s)
                      .SetFontWeight("bold"s)
                      .SetFillColor(route_colors_[i])
                      .SetData(route_name);
            doc.Add(label_text);
        }
    }
}

// Helper method to render stop symbols (circles)
void MapRenderer::RenderStopSymbols(svg::Document& doc) const {
    // Получаем все остановки, участвующие в маршрутах
    std::vector<const transport_catalogue_app::core::Stop*> all_stops = catalogue_.GetAllStops();

    // Сортируем остановки лексикографически по имени
    std::sort(all_stops.begin(), all_stops.end(), [](const Stop* lhs, const Stop* rhs) {
        return lhs->name < rhs->name;
    });

    // Рисуем круги для остановок
    for (const auto* stop : all_stops) {
        svg::Circle circle;
        svg::Point stop_point = (*projector_)(stop->coordinates);
        circle.SetCenter(stop_point)
              .SetRadius(settings_.stop_radius)
              .SetFillColor("white"s);
        doc.Add(circle);
    }
}

void MapRenderer::RenderStopNames(svg::Document& doc) const {
    // Получаем все остановки, участвующие в маршрутах
    std::vector<const transport_catalogue_app::core::Stop*> all_stops = catalogue_.GetAllStops();

    // Сортируем остановки лексикографически по имени
    std::sort(all_stops.begin(), all_stops.end(), [](const Stop* lhs, const Stop* rhs) {
        return lhs->name < rhs->name;
    });

    // Рисуем названия остановок
    for (const auto* stop : all_stops) {
        svg::Point stop_point = (*projector_)(stop->coordinates);

        // Подложка текста
        svg::Text background_text;
        background_text.SetPosition(stop_point)
                       .SetOffset(settings_.stop_label_offset)
                       .SetFontSize(settings_.stop_label_font_size)
                       .SetFontFamily("Verdana"s)
                       // Не устанавливаем font-weight
                       .SetFillColor(settings_.underlayer_color)
                       .SetStrokeColor(settings_.underlayer_color)
                       .SetStrokeWidth(settings_.underlayer_width)
                       .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                       .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                       .SetData(stop->name);
        doc.Add(background_text);

        // Текстовое содержимое
        svg::Text label_text;
        label_text.SetPosition(stop_point)
                  .SetOffset(settings_.stop_label_offset)
                  .SetFontSize(settings_.stop_label_font_size)
                  .SetFontFamily("Verdana"s)
                  // Не устанавливаем font-weight
                  .SetFillColor("black"s)
                  .SetData(stop->name);
        doc.Add(label_text);
    }
}

} // namespace transport_catalogue_app::map_renderer