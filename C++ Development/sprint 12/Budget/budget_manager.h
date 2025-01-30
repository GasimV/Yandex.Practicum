#pragma once

#include "date.h"
#include <vector>

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    BudgetManager();

    // Методы для управления бюджетом
    void Earn(const Date& from, const Date& to, double amount);
    void Spend(const Date& from, const Date& to, double amount);
    void PayTax(const Date& from, const Date& to, int tax_percentage);
    double ComputeIncome(const Date& from, const Date& to) const;

private:
    std::vector<double> incomes_; // Заработанные средства по дням
    std::vector<double> spends_;  // Потраченные средства по дням

    // Метод для получения индекса даты относительно START_DATE
    int GetDateIndex(const Date& date) const;
};