#pragma once

#include "bulk_updater.h"
#include "date.h"
#include "summing_segment_tree.h"

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    // Посчитать индекс дня (от 0 до ...)
    static size_t GetDayIndex(const Date& day) {
        return static_cast<size_t>(
            Date::ComputeDistance(START_DATE, day)
        );
    }

    static IndexSegment MakeDateSegment(const Date& from, const Date& to) {
        // правую границу берем +1, т.к. [left, right)
        return {GetDayIndex(from), GetDayIndex(to) + 1};
    }

    // Вычислить чистый доход = (итого earned - итого spent) на интервале
    double ComputeSum(const Date& from, const Date& to) const;

    // Добавить "bulk"-операцию (Earn / Spend / PayTax) на интервале
    void AddBulkOperation(const Date& from, const Date& to, const BulkLinearUpdater& operation) {
        tree_.AddBulkOperation(MakeDateSegment(from, to), operation);
    }

private:
    // Вместо double теперь храним DayState
    SummingSegmentTree<DayState, BulkLinearUpdater> tree_{
        GetDayIndex(END_DATE)
    };
};
