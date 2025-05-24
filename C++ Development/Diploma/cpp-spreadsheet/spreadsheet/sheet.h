#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Sheet : public SheetInterface {
public:
    void SetCell(Position pos, std::string text) override;
    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;
    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    bool DetectCycle(const Position& start) const;

private:
    struct PositionHasher {
        size_t operator()(const Position& pos) const {
            return std::hash<int>()(pos.row) * 37 + std::hash<int>()(pos.col);
        }
    };

    bool DetectCycleHelper(const Position& current, const Position& start,
                           std::unordered_set<Position, PositionHasher>& visited) const;

    std::unordered_map<Position, std::unique_ptr<Cell>, PositionHasher> cells_;
    std::unordered_map<Position, std::unordered_set<Position, PositionHasher>, PositionHasher> dependencies_;

    friend class Cell;
};

std::unique_ptr<SheetInterface> CreateSheet();