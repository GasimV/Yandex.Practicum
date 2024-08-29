#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <random>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename T>
void Merge(T range_begin, T middle, T range_end) {
    // Ensure that the temporary vector holds elements of the correct type pointed to by the iterators.
    using ValueType = typename iterator_traits<T>::value_type;
    vector<ValueType> temp;

    auto left_it = range_begin;
    auto right_it = middle;

    while (left_it != middle && right_it != range_end) {
        if (*left_it < *right_it) {
            temp.push_back(*left_it);
            ++left_it;
        } else {
            temp.push_back(*right_it);
            ++right_it;
        }
    }
    // Append any remaining elements from the left side
    while (left_it != middle) {
        temp.push_back(*left_it);
        ++left_it;
    }

    // Append any remaining elements from the right side
    while (right_it != range_end) {
        temp.push_back(*right_it);
        ++right_it;
    }
    
    // Copy the merged elements back to the original range
    copy(temp.begin(), temp.end(), range_begin);  
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    if (range_end - range_begin < 2) {
        return; // Base case: if the range contains fewer than 2 elements, it's already sorted
    }
    // Find the middle point to split the range
    RandomIt middle = range_begin + (range_end - range_begin) / 2;
    
    MergeSort(range_begin, middle); // Recursively sort the first half
    MergeSort(middle, range_end); // Recursively sort the second half

    Merge(range_begin, middle, range_end); // Merge the two sorted halves
}

int main() {
    vector<int> test_vector(10);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_device rd;
    mt19937 g(rd());
    shuffle(test_vector.begin(), test_vector.end(), g);
    
    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());
    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());
    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());
    return 0;
}