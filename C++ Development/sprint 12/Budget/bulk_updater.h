#pragma once

#include "entities.h"
#include "summing_segment_tree.h"
#include <cmath>

//
// Вспомогательные структуры
//

// 1) Прибавить доход (earned) или расход (spent).
struct BulkMoneyAdder {
    DayState delta;  
    // delta.earned > 0 => это доход
    // delta.spent  > 0 => это расход
};

// 2) Налоговый множитель.
// По умолчанию factor = 1.0 означает "не менять ничего".
// Если пришёл налог 13%, то factor = (1.0 - 0.13) = 0.87.
struct BulkTaxApplier {
    double factor = 1.0;
};

//
// Основной класс, описывающий объединённую операцию
// (добавление и налог).
//
class BulkLinearUpdater {
public:
    BulkLinearUpdater() = default;

    // Конструктор от "прибавить доход/расход"
    BulkLinearUpdater(const BulkMoneyAdder& add)
        : add_(add)
    {
    }

    // Конструктор от "налог"
    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax)
    {
    }

    // Слить (объединить) другую операцию в текущую.
    // Имеется в виду, что "other" будет применяться ПОСЛЕ текущей.
    void CombineWith(const BulkLinearUpdater& other) {
        // 1) Умножаем свой налоговый фактор на фактор "other".
        tax_.factor *= other.tax_.factor;

        // 2) Сначала прибавка по earned должна быть умножена на новый налог
        add_.delta.earned = add_.delta.earned * other.tax_.factor + other.add_.delta.earned;

        // 3) Расход (spent) налогом не облагается, просто суммируем
        add_.delta.spent += other.add_.delta.spent;
    }

    // Применить (свернуть) операцию к исходным данным узла `origin`.
    // segment.length() = количество дней в этом узле.
    DayState Collapse(DayState origin, IndexSegment segment) const {
        // 1) Облагаем налогом только earned
        origin.earned *= tax_.factor;

        // 2) Прибавляем доход/расход (умноженный на количество дней)
        origin.earned += add_.delta.earned * segment.length();
        origin.spent  += add_.delta.spent  * segment.length();

        return origin;
    }

private:
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
};
