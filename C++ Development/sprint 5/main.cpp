#include "log_duration.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

// Наивная реализация реверсирования
vector<int> ReverseVector(const vector<int>& source_vector) {
    vector<int> res;

    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }

    return res;
}

// Хорошая реализация реверсирования (вставки в конец)
vector<int> ReverseVector2(const vector<int>& source_vector) {
    vector<int> res;

    // будем проходить source_vector задом наперёд
    // с помощью обратного итератора
    for (auto iterator = source_vector.rbegin(); iterator != source_vector.rend(); ++iterator) {
        res.push_back(*iterator);
    }

    return res;
}

// Отличная реализация реверсирования (двумя итераторами)
vector<int> ReverseVector3(const vector<int>& source_vector) {
    return {source_vector.rbegin(), source_vector.rend()};
}

// Собственная реализация реверсирования (заполняем с конца)
vector<int> ReverseVector4(const vector<int>& source_vector) {
    vector<int> res(source_vector.size());

    for (size_t i = 0; i < source_vector.size(); ++i) {
        res[source_vector.size() - 1 - i] = source_vector[i];
    }

    return res;
}

// Функция для запуска тестов и замеров
void Operate() {
    vector<int> rand_vector;
    int n;

    cin >> n;
    rand_vector.reserve(n);

    // Заполняем вектор случайными числами
    for (int i = 0; i < n; ++i) {
        rand_vector.push_back(rand());
    }

    // Если размер вектора <= 100000, тестируем Naive и Good
    if (n <= 100'000) {
        {
            LOG_DURATION("Naive");
            auto reversed = ReverseVector(rand_vector);
        }
        {
            LOG_DURATION("Good");
            auto reversed = ReverseVector2(rand_vector);
        }
    }
    // Если размер вектора > 100000, тестируем Good, Best и Your
    else {
        {
            LOG_DURATION("Good");
            auto reversed = ReverseVector2(rand_vector);
        }
        {
            LOG_DURATION("Best");
            auto reversed = ReverseVector3(rand_vector);
        }
        {
            LOG_DURATION("Your");
            auto reversed = ReverseVector4(rand_vector);
        }
    }
}

int main() {
    Operate();
    return 0;
}