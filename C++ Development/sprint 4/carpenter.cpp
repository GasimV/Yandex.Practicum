#include "carpenter.h"
#include "square_calculation.h"
#include <iostream>

using namespace std;

int Carpenter::CalcShelves(const Wall& wall) {
    double area = CalcSquare(wall.GetHeight(), wall.GetWidth());
    return static_cast<int>(area / 2.0); // 1 shelf per 2 square meters
}

int main() {
    Carpenter jack;
    Wall wall(3.5, 2.45);
    int shelves = jack.CalcShelves(wall);
    cout << shelves;
}