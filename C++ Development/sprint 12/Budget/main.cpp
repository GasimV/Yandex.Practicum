#include "budget_manager.h"

#include <iostream>
#include <string_view>
#include <sstream>

// Объявление функции для парсинга и обработки запроса
void ParseAndProcessQuery(BudgetManager& manager, std::string_view line) {
    // Используем фигурные скобки для инициализации istringstream и избегаем most vexing parse
    std::istringstream iss{std::string(line)};
    std::string command;
    iss >> command;

    if (command == "ComputeIncome") {
        std::string from_str, to_str;
        iss >> from_str >> to_str;
        Date from(from_str);
        Date to(to_str);
        double income = manager.ComputeIncome(from, to);
        std::cout << income << "\n";
    }
    else if (command == "Earn") {
        std::string from_str, to_str;
        double amount;
        iss >> from_str >> to_str >> amount;
        Date from(from_str);
        Date to(to_str);
        manager.Earn(from, to, amount);
    }
    else if (command == "Spend") {
        std::string from_str, to_str;
        double amount;
        iss >> from_str >> to_str >> amount;
        Date from(from_str);
        Date to(to_str);
        manager.Spend(from, to, amount);
    }
    else if (command == "PayTax") {
        std::string from_str, to_str;
        int tax_percentage;
        iss >> from_str >> to_str >> tax_percentage;
        Date from(from_str);
        Date to(to_str);
        manager.PayTax(from, to, tax_percentage);
    }
}

int ReadNumberOnLine(std::istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}

int main() {
    BudgetManager manager;

    const int query_count = ReadNumberOnLine(std::cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) {
            ParseAndProcessQuery(manager, line);
        }
    }

    return 0;
}