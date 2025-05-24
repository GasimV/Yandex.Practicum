#pragma once

#include "common.h"

#include "FormulaAST.h"

#include <memory>
#include <variant>
#include <sstream>

class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression);

    Value Evaluate(const SheetInterface& sheet) const override;
    std::string GetExpression() const override;
    std::vector<Position> GetReferencedCells() const override;

private:
    FormulaAST ast_;
};

// Парсит переданное выражение и возвращает объект формулы.
// Бросает FormulaException в случае если формула синтаксически некорректна.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);