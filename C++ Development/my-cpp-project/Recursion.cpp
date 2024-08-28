#include <iostream>
#include <cstdint>

using namespace std;

bool IsPowOfTwo(int n) {
    if (n < 1) {
        return false;
    } else if (n == 1) {
        return true;
    } else if (n % 2 == 0) {
        return IsPowOfTwo(n / 2);
    }
    return false;
}

int main() {
    int result = IsPowOfTwo(1024);
    cout << result << endl;
}