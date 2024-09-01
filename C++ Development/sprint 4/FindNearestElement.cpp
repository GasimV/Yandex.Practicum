#include <iostream>
#include <set>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    // set<int>::const_iterator — тип итераторов для константного множества целых чисел
    if (numbers.empty()) {
        return numbers.end(); // No elements to compare, return end
    }

    auto it = numbers.lower_bound(border);
    
    if (it == numbers.begin()) {
        return it; // No smaller element exists, so this is the nearest
    }

    if (it == numbers.end()) {
        // All elements are smaller, so return the last element 
        // "prev(numbers.end())" because all elements in the set are smaller than border.
        return prev(numbers.end());
    }

    // Move one step back to compare with the previous element
    auto prev_it = prev(it, 1);

    if (abs(*it - border) < abs(*prev_it - border)) {
        return it; // The current element is closer
    }
    return prev_it; // The previous element is closer
}

int main() {
    set<int> numbers = {1, 4, 6};
    cout << *FindNearestElement(numbers, 0) << " " << *FindNearestElement(numbers, 3) << " "
         << *FindNearestElement(numbers, 5) << " " << *FindNearestElement(numbers, 6) << " "
         << *FindNearestElement(numbers, 100) << endl;
    set<int> empty_set;
    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
    return 0;
}