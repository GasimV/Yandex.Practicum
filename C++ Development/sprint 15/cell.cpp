#include "cell.h"
#include "sheet.h"
#include "formula.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

// Базовый класс для реализации ячейки
class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
    // Для текстовых и пустых ячеек возвращаем пустой список
    virtual std::vector<Position> GetReferencedCells() const { return {}; }
};

class Cell::EmptyImpl : public Cell::Impl {
public:
    CellInterface::Value GetValue() const override {
        return "";
    }
    std::string GetText() const override {
        return "";
    }
};

class Cell::TextImpl : public Cell::Impl {
public:
    explicit TextImpl(std::string text) : text_(std::move(text)) {}
    CellInterface::Value GetValue() const override {
        if (!text_.empty() && text_[0] == ESCAPE_SIGN) {
            return text_.substr(1);
        }
        return text_;
    }
    std::string GetText() const override {
        return text_;
    }
private:
    std::string text_;
};

class Cell::FormulaImpl : public Cell::Impl {
public:
    explicit FormulaImpl(std::string expression)
        : formula_(ParseFormula(expression.substr(1))) {}
    CellInterface::Value GetValue() const override {
        if (!sheet_ptr_) {
            throw std::runtime_error("Sheet not bound");
        }
        auto result = formula_->Evaluate(*sheet_ptr_);
        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        }
        return std::get<FormulaError>(result);
    }
    std::string GetText() const override {
        return std::string(1, FORMULA_SIGN) + formula_->GetExpression();
    }
    std::vector<Position> GetReferencedCells() const override {
        auto refs = formula_->GetReferencedCells();
        std::sort(refs.begin(), refs.end());
        refs.erase(std::unique(refs.begin(), refs.end()), refs.end());
        return refs;
    }
    void BindSheet(Sheet& sheet) { sheet_ptr_ = &sheet; }
private:
    std::unique_ptr<FormulaInterface> formula_;
    Sheet* sheet_ptr_ = nullptr;
};

Cell::Cell(Sheet& sheet)
    : sheet_(sheet), impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
    } else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        try {
            auto formula_impl = std::make_unique<FormulaImpl>(text);
            formula_impl->BindSheet(sheet_);
            impl_ = std::move(formula_impl);
        } catch (const FormulaException&) {
            throw;
        }
    } else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    if (auto formulaImpl = dynamic_cast<FormulaImpl*>(impl_.get())) {
        return formulaImpl->GetValue();
    }
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return false;
}