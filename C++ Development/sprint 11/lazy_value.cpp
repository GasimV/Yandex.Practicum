#include <cassert>
#include <functional>
#include <optional>
#include <string>
#include <mutex>

using namespace std;

template <typename T>
class LazyValue {
public:
    explicit LazyValue(function<T()> init) : initializer(move(init)), value(), mutex_() {}

    // Проверяет, был ли создан объект
    bool HasValue() const {
        lock_guard<mutex> lock(mutex_);
        return value.has_value();
    }

    // Возвращает ссылку на объект, создавая его при первом вызове
    const T& Get() const {
        lock_guard<mutex> lock(mutex_);
        if (!value.has_value()) {
            value = initializer();
        }
        return *value;
    }

private:
    function<T()> initializer; // Функция инициализации
    mutable optional<T> value; // Хранимое значение (опционально)
    mutable mutex mutex_; // Мьютекс для обеспечения потокобезопасности
};

void UseExample() {
    const string big_string = "Giant amounts of memory"s;

    LazyValue<string> lazy_string([&big_string] {
        return big_string;
    });

    assert(!lazy_string.HasValue());
    assert(lazy_string.Get() == big_string);
    assert(lazy_string.Get() == big_string);
}

void TestInitializerIsntCalled() {
    bool called = false;

    {
        LazyValue<int> lazy_int([&called] {
            called = true;
            return 0;
        });
    }
    assert(!called);
}

int main() {
    UseExample();
    TestInitializerIsntCalled();
}