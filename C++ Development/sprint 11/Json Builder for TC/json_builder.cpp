#include "json_builder.h"

using namespace std;

namespace json {

// -----------------------------------------------------------------------------
// Вспомогательные методы
// -----------------------------------------------------------------------------

bool Builder::IsInArray() const {
    if (nodes_stack_.empty()) {
        return false;
    }
    return nodes_stack_.back()->IsArray();
}

bool Builder::IsInDict() const {
    if (nodes_stack_.empty()) {
        return false;
    }
    return nodes_stack_.back()->IsDict();
}

bool Builder::CanAddValue() const {
    if (!nodes_stack_.empty()) {
        if (nodes_stack_.back()->IsArray()) {
            std::cerr << "  Context: In Array\n";
        } else if (nodes_stack_.back()->IsDict()) {
            std::cerr << "  Context: In Dict\n";
        }
    }
    if (IsInDict() && !key_in_progress_) {
        return false;
    }
    return !is_build_finished_ && (root_.IsNull() || IsInArray() || IsInDict());
}

// -----------------------------------------------------------------------------
// Основные методы
// -----------------------------------------------------------------------------

Node Builder::Build() {
    if (is_build_finished_) {
        throw logic_error("Build() has already been called. The JSON object is complete.");
    }

    if (!nodes_stack_.empty()) {
        throw logic_error("Unfinished JSON: some StartDict/StartArray have not been closed.");
    }

    if (root_.IsNull()) {
        throw logic_error("Cannot build empty JSON object.");
    }

    is_build_finished_ = true;
    return root_;
}

DictValueContext Builder::Key(string key) {
    if (!IsInDict()) {
        throw std::logic_error("Key() called outside of a dictionary.");
    }
    if (key_in_progress_) {
        throw std::logic_error("Key() called again before assigning a value to the previous key.");
    }

    current_key_ = std::move(key);
    key_in_progress_ = true;
    return DictValueContext(BaseContext(*this));
}

BaseContext Builder::Value(Node::Value value) {
    if (!CanAddValue()) {
        throw std::logic_error("Value(...) called in invalid context.");
    }

    Node new_node(std::move(value));

    if (nodes_stack_.empty()) {
        if (!root_.IsNull()) {
            throw std::logic_error("Root object already set.");
        }
        root_ = std::move(new_node);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(new_node));
    } else if (IsInDict()) {
        if (!key_in_progress_) {
            throw std::logic_error("Value(...) in Dict but Key(...) wasn't called (or already used).");
        }
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        dict.emplace(std::move(current_key_), std::move(new_node));
        key_in_progress_ = false;
        current_key_.clear();
    }

    return BaseContext(*this);
}

DictItemContext Builder::StartDict() {
    if (!CanAddValue()) {
        throw std::logic_error("StartDict() called in invalid context.");
    }

    Node dict_node(Dict{});

    if (nodes_stack_.empty()) {
        root_ = std::move(dict_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(dict_node));
        nodes_stack_.push_back(&arr.back());
    } else if (IsInDict()) {
        if (!key_in_progress_) {
            throw std::logic_error("StartDict() called in a dictionary without a valid key.");
        }
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        auto [it, inserted] = dict.emplace(std::move(current_key_), std::move(dict_node));
        key_in_progress_ = false;
        current_key_.clear();
        nodes_stack_.push_back(&it->second);
    }

    return DictItemContext(BaseContext(*this));
}

BaseContext Builder::EndDict() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw std::logic_error("EndDict() called but there is no matching StartDict().");
    }

    nodes_stack_.pop_back();
    return BaseContext(*this);
}

ArrayItemContext Builder::StartArray() {
    if (!CanAddValue()) {
        throw std::logic_error("StartArray() called in invalid context.");
    }

    Node array_node(Array{});

    if (nodes_stack_.empty()) {
        root_ = std::move(array_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(array_node));
        nodes_stack_.push_back(&arr.back());
    } else if (IsInDict()) {
        if (!key_in_progress_) {
            throw std::logic_error("StartArray() called in a dictionary without a valid key.");
        }
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        auto [it, inserted] = dict.emplace(std::move(current_key_), std::move(array_node));
        key_in_progress_ = false;
        current_key_.clear();
        nodes_stack_.push_back(&it->second);
    }

    return ArrayItemContext(BaseContext(*this));
}

BaseContext Builder::EndArray() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw std::logic_error("EndArray() called but there is no matching StartArray().");
    }

    nodes_stack_.pop_back();
    return BaseContext(*this);
}

// -----------------------------------------------------------------------------
// Методы контекстных классов
// -----------------------------------------------------------------------------

DictValueContext BaseContext::Key(string key) {
    return builder_.Key(move(key));
}

BaseContext BaseContext::Value(Node::Value value) {
    return builder_.Value(move(value));
}

BaseContext BaseContext::EndDict() {
    return builder_.EndDict();
}

BaseContext BaseContext::EndArray() {
    return builder_.EndArray();
}

} // namespace json