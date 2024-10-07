#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <chrono>

// Функция для сортировки вектора
void sortVector(size_t size) {
    std::vector<int> vec;
    for (size_t i = 0; i < size; ++i) {
        vec.push_back(rand()); // Вставляем случайный элемент
    }
    std::sort(vec.begin(), vec.end()); // Сортировка вектора
}

// Функция для сортировки дека
void sortDeque(size_t size) {
    std::deque<int> deq;
    for (size_t i = 0; i < size; ++i) {
        deq.push_back(rand()); // Вставляем случайный элемент
    }
    std::sort(deq.begin(), deq.end()); // Сортировка дека
}

int main() {
    // Количество элементов для сортировки
    size_t one_million = 1000000;
    size_t five_million = 5000000;

    // Сортировка вектора с миллионом элементов
    auto start = std::chrono::high_resolution_clock::now();
    sortVector(one_million);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Vector (1M elements) sort time: " << elapsed.count() << " seconds\n";

    // Сортировка дека с миллионом элементов
    start = std::chrono::high_resolution_clock::now();
    sortDeque(one_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Deque (1M elements) sort time: " << elapsed.count() << " seconds\n";

    // Сортировка вектора с пятью миллионами элементов
    start = std::chrono::high_resolution_clock::now();
    sortVector(five_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Vector (5M elements) sort time: " << elapsed.count() << " seconds\n";

    // Сортировка дека с пятью миллионами элементов
    start = std::chrono::high_resolution_clock::now();
    sortDeque(five_million);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Deque (5M elements) sort time: " << elapsed.count() << " seconds\n";

    return 0;
}