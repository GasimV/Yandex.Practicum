#include "budget_manager.h"
#include "parser.h"

#include <iostream>
#include <string_view>
#include <string>

int ReadNumberOnLine(std::istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}

void ParseAndProcessQuery(BudgetManager& manager, std::string_view line) {
    auto query = ParseQuery(line);
    if (!query) {
        return;
    }
    query->ProcessAndPrint(manager, std::cout);
}

int main() {
    BudgetManager manager;

    // Считываем количество запросов
    int query_count = ReadNumberOnLine(std::cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(std::cin, line);
        ParseAndProcessQuery(manager, line);
    }

    return 0;
}
