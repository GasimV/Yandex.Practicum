#include <cassert>
#include <vector>

template <typename T>
void ReverseArray(T* start, size_t size) {
    if (start == nullptr || size == 0) {
        return; // Если массив пустой или размер 0, ничего не делаем
    }

    T* left = start; // Указатель на начало массива
    T* right = start + size - 1; // Указатель на конец массива

    // Пока указатели не пересекутся
    while (left < right) {
        // Меняем местами элементы
        T temp = *left;
        *left = *right;
        *right = temp;
        // Сдвигаем указатели
        ++left;
        --right;
    }
}

int main() {
    using namespace std;
    
    vector<int> v = {1, 2};
    ReverseArray(v.data(), v.size());
    assert(v == (vector<int>{2, 1}));

    // Тест с массивом из 1 элемента
    vector<int> v2 = {1};
    ReverseArray(v2.data(), v2.size());
    assert(v2 == (vector<int>{1}));

    // Тест с пустым массивом
    vector<int> v3;
    ReverseArray(v3.data(), v3.size());
    assert(v3 == vector<int>{});

    // Тест с массивом из чётного числа элементов
    vector<int> v4 = {1, 2, 3, 4};
    ReverseArray(v4.data(), v4.size());
    assert(v4 == (vector<int>{4, 3, 2, 1}));

    // Тест с массивом из нечётного числа элементов
    vector<int> v5 = {1, 2, 3, 4, 5};
    ReverseArray(v5.data(), v5.size());
    assert(v5 == (vector<int>{5, 4, 3, 2, 1}));
}