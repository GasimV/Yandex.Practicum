#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы json.h и json.cpp
#include "json.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(
        spendings.begin(), spendings.end(), 0, [](int current, const Spending& spending){
            return current + spending.amount;
        });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

vector<Spending> LoadFromJson(istream& input) {
    // место для вашей реализации
    // пример корректного JSON-документа в условии
    Document doc = Load(input); // Загружаем JSON-документ
    const Node& root = doc.GetRoot(); // Получаем корневой узел
    vector<Spending> spendings;

    // Перебираем элементы массива в корневом узле
    for (const Node& item : root.AsArray()) {
        // Каждый элемент массива - словарь
        const auto& map = item.AsMap();
        // Получаем категорию и сумму
        string category = map.at("category").AsString();
        int amount = map.at("amount").AsInt();
        // Добавляем трату в вектор
        spendings.push_back({category, amount});
    }
    
    return spendings;
}

int main() {
    // не меняйте main
    const vector<Spending> spendings = LoadFromJson(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}