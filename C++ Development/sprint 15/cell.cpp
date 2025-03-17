#include "cell.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
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
        auto result = formula_->Evaluate();
        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        }
        return std::get<FormulaError>(result);
    }
    
    std::string GetText() const override {
        return FORMULA_SIGN + formula_->GetExpression();
    }
    
private:
    std::unique_ptr<FormulaInterface> formula_;
};

Cell::Cell() : impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
    } else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        try {
            impl_ = std::make_unique<FormulaImpl>(text);
        } catch (const FormulaException&) {
            throw;  // Если формула синтаксически неверна, пробрасываем исключение.
        }
    } else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}