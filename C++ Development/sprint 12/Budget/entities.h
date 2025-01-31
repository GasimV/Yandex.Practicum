#pragma once

struct DayState {
    double earned = 0.0;
    double spent = 0.0;
};

inline DayState operator+(const DayState& lhs, const DayState& rhs) {
    return {
        lhs.earned + rhs.earned,
        lhs.spent + rhs.spent
    };
}
