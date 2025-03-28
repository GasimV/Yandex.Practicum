#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>
#include <optional>
#include <unordered_set>

class Sheet : public SheetInterface {
public:
    ~Sheet() override = default;

    void SetCell(Position pos, std::string text) override;
    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    struct PositionHasher {
        std::size_t operator()(const Position& pos) const {
            return std::hash<int>()(pos.row) * 31 + std::hash<int>()(pos.col);
        }
    };

    std::unordered_map<Position, std::unique_ptr<Cell>, PositionHasher> cells_;

    // Граф зависимостей: для каждой ячейки множество ячеек, на которые она ссылается
    std::unordered_map<Position, std::unordered_set<Position, PositionHasher>, PositionHasher> dependencies_;

    bool DetectCycle(const Position& start) const;
    bool DetectCycleHelper(const Position& current, const Position& start,
                           std::unordered_set<Position, PositionHasher>& visited) const;
};