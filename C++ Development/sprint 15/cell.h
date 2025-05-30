#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

class Sheet; // предварительное объявление

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
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
};