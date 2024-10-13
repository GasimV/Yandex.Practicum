#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>
#include <cstdint>
#include <list>

using namespace std;

template <typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
    list<typename RandomIt::value_type> pool;
    for (auto it = first; it != last; ++it) {
        pool.push_back(std::move(*it));
    }
    auto cur_pos = pool.begin();
    while (!pool.empty()) {
        *(first++) = std::move(*cur_pos);
        cur_pos = pool.erase(cur_pos); // Удаляем текущий элемент и получаем следующий
        if (pool.empty()) {
            break;
        }
        // Двигаем позицию на step_size - 1 шагов вперёд
        if (cur_pos == pool.end()) {
            cur_pos = pool.begin(); // Возвращаемся в начало, если вышли за пределы
        }
        for (uint32_t i = 0; i < step_size - 1; ++i) {
            ++cur_pos;
            if (cur_pos == pool.end()) {
                cur_pos = pool.begin(); // Циклически обрабатываем элементы
            }
        }
    }
}

vector<int> MakeTestVector() {
    vector<int> numbers(10);
    iota(begin(numbers), end(numbers), 0);
    return numbers;
}

void TestIntVector() {
    const vector<int> numbers = MakeTestVector();
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
        assert(numbers_copy == numbers);
    }
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
        assert(numbers_copy == vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
    }
}

// Это специальный тип, который поможет вам убедиться, что ваша реализация
// функции MakeJosephusPermutation не выполняет копирование объектов.
// Сейчас вы, возможно, не понимаете как он устроен, однако мы расскажем
// об этом далее в нашем курсе

struct NoncopyableInt {
    int value;

    NoncopyableInt(const NoncopyableInt&) = delete;
    NoncopyableInt& operator=(const NoncopyableInt&) = delete;

    NoncopyableInt(NoncopyableInt&&) = default;
    NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator==(const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const NoncopyableInt& v) {
    return os << v.value;
}

void TestAvoidsCopying() {
    vector<NoncopyableInt> numbers;
    numbers.push_back({1});
    numbers.push_back({2});
    numbers.push_back({3});
    numbers.push_back({4});
    numbers.push_back({5});

    MakeJosephusPermutation(begin(numbers), end(numbers), 2);

    vector<NoncopyableInt> expected;
    expected.push_back({1});
    expected.push_back({3});
    expected.push_back({5});
    expected.push_back({4});
    expected.push_back({2});

    assert(numbers == expected);
}

int main() {
    TestIntVector();
    TestAvoidsCopying();
    return 0;
}