#include "budget_manager.h"
#include <algorithm>

using namespace std::literals;

// Определение константных дат начала и конца
const Date BudgetManager::START_DATE = Date("2000-01-01"s);
const Date BudgetManager::END_DATE = Date("2100-01-01"s);

BudgetManager::BudgetManager() {
    // Вычисляем количество дней между START_DATE и END_DATE
    int size = Date::ComputeDistance(START_DATE, END_DATE);
    incomes_.assign(size, 0.0); // Инициализируем вектор нулями
    spends_.assign(size, 0.0);  // Инициализируем вектор нулями
}

int BudgetManager::GetDateIndex(const Date& date) const {
    return Date::ComputeDistance(START_DATE, date);
}

void BudgetManager::Earn(const Date& from, const Date& to, double amount) {
    int start_idx = GetDateIndex(from);
    int end_idx = GetDateIndex(to);
    int num_days = end_idx - start_idx + 1; // Количество дней в диапазоне
    double amount_per_day = amount / num_days;

    for (int i = start_idx; i <= end_idx; ++i) {
        incomes_[i] += amount_per_day;
    }
}

void BudgetManager::Spend(const Date& from, const Date& to, double amount) {
    int start_idx = GetDateIndex(from);
    int end_idx = GetDateIndex(to);
    int num_days = end_idx - start_idx + 1; // Количество дней в диапазоне
    double amount_per_day = amount / num_days;

    for (int i = start_idx; i <= end_idx; ++i) {
        spends_[i] += amount_per_day;
    }
}

void BudgetManager::PayTax(const Date& from, const Date& to, int tax_percentage) {
    double tax_multiplier = (100.0 - tax_percentage) / 100.0;
    int start_idx = GetDateIndex(from);
    int end_idx = GetDateIndex(to);

    for (int i = start_idx; i <= end_idx; ++i) {
        incomes_[i] *= tax_multiplier; // Применяем налог только к заработанным средствам
    }
}

double BudgetManager::ComputeIncome(const Date& from, const Date& to) const {
    int start_idx = GetDateIndex(from);
    int end_idx = GetDateIndex(to);
    double total_income = 0.0;

    for (int i = start_idx; i <= end_idx; ++i) {
        total_income += (incomes_[i] - spends_[i]); // Чистая прибыль за день
    }

    return total_income;
}