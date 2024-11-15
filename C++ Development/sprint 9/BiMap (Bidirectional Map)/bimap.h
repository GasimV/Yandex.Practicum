#pragma once

#include <optional>
#include <string_view>
#include <deque>
#include <string>
#include <unordered_map>
#include <memory>

class BiMap {
public:
    BiMap();
    ~BiMap();

    // Копирующий и перемещающий конструкторы
    BiMap(const BiMap& other);
    BiMap(BiMap&& other) noexcept;

    // Оператор копирующего и перемещающего присваивания
    BiMap& operator=(const BiMap& other);
    BiMap& operator=(BiMap&& other) noexcept;

    bool Add(std::string_view key, std::string_view value);

    // Возвращает значение, связанное с ключом, либо nullopt, если такого ключа нет
    std::optional<std::string_view> FindValue(std::string_view key) const noexcept;

    // Возвращает ключ, связанный с value, либо nullopt, если такого значения нет
    std::optional<std::string_view> FindKey(std::string_view value) const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> Pimpl; // Указатель на реализацию структуры Pimpl
};