#include <iostream>
#include <string>

using namespace std;

template <typename F>
string BruteForce(F check) {
    string attempt = "AAAAA";
    for (char c1 = 'A'; c1 <= 'Z'; ++c1) {
        for (char c2 = 'A'; c2 <= 'Z'; ++c2) {
            for (char c3 = 'A'; c3 <= 'Z'; ++c3) {
                for (char c4 = 'A'; c4 <= 'Z'; ++c4) {
                    for (char c5 = 'A'; c5 <= 'Z'; ++c5) {
                        attempt[0] = c1;
                        attempt[1] = c2;
                        attempt[2] = c3;
                        attempt[3] = c4;
                        attempt[4] = c5;
                        if (check(attempt)) {
                            return attempt;
                        }
                    }
                }
            }
        }
    }
    return ""; // In case no valid password is found, which should not happen as per problem constraints.
}

int main() {
    string pass = "ARTUR"s;
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
}