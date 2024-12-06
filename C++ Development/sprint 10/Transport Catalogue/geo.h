#pragma once

namespace transport_catalogue_app::detail {

struct Coordinates {
    double lat;
    double lng;

    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

double ComputeDistance(Coordinates from, Coordinates to);

} // namespace transport_catalogue_app::detail