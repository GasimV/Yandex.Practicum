#include <chrono>
#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;
using namespace std::chrono;

vector<int> ReverseVector(const vector<int>& source_vector) {
    vector<int> res;
    for (int i : source_vector) {
        res.insert(res.begin(), i);
    }
    return res;
}

int CountPops(const vector<int>& source_vector, int begin, int end) {
    int res = 0;
    for (int i = begin; i < end; ++i) {
        if (source_vector[i]) {
            ++res;
        }
    }
    return res;
}

void AppendRandom(vector<int>& v, int n) {
    for (int i = 0; i < n; ++i) {
        v.push_back(rand() % 2);
    }
}

int main() {
    static const int N = 1 << 17;
    vector<int> random_bits;

    // Timing AppendRandom
    auto start = high_resolution_clock::now();
    AppendRandom(random_bits, N);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by AppendRandom: " << duration.count() << "ms" << endl;

    // Timing ReverseVector
    start = high_resolution_clock::now();
    vector<int> reversed_bits = ReverseVector(random_bits);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by ReverseVector: " << duration.count() << "ms" << endl;

    // Timing CountPops
    start = high_resolution_clock::now();
    int pops = CountPops(reversed_bits, 0, N);
    cout << "Number of pops: " << pops << endl;

    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by CountPops: " << duration.count() << "ms" << endl;

    return 0;
}