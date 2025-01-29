#pragma once

#include "date.h"
#include <vector>

class BudgetManager {
public:
    static const Date START_DATE;
    static const Date END_DATE;

    BudgetManager();

    // Метод для добавления дохода
    void Earn(const Date& from, const Date& to, double amount);

    // Метод для уплаты налога
    void PayTax(const Date& from, const Date& to);

    // Метод для вычисления чистой прибыли
    double ComputeIncome(const Date& from, const Date& to) const;

private:
    std::vector<double> incomes_; // Вектор доходов по дням

    // Метод для получения индекса даты относительно START_DATE
    int GetDateIndex(const Date& date) const;
};