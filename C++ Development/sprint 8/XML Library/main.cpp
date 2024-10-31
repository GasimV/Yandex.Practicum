#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы xml.h и xml.cpp
#include "xml.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(spendings.begin(), spendings.end(), 0,
                      [](int current, const Spending& spending) {
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

vector<Spending> LoadFromXml(istream& input) {
    // место для вашей реализации
    // пример корректного XML-документа в условии
    Document doc = Load(input); // Загружаем XML-документ
    const Node& root = doc.GetRoot(); // Получаем корневой элемент
    vector<Spending> spendings;

    // Перебираем элементы корневого узла, которые представляют теги <spend>
    for (const Node& node : root.Children()) {
        // Получаем значения атрибутов category и amount
        string category = node.AttributeValue<string>("category");
        int amount = node.AttributeValue<int>("amount");
        // Добавляем трату в вектор
        spendings.push_back({category, amount});
    }

    return spendings;
}

int main() {
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}