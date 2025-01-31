#include "budget_manager.h"
#include "date.h"

using namespace std::literals;

const Date BudgetManager::START_DATE = Date("2000-01-01"s);
const Date BudgetManager::END_DATE   = Date("2100-01-01"s);

// Чистая прибыль = sum(earned) - sum(spent)
double BudgetManager::ComputeSum(const Date& from, const Date& to) const {
    DayState st = tree_.ComputeSum(MakeDateSegment(from, to));
    return st.earned - st.spent;
}