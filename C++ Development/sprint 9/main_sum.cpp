#include <cstdint>
#include <iostream>
#include <limits>

using namespace std;

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    
    // Определяем максимальное и минимальное значение для int64_t
    const int64_t MAX = numeric_limits<int64_t>::max();
    const int64_t MIN = numeric_limits<int64_t>::min();

    // Проверяем переполнение
    if ((b > 0 && a > MAX - b) || (b < 0 && a < MIN - b)) {
        cout << "Overflow!" << endl;
    } else {
            cout << a + b << endl;
    }
}