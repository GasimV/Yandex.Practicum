#pragma once

#include "common.h"
#include "formula.h"

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <variant>

class Sheet;

class Cell : public CellInterface {
public:
    explicit Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    bool IsReferenced() const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    Sheet& sheet_;
    std::unique_ptr<Impl> impl_;

    void UpdateDependencies(Position pos, const std::vector<Position>& refs);
};