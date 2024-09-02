#include <iostream>

using namespace std;

void Hen();

void Egg(int x) {
    if (1 == x) {
        cout << "Яйцо вызывает курицу"s << endl;
        Hen();
    } else {
        cout << "А вот и цыплёнок!"s << endl;
    }
}

void Hen() {
    cout << "Курица вызывает яйцо"s << endl;
    Egg(0);
}

int main() {
    Egg(1);
}