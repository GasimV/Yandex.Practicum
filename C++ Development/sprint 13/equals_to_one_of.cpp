#include <cassert>
#include <string>
#include <string_view>

using namespace std::literals;

// Базовый случай: если передан только один аргумент, возвращаем false
template <typename T>
bool EqualsToOneOf(const T&) {
    return false;
}

// Рекурсивный шаблон, сравнивающий первый аргумент с остальными
template <typename T, typename... Args>
bool EqualsToOneOf(const T& first, const Args&... rest) {
    return ((first == rest) || ...);
}

int main() {
    assert(EqualsToOneOf("hello"sv, "hi"s, "hello"s));
    assert(!EqualsToOneOf(1, 10, 2, 3, 6));
    assert(!EqualsToOneOf(8));
}