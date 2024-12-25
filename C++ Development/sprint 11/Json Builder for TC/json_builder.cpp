#include "json_builder.h"

namespace json {

// --------------------
// Реализация RootContext
// --------------------
RootContext::RootContext(Builder& b) : b_(b) {}

RootContext RootContext::Value(Node::Value value) {
    b_.Value(std::move(value));
    return *this;
}

DictItemContext RootContext::StartDict() {
    b_.StartDict();
    return DictItemContext(b_);
}

ArrayItemContext RootContext::StartArray() {
    b_.StartArray();
    return ArrayItemContext(b_);
}

Node RootContext::Build() {
    return b_.Build();
}

// --------------------
// Реализация DictItemContext
// --------------------
DictItemContext::DictItemContext(Builder& b) : b_(b) {}

KeyValueContext DictItemContext::Key(std::string key) {
    b_.Key(std::move(key));
    return KeyValueContext(b_);
}

RootContext DictItemContext::EndDict() {
    b_.EndDict();
    return RootContext(b_);
}

// --------------------
// Реализация KeyValueContext
// --------------------
KeyValueContext::KeyValueContext(Builder& b) : b_(b) {}

DictItemContext KeyValueContext::Value(Node::Value value) {
    b_.Value(std::move(value));
    return DictItemContext(b_);
}

DictItemContext KeyValueContext::StartDict() {
    b_.StartDict();
    return DictItemContext(b_);
}

ArrayItemContext KeyValueContext::StartArray() {
    b_.StartArray();
    return ArrayItemContext(b_);
}

// --------------------
// Реализация ArrayItemContext
// --------------------
ArrayItemContext::ArrayItemContext(Builder& b) : b_(b) {}

ArrayItemContext ArrayItemContext::Value(Node::Value value) {
    b_.Value(std::move(value));
    return *this;
}

ArrayItemContext ArrayItemContext::StartDict() {
    b_.StartDict();
    return *this;
}

ArrayItemContext ArrayItemContext::StartArray() {
    b_.StartArray();
    return *this;
}

RootContext ArrayItemContext::EndArray() {
    b_.EndArray();
    return RootContext(b_);
}

// --------------------
// Реализация Builder
// --------------------
Builder::Builder() = default;

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
    return !is_build_finished_ &&
           (root_.IsNull() || IsInArray() || (IsInDict() && key_in_progress_));
}

Node Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Unfinished JSON: missing EndDict or EndArray");
    }
    if (root_.IsNull()) {
        throw std::logic_error("Nothing to build");
    }
    is_build_finished_ = true;
    return root_;
}

Builder& Builder::Key(std::string key) {
    if (!IsInDict()) {
        throw std::logic_error("Key outside of a dict");
    }
    if (key_in_progress_) {
        throw std::logic_error("Previous key not assigned a value");
    }
    current_key_ = std::move(key);
    key_in_progress_ = true;
    return *this;
}

Builder& Builder::Value(Node::Value value) {
    if (!CanAddValue()) {
        throw std::logic_error("Value(...) called in invalid context");
    }
    Node new_node(std::move(value));

    if (nodes_stack_.empty()) {
        root_ = std::move(new_node);
    } else if (IsInArray()) {
        Array& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.emplace_back(std::move(new_node));
    } else if (IsInDict()) {
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        dict.emplace(std::move(current_key_), std::move(new_node));
        key_in_progress_ = false;
        current_key_.clear();
    }
    return *this;
}

Builder& Builder::StartDict() {
    if (!CanAddValue()) {
        throw std::logic_error("StartDict(...) in invalid context");
    }
    Node dict_node(Dict{});
    if (nodes_stack_.empty()) {
        root_ = std::move(dict_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        auto& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.push_back(std::move(dict_node));
        nodes_stack_.push_back(&arr.back());
    } else if (IsInDict()) {
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        dict.emplace(std::move(current_key_), std::move(dict_node));
        key_in_progress_ = false;
        current_key_.clear();
        nodes_stack_.push_back(&dict.at(dict.rbegin()->first));
    }
    return *this;
}

Builder& Builder::EndDict() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw std::logic_error("EndDict() but no dict is open");
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::StartArray() {
    if (!CanAddValue()) {
        throw std::logic_error("StartArray(...) in invalid context");
    }
    Node array_node(Array{});
    if (nodes_stack_.empty()) {
        root_ = std::move(array_node);
        nodes_stack_.push_back(&root_);
    } else if (IsInArray()) {
        auto& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
        arr.push_back(std::move(array_node));
        nodes_stack_.push_back(&arr.back());
    } else if (IsInDict()) {
        Dict& dict = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        dict.emplace(std::move(current_key_), std::move(array_node));
        key_in_progress_ = false;
        current_key_.clear();
        nodes_stack_.push_back(&dict.at(dict.rbegin()->first));
    }
    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw std::logic_error("EndArray() but no array is open");
    }
    nodes_stack_.pop_back();
    return *this;
}

} // namespace json