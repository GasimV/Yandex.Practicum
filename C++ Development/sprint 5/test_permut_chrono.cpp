#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;
using namespace std::chrono;

bool TestPermut(const vector<int>& v1, const vector<int>& v2) {
    if (v1.size() != v2.size()) {
        return false;
    }
    vector<int> sorted_v1 = v1;
    vector<int> sorted_v2 = v2;
    sort(sorted_v1.begin(), sorted_v1.end());
    sort(sorted_v2.begin(), sorted_v2.end());
    return sorted_v1 == sorted_v2;
}

int main() {
    mt19937 g;
    int n = 100000; // большой размер вектора
    vector<int> v1, v2;
    v1.reserve(n);
    v2.reserve(n);

    for (int i = 0; i < n; ++i) {
        v1.push_back(rand());
        v2.push_back(rand());
    }

    // Измеряем время работы оптимизированной функции
    auto start = high_resolution_clock::now();
    bool result = TestPermut(v1, v2);
    auto end = high_resolution_clock::now();

    cout << "Time taken: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
    cout << "Random vectors match? "s << (result ? "Yes"s : "No"s) << endl;

    return 0;
}