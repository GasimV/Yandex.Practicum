#include "request_handler.h"

namespace transport_catalogue_app::core {

RequestHandler::RequestHandler(const TransportCatalogue& catalogue,
                               const transport_catalogue_app::domain::RoutingSettings& routing_settings)
    : catalogue_(catalogue)
    , routing_settings_(routing_settings)
    , router_(std::make_unique<TransportRouter>(catalogue_,
                                                  routing_settings_.bus_wait_time,
                                                  routing_settings_.bus_velocity))
{
}

transport_catalogue_app::domain::StopInfoResult RequestHandler::GetStopInfo(const std::string& stop_name) const {
    return catalogue_.GetStopInfoResult(stop_name);
}

transport_catalogue_app::domain::BusInfoResult RequestHandler::GetBusInfo(const std::string& bus_name) const {
    return catalogue_.GetBusInfoResult(bus_name);
}

transport_catalogue_app::domain::RouteResult RequestHandler::GetRoute(const std::string& from, const std::string& to) const {
    auto route_info_opt = router_->BuildRoute(catalogue_.GetStopInfo(from), catalogue_.GetStopInfo(to));
    transport_catalogue_app::domain::RouteResult result;
    if (!route_info_opt) {
        result.found = false;
        return result;
    }
    result.found = true;
    result.total_time = route_info_opt->total_time;
    result.items = route_info_opt->items;
    return result;
}

} // namespace transport_catalogue_app::core