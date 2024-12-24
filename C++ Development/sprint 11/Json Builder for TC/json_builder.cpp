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
    if (is_build_finished_) {
        return false;
    }

    if (nodes_stack_.empty()) {
        return root_.IsNull();
    }

    if (IsInArray()) {
        return true;
    }

    if (IsInDict() && !current_key_.empty()) {
        return true;
    }

    return false;
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

Builder& Builder::Key(std::string key) {
    if (is_build_finished_) {
        throw logic_error("Key() called after Build() - JSON is already complete.");
    }

    if (!IsInDict()) {
        throw logic_error("Key() called outside of a dict context.");
    }

    if (!current_key_.empty()) {
        throw logic_error("Key() called again before assigning a value to the previous Key.");
    }

    current_key_ = move(key);
    return *this;
}

Builder& Builder::Value(Node::Value value) {
    if (!CanAddValue()) {
        throw logic_error("Value(...) called in invalid context.");
    }

    Node new_node(move(value));

    if (nodes_stack_.empty()) {
        root_ = move(new_node);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(move(new_node));
    } else if (IsInDict()) {
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        dict.emplace(move(current_key_), move(new_node));
        current_key_.clear();
    }

    return *this;
}

Builder& Builder::StartDict() {
    if (!CanAddValue()) {
        throw logic_error("StartDict() called in invalid context.");
    }

    // Создаём новый узел-словарь
    Node dict_node(Dict{});

    if (nodes_stack_.empty()) {
        // Если стек пуст, значит это будет корень
        root_ = std::move(dict_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        // Если сверху массива — добавляем в массив
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(dict_node));
        Node& just_added = arr.back();  // <-- точно тот элемент, который вставили
        nodes_stack_.push_back(&just_added);
    } else if (IsInDict()) {
        // Если сверху словарь — добавляем {current_key_: dict_node}
        Dict& d = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        auto [it, inserted] = d.emplace(std::move(current_key_), std::move(dict_node));
        current_key_.clear();

        // it->second — это тот самый только что вставленный узел
        Node& new_dict_ref = it->second;
        nodes_stack_.push_back(&new_dict_ref);
    }

    return *this;
}

Builder& Builder::EndDict() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw logic_error("EndDict() called but there is no matching StartDict() in context.");
    }

    if (is_build_finished_) {
        throw logic_error("EndDict() called after Build() - JSON is already complete.");
    }

    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::StartArray() {
    if (!CanAddValue()) {
        throw logic_error("StartArray() called in invalid context.");
    }

    Node array_node(Array{});

    if (nodes_stack_.empty()) {
        root_ = std::move(array_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(array_node));
        Node& just_added = arr.back(); 
        nodes_stack_.push_back(&just_added);
    } else if (IsInDict()) {
        Dict& d = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        auto [it, inserted] = d.emplace(std::move(current_key_), std::move(array_node));
        current_key_.clear();

        // вместо rbegin()->first берём именно тот итератор, который вернул emplace
        Node& new_arr_ref = it->second;
        nodes_stack_.push_back(&new_arr_ref);
    }

    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw logic_error("EndArray() called but there is no matching StartArray() in context.");
    }

    if (is_build_finished_) {
        throw logic_error("EndArray() called after Build() - JSON is already complete.");
    }

    nodes_stack_.pop_back();
    return *this;
}

} // namespace json