#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <chrono>

// Функция для заполнения вектора
void fillVector(size_t size) {
    std::vector<int> vec;
    for (size_t i = 0; i < size; ++i) {
        vec.push_back(rand()); // Вставляем случайный элемент
    }
}

// Функция для заполнения дека
void fillDeque(size_t size) {
    std::deque<int> deq;
    for (size_t i = 0; i < size; ++i) {
        deq.push_back(rand()); // Вставляем случайный элемент
    }
}

int main() {
    // Количество элементов для вставки
    size_t one_million = 1000000;
    size_t five_million = 5000000;

    // Запускаем профилирование с миллионом элементов для вектора
    auto start = std::chrono::high_resolution_clock::now();
    fillVector(one_million);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Vector (1M elements) fill time: " << elapsed.count() << " seconds\n";

    // Запускаем профилирование с миллионом элементов для дека
    start = std::chrono::high_resolution_clock::now();
    fillDeque(one_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Deque (1M elements) fill time: " << elapsed.count() << " seconds\n";

    // Запускаем профилирование с пятью миллионами элементов для вектора
    start = std::chrono::high_resolution_clock::now();
    fillVector(five_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Vector (5M elements) fill time: " << elapsed.count() << " seconds\n";

    // Запускаем профилирование с пятью миллионами элементов для дека
    start = std::chrono::high_resolution_clock::now();
    fillDeque(five_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Deque (5M elements) fill time: " << elapsed.count() << " seconds\n";

    return 0;
}