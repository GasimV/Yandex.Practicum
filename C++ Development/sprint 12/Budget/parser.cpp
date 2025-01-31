#include "parser.h"
#include "bulk_updater.h"

#include <unordered_map>
#include <stdexcept>

namespace queries {

// ---- ComputeIncome ----
class ComputeIncome : public ComputeQuery {
public:
    using ComputeQuery::ComputeQuery;

    [[nodiscard]] ReadResult Process(const BudgetManager& budget) const override {
        double sum = budget.ComputeSum(GetFrom(), GetTo());
        return {sum};
    }

    // Фабрика для ComputeIncome
    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            // config: "<from> <to>"
            auto parts = Split(config, ' ');
            return std::make_unique<ComputeIncome>(
                Date(parts[0]),
                Date(parts[1])
            );
        }
    };
};

// ---- Earn (добавить доход) ----
class Earn : public ModifyQuery {
public:
    Earn(Date from, Date to, double amount)
        : ModifyQuery(from, to)
        , amount_(amount)
    {
    }

    void Process(BudgetManager& budget) const override {
        int day_count = Date::ComputeDistance(GetFrom(), GetTo()) + 1;
        double per_day = amount_ / static_cast<double>(day_count);

        // Увеличиваем earned
        BulkMoneyAdder adder;
        adder.delta.earned = per_day;

        budget.AddBulkOperation(GetFrom(), GetTo(), adder);
    }

    // Фабрика для Earn
    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            // config: "<from> <to> <amount>"
            auto parts = Split(config, ' ');
            double amount = std::stod(std::string(parts[2]));
            return std::make_unique<Earn>(
                Date(parts[0]),
                Date(parts[1]),
                amount
            );
        }
    };

private:
    double amount_;
};

// ---- Spend (добавить расход) ----
class Spend : public ModifyQuery {
public:
    Spend(Date from, Date to, double amount)
        : ModifyQuery(from, to)
        , amount_(amount)
    {
    }

    void Process(BudgetManager& budget) const override {
        int day_count = Date::ComputeDistance(GetFrom(), GetTo()) + 1;
        double per_day = amount_ / static_cast<double>(day_count);

        // Увеличиваем spent
        BulkMoneyAdder adder;
        adder.delta.spent = per_day;

        budget.AddBulkOperation(GetFrom(), GetTo(), adder);
    }

    // Фабрика для Spend
    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            // config: "<from> <to> <amount>"
            auto parts = Split(config, ' ');
            double amount = std::stod(std::string(parts[2]));
            return std::make_unique<Spend>(
                Date(parts[0]),
                Date(parts[1]),
                amount
            );
        }
    };

private:
    double amount_;
};

// ---- PayTax (процентная ставка) ----
class PayTax : public ModifyQuery {
public:
    PayTax(Date from, Date to, double rate_percent)
        : ModifyQuery(from, to)
    {
        // factor = 1 - rate% / 100
        tax_.factor = 1.0 - rate_percent / 100.0;
    }

    void Process(BudgetManager& budget) const override {
        budget.AddBulkOperation(GetFrom(), GetTo(), tax_);
    }

    // Фабрика для PayTax
    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            // config: "<from> <to> <rate>"
            auto parts = Split(config, ' ');
            double rate = std::stod(std::string(parts[2]));

            return std::make_unique<PayTax>(
                Date(parts[0]),
                Date(parts[1]),
                rate
            );
        }
    };

private:
    BulkTaxApplier tax_;
};

} // namespace queries

//
// Реализация QueryFactory::GetFactory
//
const QueryFactory& QueryFactory::GetFactory(std::string_view id) {
    using namespace std::literals;

    static queries::ComputeIncome::Factory compute_income_factory;
    static queries::Earn::Factory          earn_factory;
    static queries::Spend::Factory         spend_factory;
    static queries::PayTax::Factory        pay_tax_factory;

    static std::unordered_map<std::string_view, const QueryFactory&> factories{
        {"ComputeIncome"sv, compute_income_factory},
        {"Earn"sv,          earn_factory},
        {"Spend"sv,         spend_factory},
        {"PayTax"sv,        pay_tax_factory},
    };

    if (auto it = factories.find(id); it != factories.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown command: " + std::string(id));
}
