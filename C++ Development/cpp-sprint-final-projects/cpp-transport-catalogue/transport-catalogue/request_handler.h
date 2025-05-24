#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "domain.h"
#include <memory>
#include <string>

namespace transport_catalogue_app::core {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& catalogue,
                   const transport_catalogue_app::domain::RoutingSettings& routing_settings);

    // Делегируем получение информации соответствующим модулям
    transport_catalogue_app::domain::StopInfoResult GetStopInfo(const std::string& stop_name) const;
    transport_catalogue_app::domain::BusInfoResult GetBusInfo(const std::string& bus_name) const;
    transport_catalogue_app::domain::RouteResult GetRoute(const std::string& from, const std::string& to) const;

private:
    const TransportCatalogue& catalogue_;
    transport_catalogue_app::domain::RoutingSettings routing_settings_;

    // Создаём роутер сразу при инициализации
    std::unique_ptr<TransportRouter> router_;
};

} // namespace transport_catalogue_app::core