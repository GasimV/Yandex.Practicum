#include <cstdint>
#include <iostream>

using namespace std;

// Линейный алгоритм для нахождения чисел трибоначчи
int64_t T(int i) {
    if (i <= 1) {
        return 0;
    }
    if (i == 2) {
        return 1;
    }

    int64_t a = 0, b = 0, c = 1; // Три последних числа трибоначчи
    int64_t result = 0;

    for (int j = 3; j <= i; ++j) {
        result = a + b + c; // Следующее число трибоначчи
        a = b; // Сдвигаем числа на один шаг
        b = c;
        c = result;
    }

    return result;
}

int main() {
    int i;

    while (true) {
        cout << "Enter index: "s;
        if (!(cin >> i)) {
            break;
        }

        cout << "Ti = "s << T(i) << endl;
    }
}