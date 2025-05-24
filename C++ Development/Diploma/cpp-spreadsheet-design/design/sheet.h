#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;
    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    const Cell* GetConcreteCell(Position pos) const;
    Cell* GetConcreteCell(Position pos);

private:
    std::unordered_map<Position, std::unique_ptr<Cell>> cells_;
    std::unordered_map<Position, std::unordered_set<Position>> dependency_graph_;
    std::unordered_map<Position, std::unordered_set<Position>> reverse_dependency_graph_;

    void AddDependency(Position from, Position to);
    void RemoveDependency(Position from, Position to);
    void CheckForCircularDependency(Position pos, const std::vector<Position>& new_references);
    void InvalidateCache(Position pos);
};