#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

// функция возвращает true, если векторы из одинаковых элементов
// перепишите эту функцию, улучшив её асимптотическую сложность
bool TestPermut(const vector<int>& v1, const vector<int>& v2) {
    // Если они разной длины, элементы заведомо разные
    if (v1.size() != v2.size()) {
        return false;
    }

    // Создаем копии векторов
    vector<int> sorted_v1 = v1;
    vector<int> sorted_v2 = v2;

    // Сортируем оба вектора
    sort(sorted_v1.begin(), sorted_v1.end());
    sort(sorted_v2.begin(), sorted_v2.end());

    // Сравниваем отсортированные векторы
    return sorted_v1 == sorted_v2;
}

int main() {
    std::mt19937 g;

    int n;
    cin >> n;
    vector<int> v1, v2;
    v1.reserve(n);
    v2.reserve(n);

    for (int i = 0; i < n; ++i) {
        v1.push_back(rand());
        v2.push_back(rand());
    }

    // оба вектора случайны, вряд ли они совпадут
    cout << "Random vectors match? "s << flush;
    cout << (TestPermut(v1, v2) ? "Yes"s : "No"s) << endl;

    // делаем один перестановкой другого явным образом
    v2 = v1;
    shuffle(v2.begin(), v2.end(), g);
    cout << "Permuted vectors match? "s << flush;
    cout << (TestPermut(v1, v2) ? "Yes"s : "No"s) << endl;
}