#include "json.h"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <variant>

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

// Загружает массив из входного потока
Node LoadArray(istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    if (!input) {
        throw ParsingError("Unexpected end of array");
    }

    return Node(move(result));
}

// Загружаем число (int или double) из входного потока
Node LoadNumber(istream& input) {
    string parsed_num;

    // Лямбда для чтения следующего символа и добавления его к parsed_num
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream");
        }
    };

    // Прочитать необязательный отрицательный знак
    if (input.peek() == '-') {
        read_char();
    }

    // Прочитать целую часть числа
    if (isdigit(input.peek())) {
        while (isdigit(input.peek())) {
            read_char();
        }
    } else {
        throw ParsingError("A digit is expected");
    }

    bool is_int = true;
    // Прочитать дробную часть, если она существует
    if (input.peek() == '.') {
        read_char();
        while (isdigit(input.peek())) {
            read_char();
        }
        is_int = false;
    }

    // Прочитать экспоненциальную часть, если она существует
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        while (isdigit(input.peek())) {
            read_char();
        }
        is_int = false;
    }

    // Преобразуем разобранное число в int или double
    try {
        if (is_int) {
            return Node(stoi(parsed_num));
        }
        return Node(stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert " + parsed_num + " to number");
    }
}

// Загружает строку из входного потока, обрабатывая escape-последовательности
Node LoadString(istream& input) {
    string result;
    char ch;
    while (input.get(ch)) {
        if (ch == '\\') {
            if (!input.get(ch)) {
                throw ParsingError("String parsing error");
            }
            switch (ch) {
                case 'n':
                    result.push_back('\n');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case '"':
                    result.push_back('"');
                    break;
                case '\\':
                    result.push_back('\\');
                    break;
                default:
                    throw ParsingError("Unrecognized escape sequence \\" + string(1, ch));
            }
        } else if (ch == '"') {
            return Node(move(result));
        } else {
            result.push_back(ch);
        }
    }
    throw ParsingError("String parsing error");
}

// Загружает словарь (map) из входного потока
Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        if (c != '"') {
            throw ParsingError("Expected key in double quotes");
        }

        string key = LoadString(input).AsString();
        input >> c; // Ожидается ':' после ключа
        if (c != ':') {
            throw ParsingError("Expected ':' after key");
        }
        result.insert({move(key), LoadNode(input)});
    }

    if (!input) {
        throw ParsingError("Unexpected end of object");
    }

    return Node(move(result));
}

// Загружает Node из входного потока
Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (isdigit(c) || c == '-') {
        input.putback(c);
        return LoadNumber(input);
    } else if (isalpha(c)) {
        // Прочитать слово целиком
        std::string word;
        word += c;
        while (isalpha(input.peek())) {
            word += static_cast<char>(input.get());
        }
        if (word == "null") {
            return Node(nullptr);
        } else if (word == "true") {
            return Node(true);
        } else if (word == "false") {
            return Node(false);
        } else {
            throw ParsingError("Invalid literal: " + word);
        }
    } else {
        throw ParsingError("Unexpected character or invalid value while parsing JSON");
    }
}

}  // namespace

// Конструктор Document
Document::Document(Node root) : root_(std::move(root)) {}

// Возвращает корневой узел документа
const Node& Document::GetRoot() const {
    return root_;
}

// Загружает документ JSON из входного потока
Document Load(istream& input) {
    return Document{LoadNode(input)};
}

// Вспомогательная функция для печати значения узла
void PrintValue(const Node::Value& value, ostream& out);

// Печатает узел
void PrintNode(const Node& node, ostream& out) {
    PrintValue(node.GetValue(), out);
}

// Выводит значение, хранящееся в узле
void PrintValue(const Node::Value& value, ostream& out) {
    visit([&out](const auto& val) {
        using T = decay_t<decltype(val)>;
        if constexpr (is_same_v<T, nullptr_t>) {
            out << "null";
        } else if constexpr (is_same_v<T, Array>) {
            out << '[';
            bool first = true;
            for (const auto& item : val) {
                if (!first) {
                    out << ", ";
                }
                first = false;
                PrintNode(item, out);
            }
            out << ']';
        } else if constexpr (is_same_v<T, Dict>) {
            out << '{';
            bool first = true;
            for (const auto& [key, node] : val) {
                if (!first) {
                    out << ", ";
                }
                first = false;
                out << '"' << key << "\": ";
                PrintNode(node, out);
            }
            out << '}';
        } else if constexpr (is_same_v<T, bool>) {
            out << (val ? "true" : "false");
        } else if constexpr (is_same_v<T, int> || is_same_v<T, double>) {
            out << val;
        } else if constexpr (is_same_v<T, string>) {
            out << '"';
            for (const char c : val) {
                switch (c) {
                    case '\\':
                        out << "\\\\";
                        break;
                    case '"':
                        out << "\\\"";
                        break;
                    case '\n':
                        out << "\\n";
                        break;
                    case '\r':
                        out << "\\r";
                        break;
                    case '\t':
                        out << "\\t";
                        break;
                    default:
                        out << c;
                }
            }
            out << '"';
        }
    }, value);
}

// Печатает весь документ
void Print(const Document& doc, ostream& output) {
    PrintNode(doc.GetRoot(), output);
}

}  // namespace json