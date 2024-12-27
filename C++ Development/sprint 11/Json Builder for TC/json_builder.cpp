#include "json_builder.h"

namespace json {

// Implementation of BaseContext::Build()
Node BaseContext::Build() {
    return builder_.Build();
}

// Builder class implementations
Builder::Builder() : root_(nullptr) {}

bool Builder::IsInArray() const {
    return !nodes_stack_.empty() && nodes_stack_.back()->IsArray();
}

bool Builder::IsInDict() const {
    return !nodes_stack_.empty() && nodes_stack_.back()->IsDict();
}

bool Builder::CanAddValue() const {
    return !is_build_finished_ && (root_.IsNull() || IsInArray() || (IsInDict() && !keys_stack_.empty()));
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

DictValueContext Builder::Key(const std::string& key) {
    if (!IsInDict()) {
        throw std::logic_error("Key() outside of a dict");
    }
    if (keys_stack_.empty()) {
        throw std::logic_error("Key() called without a corresponding key");
    }
    keys_stack_.emplace_back(key);
    return DictValueContext(*this);
}

BaseContext Builder::Value(const Node::Value& value) {
    if (!CanAddValue()) {
        throw std::logic_error("Value() called in invalid context");
    }
    Node new_node(value);

    if (nodes_stack_.empty()) {
        root_ = std::move(new_node);
    } else if (IsInArray()) {
        nodes_stack_.back()->AsArray().emplace_back(std::move(new_node));
    } else if (IsInDict()) {
        if (keys_stack_.empty()) {
            throw std::logic_error("Value() called in dict without a key");
        }
        nodes_stack_.back()->AsDict().emplace(keys_stack_.back(), std::move(new_node));
        keys_stack_.pop_back();
    }
    return BaseContext(*this);
}

DictItemContext Builder::StartDict() {
    if (!CanAddValue()) {
        throw std::logic_error("StartDict() in invalid context");
    }
    Node new_node(Dict{});
    if (nodes_stack_.empty()) {
        root_ = std::move(new_node);
        nodes_stack_.push_back(&root_);
    } else {
        if (IsInArray()) {
            nodes_stack_.back()->AsArray().emplace_back(std::move(new_node));
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (IsInDict()) {
            if (keys_stack_.empty()) {
                throw std::logic_error("StartDict() called in dict without a key");
            }
            nodes_stack_.back()->AsDict().emplace(keys_stack_.back(), std::move(new_node));
            nodes_stack_.push_back(&nodes_stack_.back()->AsDict().at(keys_stack_.back()));
            keys_stack_.pop_back();
        }
    }
    return DictItemContext(*this);
}

ArrayItemContext Builder::StartArray() {
    if (!CanAddValue()) {
        throw std::logic_error("StartArray() in invalid context");
    }
    Node new_node(Array{});
    if (nodes_stack_.empty()) {
        root_ = std::move(new_node);
        nodes_stack_.push_back(&root_);
    } else {
        if (IsInArray()) {
            nodes_stack_.back()->AsArray().emplace_back(std::move(new_node));
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (IsInDict()) {
            if (keys_stack_.empty()) {
                throw std::logic_error("StartArray() called in dict without a key");
            }
            nodes_stack_.back()->AsDict().emplace(keys_stack_.back(), std::move(new_node));
            nodes_stack_.push_back(&nodes_stack_.back()->AsDict().at(keys_stack_.back()));
            keys_stack_.pop_back();
        }
    }
    return ArrayItemContext(*this);
}

BaseContext Builder::EndDict() {
    if (!IsInDict()) {
        throw std::logic_error("EndDict() called but not in a dict");
    }
    nodes_stack_.pop_back();
    return BaseContext(*this);
}

BaseContext Builder::EndArray() {
    if (!IsInArray()) {
        throw std::logic_error("EndArray() called but not in an array");
    }
    nodes_stack_.pop_back();
    return BaseContext(*this);
}

// Implementations for DictItemContext
DictValueContext DictItemContext::Key(const std::string& key) {
    return builder_.Key(key);
}

BaseContext DictItemContext::EndDict() {
    return builder_.EndDict();
}

// Implementations for ArrayItemContext
ArrayItemContext ArrayItemContext::Value(const Node::Value& value) {
    builder_.Value(value);
    return *this;
}

ArrayItemContext ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

DictItemContext ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

BaseContext ArrayItemContext::EndArray() {
    return builder_.EndArray();
}

// Implementations for DictValueContext
DictItemContext DictValueContext::Value(const Node::Value& value) {
    builder_.Value(value);
    return DictItemContext(builder_);
}

ArrayItemContext DictValueContext::StartArray() {
    return builder_.StartArray();
}

DictItemContext DictValueContext::StartDict() {
    return builder_.StartDict();
}

} // namespace json
