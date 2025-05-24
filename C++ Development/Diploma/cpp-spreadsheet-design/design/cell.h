#pragma once

#include "common.h"
#include "formula.h"

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <variant>

class Sheet;  // Предварительное объявление

class Cell : public CellInterface {
public:
    explicit Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    void InvalidateCache();
    bool HasCachedValue() const;
    
private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    
    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;

    mutable std::optional<Value> cache_;
    std::set<Position> referenced_cells_;

    void UpdateDependencies(const std::vector<Position>& new_dependencies);
};