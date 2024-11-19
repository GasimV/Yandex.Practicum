#include <iostream>
#include <string>
#include <string_view>
#include <algorithm>

using namespace std;


// clang-format off

const int NUM_PLANETS = 9;
const string_view PLANETS[] = {
    "Mercury"sv, "Venus"sv, "Earth"sv,
    "Mars"sv, "Jupiter"sv, "Saturn"sv,
    "Uranus"sv, "Neptune"sv,
};

// clang-format on

bool IsPlanet(string_view name) {
    return any_of(begin(PLANETS), end(PLANETS), [&](string_view planet) {
        return planet == name;
    });
}

int main() {
    string input;
    getline(cin, input); // Считываем строку из стандартного ввода

    if (IsPlanet(input)) {
        cout << input << " is a planet" << endl;
    } else {
        cout << input << " is NOT a planet" << endl;
    }
}