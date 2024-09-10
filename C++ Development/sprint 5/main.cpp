#include "log_duration.h"

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std;

vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
    if (measures.empty()) {
        return {}; // Если пустой набор, возвращаем пустой вектор
    }

    // int days = measures.size();
    int measurements_per_day = measures[0].size();

    // Вектор для суммы положительных температур
    vector<float> sum(measurements_per_day, 0.0f);
    // Вектор для подсчета положительных измерений
    vector<int> count(measurements_per_day, 0);

    // Проход по каждому дню
    for (const auto& day : measures) {
        // Проход по каждому измерению
        for (int i = 0; i < measurements_per_day; ++i) {
            // Если температура положительная, добавляем её в сумму и увеличиваем счетчик
            sum[i] += (day[i] > 0 ? day[i] : 0);
            count[i] += (day[i] > 0 ? 1 : 0);
        }
    }

    // Вектор для хранения среднего значения температур
    vector<float> avg(measurements_per_day, 0.0f);

    // Вычисляем среднее значение для каждого времени измерения
    for (int i = 0; i < measurements_per_day; ++i) {
        if (count[i] > 0) {
            avg[i] = sum[i] / count[i]; // Делим сумму на количество положительных значений
        } else {
            avg[i] = 0.0f; // Если не было положительных измерений, записываем 0
        }
    }

    return avg;
}

vector<float> GetRandomVector(int size) {
    static mt19937 engine;
    uniform_real_distribution<float> d(-100, 100);

    vector<float> res(size);
    for (int i = 0; i < size; ++i) {
        res[i] = d(engine);
    }

    return res;
}

int main() {
    vector<vector<float>> data;
    data.reserve(5000);

    for (int i = 0; i < 5000; ++i) {
        data.push_back(GetRandomVector(5000));
    }

    vector<float> avg;
    {
        LOG_DURATION("ComputeAvgTemp"s);
        avg = ComputeAvgTemp(data);
    }

    cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;
}