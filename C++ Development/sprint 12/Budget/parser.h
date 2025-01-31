#pragma once

#include "budget_manager.h"
#include "date.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

//
// Вспомогательный класс для вывода результата ComputeIncome
//
struct ReadResult {
    double total_income = 0.0;

    void Print(std::ostream& out) const {
        out << total_income << std::endl;
    }
};

//
// Функции-помощники по разбиению строки
//
inline std::pair<std::string_view, std::optional<std::string_view>>
SplitFirst(std::string_view input, char c) {
    auto cpos = input.find(c);
    if (cpos == std::string_view::npos) {
        return {input, {}};
    }
    return {input.substr(0, cpos), input.substr(cpos + 1)};
}

inline std::vector<std::string_view> Split(std::string_view input, char c) {
    std::vector<std::string_view> res;
    while (true) {
        auto [l, r] = SplitFirst(input, c);
        res.push_back(l);
        if (!r) {
            break;
        }
        input = *r;
    }
    return res;
}

//
// Базовый класс запроса
//
class Query {
public:
    Query(Date from, Date to)
        : from_(from), to_(to) {
    }

    virtual ~Query() = default;

    Date GetFrom() const { return from_; }
    Date GetTo()   const { return to_;   }

    // Обрабатывает запрос и при необходимости выводит результат
    virtual void ProcessAndPrint(BudgetManager& budget, std::ostream& out) const = 0;

private:
    Date from_, to_;
};

//
// Запрос на вычисление (ComputeIncome)
//
class ComputeQuery : public Query {
public:
    using Query::Query;
    virtual ~ComputeQuery() = default;

    // Для ComputeQuery мы хотим возвращать ReadResult
    [[nodiscard]] virtual ReadResult Process(const BudgetManager& budget) const = 0;

    // Вызываем Process(...) и печатаем результат
    void ProcessAndPrint(BudgetManager& budget, std::ostream& out) const override {
        auto res = Process(budget);
        res.Print(out);
    }
};

//
// Запрос на модификацию (Earn / Spend / PayTax)
//
class ModifyQuery : public Query {
public:
    using Query::Query;
    virtual ~ModifyQuery() = default;

    // Ничего не возвращаем, только меняем состояние
    virtual void Process(BudgetManager& budget) const = 0;

    void ProcessAndPrint(BudgetManager& budget, std::ostream&) const override {
        Process(budget);
    }
};

//
// Фабрика для создания Query по её "команде"
//
class QueryFactory {
public:
    [[nodiscard]] virtual std::unique_ptr<Query> Construct(std::string_view config) const = 0;

    // Выбирает подходящую фабрику по ключу (строке).
    static const QueryFactory& GetFactory(std::string_view id);

    virtual ~QueryFactory() = default;
};

//
// Парсим строку и создаём Query
//
inline std::unique_ptr<Query> ParseQuery(std::string_view line) {
    auto [command, pconfig] = SplitFirst(line, ' ');
    if (!pconfig) {
        // Нет аргументов
        return nullptr;
    }

    const auto& factory = QueryFactory::GetFactory(command);
    return factory.Construct(*pconfig);
}
