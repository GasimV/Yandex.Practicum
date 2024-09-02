#include "builder.h"
#include <iostream>

using namespace std;

int main() {
    Builder tom;
    // Попросите Тома посчитать кирпичи для стены 3.5 х 2.45
    Wall wall(3.5, 2.45); // Create a Wall object with the given width and height
    const auto num_of_bricks = tom.CalcBricksNeeded(wall);
    cout << num_of_bricks;
}