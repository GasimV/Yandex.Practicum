#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

template <typename Type>
class StackMin {
public:
    // Push method
    void Push(const Type& element) {
        elements_.push_back(element);
        if (min_elements_.empty() || element <= min_elements_.back()) {
            min_elements_.push_back(element);
        } else {
            min_elements_.push_back(min_elements_.back());
        }
    }
    // Pop method
    void Pop() {
        if (!elements_.empty()) {
            elements_.pop_back();
            min_elements_.pop_back();
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
    // PeekMin method (returns the current minimum) (const version)
    const Type& PeekMin() const {
        return min_elements_.back();
    }
    // PeekMin method (non-const version)
    Type& PeekMin() {
        return min_elements_.back();
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
    vector<Type> elements_; // Main stack to store elements
    vector<Type> min_elements_; // Stack to store current minimum elements

};

int main() {
    StackMin<int> stack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего стека
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);
    // заполняем стек
    for (int i = 0; i < 5; ++i) {
        stack.Push(values[i]);
    }
    // печатаем стек и его минимум, постепенно убирая из стека элементы
    while (!stack.IsEmpty()) {
        stack.Print();
        cout << "Минимум = "s << stack.PeekMin() << endl;
        stack.Pop();
    }
}