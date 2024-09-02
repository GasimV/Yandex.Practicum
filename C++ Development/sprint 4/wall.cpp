#include "wall.h"

// Constructor implementation
Wall::Wall(double width, double height)
    : width_(width)
    , height_(height)
    , color_(Color::WHITE) {
}

// Method to get the height of the wall
double Wall::GetHeight() const {
    return height_;
}

// Method to get the width of the wall
double Wall::GetWidth() const {
    return width_;
}

// Method to set the color of the wall
void Wall::SetColor(Color color) {
    color_ = color;
}

// Method to get the current color of the wall
Wall::Color Wall::GetColor() const {
    return color_;
}

int main() {
    Wall wall(3.5, 2.45);
    wall.SetColor(Wall::Color::BLUE);
    return 0;
}