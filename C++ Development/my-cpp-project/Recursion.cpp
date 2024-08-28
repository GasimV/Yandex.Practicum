#include <iostream>
#include <cstdint>

using namespace std;

uint64_t Fibonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    }
    uint64_t a = Fibonacci(n-1);
    uint64_t b = Fibonacci(n-2);
    return a + b;
}

int main() {
    cout << Fibonacci(6) << endl;
}