#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std;

// функция, записывающая элементы диапазона в строку
template <typename It>
string PrintRangeToString(It range_begin, It range_end) {
    // удобный тип ostringstream -> https://ru.cppreference.com/w/cpp/io/basic_ostringstream
    ostringstream out;
    for (auto it = range_begin; it != range_end; ++it) {
        out << *it << " "s;
    }
    out << endl;
    // получаем доступ к строке с помощью метода str для ostringstream
    return out.str();
}

template <typename It>
vector<string> GetPermutations(It begin_iterator, It end_iterator) {
    vector<string> permutations;

    // Sort the range to start with the first lexicographical permutation
    sort(begin_iterator, end_iterator);

    // Capture the first permutation
    permutations.push_back(PrintRangeToString(begin_iterator, end_iterator));

    // Capture subsequent permutations
    while (next_permutation(begin_iterator, end_iterator)) {
        string s = PrintRangeToString(begin_iterator, end_iterator);
        permutations.push_back(s);
    }

    return permutations;
}

int main() {
    vector<int> permutation(3);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(permutation.begin(), permutation.end(), 1);
    auto result = GetPermutations(permutation.begin(), permutation.end());
    for (const auto& s : result) {
        cout << s;
    }
    return 0;
}