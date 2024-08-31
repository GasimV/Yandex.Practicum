#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        // Method to push an element onto the stack
        elements_.push_back(element);
    }
    void Pop() {
        // Method to pop an element from the stack
        if (!elements_.empty()) {
            elements_.pop_back();
        } else {
            throw runtime_error("Error: Pop on empty stack");
        }
    }
    const Type& Peek() const {
        // Method to peek at the top element (const version)
        return elements_.back();
    }
    Type& Peek() {
        // Method to peek at the top element (non-const version)
        return elements_.back();
    }
    void Print() const {
        // Method to print all elements in the stack
        for (const auto& element : elements_) {
            cout << element << ' ';
        }
        cout << endl;
    }
    uint64_t Size() const {
        // Method to get the size of the stack
        return elements_.size();
    }
    bool IsEmpty() const {
        // Method to check if the stack is empty
        if (elements_.empty()) {
            return true;
        }
        return false;
    }

private:
    vector<Type> elements_; // Vector to store stack elements
};

int main() {
    Stack<int> stack;
    for (uint32_t i = 0; i < 10; ++i) {
        stack.Push(i);
        stack.Print();
    }
    while (!stack.IsEmpty()) {
        stack.Pop();
        stack.Print();
    }
}