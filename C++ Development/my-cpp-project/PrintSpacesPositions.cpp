#include <iostream>
#include <string>

using namespace std;

void PrintSpacesPositions(string& str) {
    // Using "find" method from the "string" to get the first space position
    size_t first_space = str.find(' ');

    // Continue until no more spaces are found
    while (first_space != string::npos) {
        // Print the current space's position
        cout << first_space << endl;

        // Update to the next space, starting search from position after the current space
        first_space = str.find(' ', first_space + 1);
    }
}

int main() {
    string str = "He said: one and one and one is three"s;
    PrintSpacesPositions(str);
    return 0;
}