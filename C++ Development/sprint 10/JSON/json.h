#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    // Конструкторы для инициализации Node с различными типами
    Node() : value_(nullptr) {};
    Node(Array array) : value_(std::move(array)) {}
    Node(Dict map) : value_(std::move(map)) {}
    Node(int value) : value_(value) {}
    Node(double value) : value_(value) {}
    Node(std::string value) : value_(std::move(value)) {}
    Node(bool value) : value_(value) {}
    Node(std::nullptr_t) : value_(nullptr) {}

    // Методы проверки типов для определения типа хранимого значения
    bool IsInt() const { return std::holds_alternative<int>(value_); }
    bool IsDouble() const { return std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_); }
    bool IsPureDouble() const { return std::holds_alternative<double>(value_); }
    bool IsBool() const { return std::holds_alternative<bool>(value_); }
    bool IsString() const { return std::holds_alternative<std::string>(value_); }
    bool IsNull() const { return std::holds_alternative<std::nullptr_t>(value_); }
    bool IsArray() const { return std::holds_alternative<Array>(value_); }
    bool IsMap() const { return std::holds_alternative<Dict>(value_); }

    // Методы для получения значения, хранящегося в Node
    // Выдает std::logic_error, если тип значения не совпадает
    int AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Not an int");
        }
        return std::get<int>(value_);
    }

    double AsDouble() const {
        if (IsInt()) {
            return static_cast<double>(std::get<int>(value_));
        } else if (IsPureDouble()) {
            return std::get<double>(value_);
        }
        throw std::logic_error("Not a double");
    }

    bool AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Not a bool");
        }
        return std::get<bool>(value_);
    }

    const std::string& AsString() const {
        if (!IsString()) {
            throw std::logic_error("Not a string");
        }
        return std::get<std::string>(value_);
    }

    const Array& AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Not an array");
        }
        return std::get<Array>(value_);
    }

    const Dict& AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("Not a map");
        }
        return std::get<Dict>(value_);
    }

    // Getter для базового значения
    const Value& GetValue() const { return value_; }

    // Перегрузка операторов сравнения
    bool operator==(const Node& other) const {
        return value_ == other.value_;
    }

    bool operator!=(const Node& other) const {
        return value_ != other.value_;
    }

private:
    Value value_; // Хранит значение Node, может быть разных типов
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    // Перегрузка оператора равенства
    bool operator==(const Document& other) const {
        return root_ == other.root_;
    }

    // Перегрузка оператора неравенства
    bool operator!=(const Document& other) const {
        return root_ != other.root_;
    }

private:
    Node root_; // Корневой узел документа JSON
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json