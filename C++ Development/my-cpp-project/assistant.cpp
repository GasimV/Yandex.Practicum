#include <iostream>

using namespace std;

int main() {
    int tasks, days;
    cout << "Number of tasks: "s;
    cin >> tasks;
    cout << "Amount of days: "s;
    cin >> days;
    double result =  tasks / days;
    cout << "You need to complete "s << result << " tasks daily"s << endl;
}