#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename It>
auto MakeVector(It range_begin, It range_end) {
    return vector(range_begin, range_end);
}

template <typename Container, typename Iterator>
void EraseAndPrint(Container& container, Iterator it) {
    // After erasing an element from the container, the iterator 'it' becomes invalid, so it should be reassigned
    it = container.erase(it); // Erase the element and update 'it'
    PrintRange(container.begin(), it);
    PrintRange(it, container.end());
}

int main() {
    vector<string> langs = {"Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s};
    EraseAndPrint(langs, langs.begin());
    return 0;
}