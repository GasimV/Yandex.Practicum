#include <algorithm>
#include <cassert>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
#include <type_traits>

using namespace std;

// Функция ComputeStatistics
template <typename InputIt, typename OutSum, typename OutSqSum, typename OutMax>
constexpr void ComputeStatistics(InputIt first, InputIt last, OutSum& out_sum, OutSqSum& out_sq_sum, OutMax& out_max) {
    // Определяем тип элемента диапазона
    using Elem = std::decay_t<decltype(*first)>;

    // Определяем, какую статистику требуется вычислять
    constexpr bool need_sum    = !std::is_same_v<OutSum, const std::nullopt_t>;
    constexpr bool need_sq_sum = !std::is_same_v<OutSqSum, const std::nullopt_t>;
    constexpr bool need_max    = !std::is_same_v<OutMax, const std::nullopt_t>;

    // Если диапазон пуст, ничего не делаем.
    if (first == last)
        return;

    // Инициализируем накопители, используя первый элемент диапазона.
    auto it = first;
    if constexpr (need_sum) {
        Elem sum_val = *it;
        if constexpr (need_sq_sum) {
            Elem sq_val = (*it) * (*it);
            if constexpr (need_max) {
                Elem max_val = *it;
                ++it;
                for (; it != last; ++it) {
                    sum_val += *it;
                    sq_val  += (*it) * (*it);
                    if (max_val < *it)
                        max_val = *it;
                }
                out_sum    = sum_val;
                out_sq_sum = sq_val;
                out_max    = max_val;
            } else { // вычисляем только сумму и сумму квадратов
                ++it;
                for (; it != last; ++it) {
                    sum_val += *it;
                    sq_val  += (*it) * (*it);
                }
                out_sum    = sum_val;
                out_sq_sum = sq_val;
            }
        } else { // не нужна сумма квадратов
            if constexpr (need_max) {
                Elem max_val = *it;
                ++it;
                for (; it != last; ++it) {
                    sum_val += *it;
                    if (max_val < *it)
                        max_val = *it;
                }
                out_sum = sum_val;
                out_max = max_val;
            } else { // вычисляем только сумму
                ++it;
                for (; it != last; ++it)
                    sum_val += *it;
                out_sum = sum_val;
            }
        }
    } else { // сумма не нужна
        if constexpr (need_sq_sum) {
            Elem sq_val = (*it) * (*it);
            if constexpr (need_max) {
                Elem max_val = *it;
                ++it;
                for (; it != last; ++it) {
                    sq_val += (*it) * (*it);
                    if (max_val < *it)
                        max_val = *it;
                }
                out_sq_sum = sq_val;
                out_max    = max_val;
            } else { // вычисляем только сумму квадратов
                ++it;
                for (; it != last; ++it)
                    sq_val += (*it) * (*it);
                out_sq_sum = sq_val;
            }
        } else { // сумма и сумма квадратов не нужны
            if constexpr (need_max) {
                Elem max_val = *it;
                ++it;
                for (; it != last; ++it) {
                    if (max_val < *it)
                        max_val = *it;
                }
                out_max = max_val;
            }
            // Если ни одна статистика не требуется – ничего не делаем.
        }
    }
}

struct OnlySum {
    int value;
};

OnlySum operator+(OnlySum l, OnlySum r) {
    return {l.value + r.value};
}
OnlySum& operator+=(OnlySum& l, OnlySum r) {
    return l = l + r;
}

int main() {
    vector input = {1, 2, 3, 4, 5, 6};
    int sq_sum;
    std::optional<int> max;

    // Переданы выходные параметры разных типов - std::nullopt_t, int и std::optional<int>
    ComputeStatistics(input.begin(), input.end(), nullopt, sq_sum, max);

    assert(sq_sum == 91 && max && *max == 6);

    vector<OnlySum> only_sum_vector = {{100}, {-100}, {20}};
    OnlySum sum;

    // Поданы значения поддерживающие только суммирование, но запрошена только сумма
    ComputeStatistics(only_sum_vector.begin(), only_sum_vector.end(), sum, nullopt, nullopt);

    assert(sum.value == 20);
}