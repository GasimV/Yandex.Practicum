#include "bimap.h"

struct BiMap::Impl {
    std::deque<std::string> storage; // Для хранения строк
    std::unordered_map<std::string_view, std::string_view> key_to_value; // Быстрый поиск значения по ключу
    std::unordered_map<std::string_view, std::string_view> value_to_key; // Быстрый поиск ключа по значению

    Impl() = default;

    // Копирующий конструктор
    Impl(const Impl& other) : storage(other.storage) {
        for (const auto& str : other.key_to_value) {
            auto key = str.first;
            auto value = str.second;
            key_to_value[key] = value;
            value_to_key[value] = key;
        }
    }

    // Оператор присваивания с копированием
    Impl& operator=(const Impl& other) {
        if (this != &other) {
            Impl tmp(other);
            *this = std::move(tmp);
        }
        return *this;
    }

    // Перемещающий конструктор и перемещающий оператор присваивания по умолчанию
    Impl(Impl&&) noexcept = default;
    Impl& operator=(Impl&&) noexcept = default;
};

BiMap::BiMap() : Pimpl(std::make_unique<Impl>()) {}

BiMap::~BiMap() = default;

// Конструктор копирования
BiMap::BiMap(const BiMap& other) : Pimpl(std::make_unique<Impl>(*other.Pimpl)) {}

// Конструктор перемещения
BiMap::BiMap(BiMap&& other) noexcept = default;

// Оператор копирующего присваивания
BiMap& BiMap::operator=(const BiMap& other) {
    if (this != &other) {
        Pimpl = std::make_unique<Impl>(*other.Pimpl);
    }
    return *this;
}

// Оператор перемещающего присваивания
BiMap& BiMap::operator=(BiMap&& other) noexcept = default;

bool BiMap::Add(std::string_view key, std::string_view value) {
    // Проверка наличия ключа или значения
    if (Pimpl->key_to_value.count(key) > 0 || Pimpl->value_to_key.count(value) > 0) {
        return false;
    }

    try {
        // Сохранить строки в storage, чтобы string_view оставались валидными
        Pimpl->storage.emplace_back(key);
        Pimpl->storage.emplace_back(value);

        // Получить string_view из storage
        std::string_view key_view = Pimpl->storage[Pimpl->storage.size() - 2];
        std::string_view value_view = Pimpl->storage.back();

        // Добавить в карты для двунаправленного поиска
        Pimpl->key_to_value[key_view] = value_view;
        Pimpl->value_to_key[value_view] = key_view;
    } catch(...) {
        // При исключении отменяем частично внесенные изменения
        Pimpl->storage.pop_back();
        Pimpl->storage.pop_back();
        throw;
    }
    return true;
}

std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    auto it = Pimpl->key_to_value.find(key);
    if (it != Pimpl->key_to_value.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    auto it = Pimpl->value_to_key.find(value);
    if (it != Pimpl->value_to_key.end()) {
        return it->second;
    }
    return std::nullopt;
}