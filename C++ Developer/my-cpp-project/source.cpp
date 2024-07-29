#include <iostream>
#include <string>

using namespace std;

int main() {
    cout << "What is your name?" << endl;
    string name;

    getline(cin, name);

    cout << "Hello, " << name << endl;
}