#pragma once 
 
#include <iostream> 
#include <map> 
#include <string> 
#include <vector> 
#include <variant> 
#include <stdexcept> 
 
namespace json { 

// Сохраните объявления Dict и Array без изменения 
using Dict = std::map<std::string, class Node>; 
using Array = std::vector<class Node>; 
 
// Эта ошибка должна выбрасываться при ошибках парсинга JSON 
class ParsingError : public std::runtime_error { 
public: 
    using runtime_error::runtime_error; 
}; 

// Base value class for Node
class Value : public std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>::variant;
};

class Node : public Value { 
public: 
    using Value::Value; 

    // Методы проверки типов для определения типа хранимого значения 
    bool IsInt() const { return std::holds_alternative<int>(*this); } 
    bool IsDouble() const { return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this); } 
    bool IsPureDouble() const { return std::holds_alternative<double>(*this); } 
    bool IsBool() const { return std::holds_alternative<bool>(*this); } 
    bool IsString() const { return std::holds_alternative<std::string>(*this); } 
    bool IsNull() const { return std::holds_alternative<std::nullptr_t>(*this); } 
    bool IsArray() const { return std::holds_alternative<Array>(*this); } 
    bool IsMap() const { return std::holds_alternative<Dict>(*this); } 
 
    // Методы для получения значения, хранящегося в Node 
    // Выдает std::logic_error, если тип значения не совпадает 
    int AsInt() const { 
        if (!IsInt()) { 
            throw std::logic_error("Not an int"); 
        } 
        return std::get<int>(*this); 
    } 
 
    double AsDouble() const { 
        if (IsInt()) { 
            return static_cast<double>(std::get<int>(*this)); 
        } else if (IsPureDouble()) { 
            return std::get<double>(*this); 
        } 
        throw std::logic_error("Not a double"); 
    } 
 
    bool AsBool() const { 
        if (!IsBool()) { 
            throw std::logic_error("Not a bool"); 
        } 
        return std::get<bool>(*this); 
    } 
 
    const std::string& AsString() const { 
        if (!IsString()) { 
            throw std::logic_error("Not a string"); 
        } 
        return std::get<std::string>(*this); 
    } 
 
    const Array& AsArray() const { 
        if (!IsArray()) { 
            throw std::logic_error("Not an array"); 
        } 
        return std::get<Array>(*this); 
    } 
 
    const Dict& AsMap() const { 
        if (!IsMap()) { 
            throw std::logic_error("Not a map"); 
        } 
        return std::get<Dict>(*this); 
    } 
 
    // Getter для базового значения 
    const Value& GetValue() const { return *this; } 
 
    // Перегрузка операторов сравнения 
    bool operator==(const Node& other) const { 
        return static_cast<const Value&>(*this) == static_cast<const Value&>(other); 
    } 
 
    bool operator!=(const Node& other) const { 
        return static_cast<const Value&>(*this) != static_cast<const Value&>(other); 
    } 
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