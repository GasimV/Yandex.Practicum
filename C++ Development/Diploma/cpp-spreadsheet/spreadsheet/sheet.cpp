#include "sheet.h"
#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_set>

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid cell position");
    }
    if (cells_.find(pos) == cells_.end()) {
        cells_[pos] = std::make_unique<Cell>(*this);
    }
    cells_[pos]->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    auto it = cells_.find(pos);
    return it != cells_.end() ? it->second.get() : nullptr;
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    auto it = cells_.find(pos);
    return it != cells_.end() ? it->second.get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    cells_.erase(pos);
    dependencies_.erase(pos);
}

Size Sheet::GetPrintableSize() const {
    if (cells_.empty()) return {0, 0};

    int max_row = 0;
    int max_col = 0;
    for (const auto& [pos, cell] : cells_) {
        if (!cell->GetText().empty()) {
            max_row = std::max(max_row, pos.row + 1);
            max_col = std::max(max_col, pos.col + 1);
        }
    }
    return {max_row, max_col};
}

void Sheet::PrintValues(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) {
                output << '\t';
            }
            Position pos{row, col};
            auto it = cells_.find(pos);
            if (it != cells_.end()) {
                std::visit([&](const auto& val) { output << val; }, it->second->GetValue());
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) output << '\t';
            Position pos{row, col};
            auto it = cells_.find(pos);
            if (it != cells_.end()) {
                output << it->second->GetText();
            }
        }
        output << '\n';
    }
}

bool Sheet::DetectCycleHelper(const Position& current, const Position& start,
                              std::unordered_set<Position, PositionHasher>& visited) const {
    if (current == start && !visited.empty()) {
        return true;
    }

    if (!visited.insert(current).second) {
        return false;
    }

    if (auto it = dependencies_.find(current); it != dependencies_.end()) {
        for (const auto& next : it->second) {
            if (DetectCycleHelper(next, start, visited)) {
                return true;
            }
        }
    }
    return false;
}

bool Sheet::DetectCycle(const Position& start) const {
    std::unordered_set<Position, PositionHasher> visited;
    return DetectCycleHelper(start, start, visited);
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}