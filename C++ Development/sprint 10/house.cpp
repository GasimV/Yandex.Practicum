#include <cassert>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

class House {
private:
    int length_;
    int width_;
    int height_;

public:
    // Constructor
    House(int length, int width, int height) : length_(length), width_(width), height_(height) {}

    // Getters
    int GetLength() const {
        return length_;
    }

    int GetWidth() const {
        return width_;
    }

    int GetHeight() const {
        return height_;
    }
};

class Resources {
private:
    int brick_count_;

public:
    // Constructor
    explicit Resources(int brick_count) : brick_count_(brick_count) {
        if (brick_count < 0) {
            throw std::invalid_argument("Brick count cannot be negative.");
        }
    }

    // Take a specific number of bricks
    void TakeBricks(int count) {
        if (count < 0) {
            throw std::out_of_range("Cannot take a negative number of bricks.");
        }
        if (count > brick_count_) {
            throw std::out_of_range("Not enough bricks available.");
        }
        brick_count_ -= count;
    }

    // Get the current brick count
    int GetBrickCount() const {
        return brick_count_;
    }
};

struct HouseSpecification {
    int length = 0;
    int width = 0;
    int height = 0;
};

class Builder {
private:
    Resources& resources_;

public:
    // Constructor
    explicit Builder(Resources& resources) : resources_(resources) {}

    // Build a house based on the specification
    House BuildHouse(const HouseSpecification& spec) {
        // Calculate bricks needed for two walls with length and two walls with width
        int bricks_for_length_walls = 2 * spec.length * spec.height * 32; // 4 bricks * 8 rows
        int bricks_for_width_walls = 2 * spec.width * spec.height * 32;

        int total_bricks_needed = bricks_for_length_walls + bricks_for_width_walls;

        if (total_bricks_needed > resources_.GetBrickCount()) {
            throw std::runtime_error("Not enough bricks to build the house.");
        }

        resources_.TakeBricks(total_bricks_needed);
        return House(spec.length, spec.width, spec.height);
    }
};

int main() {
    Resources resources{10000};
    Builder builder1{resources};
    Builder builder2{resources};

    House house1 = builder1.BuildHouse(HouseSpecification{12, 9, 3});
    assert(house1.GetLength() == 12);
    assert(house1.GetWidth() == 9);
    assert(house1.GetHeight() == 3);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House house2 = builder2.BuildHouse(HouseSpecification{8, 6, 3});
    assert(house2.GetLength() == 8);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House banya = builder1.BuildHouse(HouseSpecification{4, 3, 2});
    assert(banya.GetHeight() == 2);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
}