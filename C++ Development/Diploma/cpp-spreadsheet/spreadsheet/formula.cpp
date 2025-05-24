#include "formula.h"

#include "FormulaAST.h"
#include "common.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, const FormulaError& fe) {
    switch (fe.GetCategory()) {
        case FormulaError::Category::Ref:
            return output << "#REF!";
        case FormulaError::Category::Value:
            return output << "#VALUE!";
        case FormulaError::Category::Arithmetic:
            return output << "#ARITHM!";
    }
    return output;
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression) try
        : ast_(ParseFormulaAST(expression)) {
    } catch (const std::exception& e) {
        throw FormulaException(e.what());
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(sheet);
        } catch (const FormulaError& e) {
            return e;
        }
    }
    
    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> refs;
        for (const auto& pos : ast_.GetCells()) {
            refs.push_back(pos);
        }
        std::sort(refs.begin(), refs.end());
        refs.erase(std::unique(refs.begin(), refs.end()), refs.end());
        return refs;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}