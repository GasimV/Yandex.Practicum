#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

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
        // получаем случайное число с помощью функции rand.
        // с помощью (rand() % 2) получим целое число в диапазоне 0..1.
        // в C++ имеются более современные генераторы случайных чисел,
        // но в данном уроке не будем их касаться
        v.push_back(rand() % 2);
    }
}

void Operate() {
    vector<int> random_bits;

    // операция << для целых чисел это сдвиг всех бит в двоичной
    // записи числа. Запишем с её помощью число 2 в степени 17 (131072)
    static const int N = 1 << 17;

    // Измеряем время AppendRandom
    auto start_random = high_resolution_clock::now();
    // заполним вектор случайными числами 0 и 1
    AppendRandom(random_bits, N);
    auto stop_random = high_resolution_clock::now();
    auto duration_random = duration_cast<milliseconds>(stop_random - start_random);
    cerr << "Append random: " << duration_random.count() << " ms" << endl;

    // Измеряем время ReverseVector
    auto start_reverse = high_resolution_clock::now();
    // перевернём вектор задом наперёд
    vector<int> reversed_bits = ReverseVector(random_bits);
    auto stop_reverse = high_resolution_clock::now();
    auto duration_reverse = duration_cast<milliseconds>(stop_reverse - start_reverse);
    cerr << "Reverse: " << duration_reverse.count() << " ms" << endl;

    // Измеряем время CountPops
    auto start_pops = high_resolution_clock::now();
    int pops = CountPops(reversed_bits, 0, N);
    auto stop_pops = high_resolution_clock::now();
    auto duration_pops = duration_cast<milliseconds>(stop_pops - start_pops);
    (void)pops;
    cerr << "Counting: " << duration_pops.count() << " ms" << endl;

    // посчитаем процент единиц на начальных отрезках вектора
    for (int i = 1, step = 1; i <= N; i += step, step *= 2) {
        // чтобы вычислить проценты, мы умножаем на литерал 100. типа double;
        // целочисленное значение функции CountPops при этом автоматически
        // преобразуется к double, как и i
        double rate = CountPops(reversed_bits, 0, i) * 100. / i;
        cout << "After "s << i << " bits we found "s << rate << "% pops"s << endl;
    }
}

int main() {
    Operate();
    return 0;
}