#include <iostream>
#include <list>
#include <set>
#include <string_view>
#include <vector>
#include <iterator>
#include <functional>

using namespace std;

// Универсальная функция Merge, которая работает с любыми контейнерами через итераторы
template <typename InputIt1, typename InputIt2>
void Merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, ostream& out) {
    while (first1 != last1 && first2 != last2) {
        if (std::less<>()(*first1, *first2)) {
            out << *first1 << endl;
            ++first1;
        } else {
            out << *first2 << endl;
            ++first2;
        }
    }
    while (first1 != last1) {
        out << *first1 << endl;
        ++first1;
    }
    while (first2 != last2) {
        out << *first2 << endl;
        ++first2;
    }
}

// Универсальная шаблонная функция MergeSomething для любых контейнеров
template <typename Container1, typename Container2>
void MergeSomething(const Container1& src1, const Container2& src2, ostream& out) {
    Merge(begin(src1), end(src1), begin(src2), end(src2), out);
}

// Функция MergeHalves, объединяющая две половины одного вектора
template <typename T>
void MergeHalves(const vector<T>& src, ostream& out) {
    size_t mid = src.size() / 2;
    Merge(begin(src), begin(src) + mid, begin(src) + mid, end(src), out);
}

int main() {
    vector<int> v1{60, 70, 80, 90};
    vector<int> v2{65, 75, 85, 95};
    vector<int> combined{60, 70, 80, 90, 65, 75, 85, 95};
    list<double> my_list{0.1, 72.5, 82.11, 1e+30};
    string_view my_string = "ACNZ"sv;
    set<unsigned> my_set{20u, 77u, 81u};

    // пока функция MergeSomething реализована только для векторов
    cout << "Merging vectors:"sv << endl;
    MergeSomething(v1, v2, cout);

    cout << "Merging vector and list:"sv << endl;
    MergeSomething(v1, my_list, cout);

    cout << "Merging string and list:"sv << endl;
    MergeSomething(my_string, my_list, cout);

    cout << "Merging set and vector:"sv << endl;
    MergeSomething(my_set, v2, cout);

    cout << "Merging vector halves:"sv << endl;
    MergeHalves(combined, cout);

    return 0;
}