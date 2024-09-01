#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

template <typename Type>
class SortedSack {
public:
    void Push(const Type& element) {
        // Find the correct position to insert the element
        auto pos = find_if(elements_.rbegin(), elements_.rend(), [&](const Type& el) {
            return el >= element;
        }).base();

        // Insert the element at the found position
        elements_.insert(pos, element);
    }

    void Pop() {
        if (!elements_.empty()) {
            elements_.pop_back();
        } else {
            throw runtime_error("Error: Pop on empty sack");
        }
    }

    const Type& Peek() const {
        if (!elements_.empty()) {
            return elements_.back();
        } else {
            throw runtime_error("Error: Peek on empty sack");
        }
    }

    Type& Peek() {
        if (!elements_.empty()) {
            return elements_.back();
        } else {
            throw runtime_error("Error: Peek on empty sack");
        }
    }

    void Print() const {
        for (const auto& element : elements_) {
            cout << element << ' ';
        }
        cout << endl;
    }

    uint64_t Size() const {
        return elements_.size();
    }

    bool IsEmpty() const {
        return elements_.empty();
    }

private:
    vector<Type> elements_;  // Vector to store elements in sorted order
};

int main() {
    SortedSack<int> sack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего класса
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    // заполняем класс и проверяем, что элементы добавляются правильно
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        sack.Push(values[i]);
        sack.Print();
    }
}