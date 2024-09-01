#include <stack>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Queue {
public:
    // Push method to add elements to the queue
    void Push(const Type& element) {
        stack_in.push(element);
    }
    // Pop method to remove the front element
    void Pop() {
        if (!stack_out.empty()) {
            stack_out.pop();
        } else if (!stack_in.empty()) {
            while (!stack_in.empty())
            {
                stack_out.push(stack_in.top());
                stack_in.pop();
            }
            stack_out.pop();
        } else {
            throw runtime_error("Error: Pop on empty queue");
        }
    }
    // Front method to get the front element of the queue
    Type& Front() {
        if (!stack_out.empty()) {
            return stack_out.top();
        } else if (!stack_in.empty()) {
            while (!stack_in.empty()) {
                stack_out.push(stack_in.top());
                stack_in.pop();
            }
            return stack_out.top();
        } else {
            throw runtime_error("Error: Front on empty queue");
        }
    }
    // Size method to get the size of the queue
    uint64_t Size() const {
        return stack_in.size() + stack_out.size();
    }
    // IsEmpty method to check if the queue is empty
    bool IsEmpty() const {
        return stack_in.empty() && stack_out.empty();
    }

private:
    // Initialize two stacks to simulate the queue
    stack<Type> stack_in;
    stack<Type> stack_out;
};

int main() {
    Queue<int> queue;
    vector<int> values(5);
    // заполняем вектор для тестирования очереди
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_device rd;
    mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);
    PrintRange(values.begin(), values.end());
    cout << "Заполняем очередь"s << endl;
    // заполняем очередь и выводим элемент в начале очереди
    for (int i = 0; i < 5; ++i) {
        queue.Push(values[i]);
        cout << "Вставленный элемент "s << values[i] << endl;
        cout << "Первый элемент очереди "s << queue.Front() << endl;
    }
    cout << "Вынимаем элементы из очереди"s << endl;
    // выводим элемент в начале очереди и вытаскиваем элементы по одному
    while (!queue.IsEmpty()) {
        // сначала будем проверять начальный элемент, а потом вытаскивать,
        // так как операция Front на пустой очереди не определена
        cout << "Будем вынимать элемент "s << queue.Front() << endl;
        queue.Pop();
    }
    return 0;
}