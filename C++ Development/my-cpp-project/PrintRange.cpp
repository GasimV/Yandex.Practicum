#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (It i = range_begin; i != range_end; ++i) {
        cout << *i << " "s; // Dereference the iterator to get the value and print it
    }
    cout << endl; // Print a newline after all elements are printed
}

// Template function to find an element in a container and print the elements before and after it.
template <typename Container, typename Element>
void FindAndPrint(Container c, Element e) {
    // 'it' will be an iterator pointing to the found element; if not, 'it' will be equal to c.end().
    auto it = find(c.begin(), c.end(), e);
    // The std::find function returns an iterator, not a boolean value. 
    // We should compare the iterator with c.end() to check if the element was found or not.
    if (it == c.end()) { // if the element is NOT found
        PrintRange(c.begin(), c.end()); // Print the entire container
        PrintRange(c.end(), c.end()); // Print an empty line (from c.end() to c.end())
    } else {
        PrintRange(c.begin(), it); // Print elements before the found element (not including 'it')
        PrintRange(it, c.end()); // Print the found element and all elements after it
    }
    
}

int main() {
    cout << "Test1"s << endl;
    set<int> test1 = {1, 1, 1, 2, 3, 4, 5, 5};
    FindAndPrint(test1, 3); // Find and print elements around the number 3 in 'test1'

    cout << "Test2"s << endl;
    vector<int> test2 = {}; // пустой контейнер
    FindAndPrint(test2, 0); // Try to find the number 0 in 'test2' (which is empty)
    cout << "End of tests"s << endl;
}