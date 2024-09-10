#include "log_duration.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

// Наивная реализация реверсирования
vector<int> ReverseVector(const vector<int>& source_vector) {
    vector<int> res;
    res.reserve(source_vector.size());  // Резервируем память

    for (int i : source_vector) {
        res.push_back(i);
    }
    reverse(res.begin(), res.end());  // Реверсируем вектор одной строкой
    return res;
}

// Хорошая реализация реверсирования (вставки в конец)
vector<int> ReverseVector2(const vector<int>& source_vector) {
    vector<int> res;
    res.reserve(source_vector.size());  // Резервируем память

    for (auto it = source_vector.rbegin(); it != source_vector.rend(); ++it) {
        res.push_back(*it);
    }

    return res;
}

// Отличная реализация реверсирования (двумя итераторами)
vector<int> ReverseVector3(const vector<int>& source_vector) {
    return {source_vector.rbegin(), source_vector.rend()};  // Реверсируем с помощью итераторов
}

// Собственная реализация реверсирования (заполняем с конца)
vector<int> ReverseVector4(const vector<int>& source_vector) {
    vector<int> res(source_vector.size());

    for (size_t i = 0; i < source_vector.size(); ++i) {
        res[source_vector.size() - 1 - i] = source_vector[i];
    }

    return res;
}

// Оптимизация функции AppendRandom: уменьшение вызовов rand в 15 раз
void AppendRandom(vector<int>& v, int n) {
    v.reserve(n);  // Резервируем память для n элементов

    for (int i = 0; i < n; i += 15) {
        int number = rand();  // Генерируем одно случайное число
        for (int j = 0; j < 15 && i + j < n; ++j) {
            v.push_back((number >> j) % 2);  // Извлекаем биты случайного числа
        }
    }
}

// Функция CountPops для подсчёта единичных элементов в диапазоне
int CountPops(const vector<int>& source_vector, int begin, int end) {
    int res = 0;
    for (int i = begin; i < end; ++i) {
        if (source_vector[i] == 1) {
            ++res;
        }
    }
    return res;
}

// Функция для запуска тестов и замеров
void Operate() {
    vector<int> rand_vector;
    int n;

    // Чтение размера вектора
    cin >> n;
    rand_vector.reserve(n);

    // Заполнение вектора случайными числами
    AppendRandom(rand_vector, n);

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