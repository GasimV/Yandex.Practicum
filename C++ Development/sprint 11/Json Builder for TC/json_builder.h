#pragma once

#include "json.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

namespace json {

class Builder;

// Forward declarations of context classes
class DictItemContext;
class ArrayItemContext;
class DictValueContext;

// BaseContext serves as the base class for all specific contexts
class BaseContext {
public:
    explicit BaseContext(Builder& builder) : builder_(builder) {}
    
    // Allow Build() to be called only when appropriate
    Node Build();

protected:
    Builder& builder_;
};

// Context classes
class DictItemContext : public BaseContext {
public:
    using BaseContext::BaseContext;

    DictValueContext Key(const std::string& key);
    // After ending a dict, return to the parent context
    BaseContext EndDict();

    // Disallow other operations in this context
    BaseContext Value(const Node::Value& value) = delete;
    ArrayItemContext StartArray() = delete;
    DictItemContext StartDict() = delete;
};

class ArrayItemContext : public BaseContext {
public:
    using BaseContext::BaseContext;

    ArrayItemContext Value(const Node::Value& value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
    // After ending an array, return to the parent context
    BaseContext EndArray();

    // Disallow Key in array context
    DictValueContext Key(const std::string& key) = delete;
    BaseContext EndDict() = delete;
};

class DictValueContext : public BaseContext {
public:
    using BaseContext::BaseContext;

    DictItemContext Value(const Node::Value& value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();

    // Disallow EndDict and Key in this context
    BaseContext EndDict() = delete;
    DictValueContext Key(const std::string& key) = delete;
};

// Builder class
class Builder {
public:
    Builder();

    // Build the final Node
    Node Build();

    // Start building a dictionary
    DictItemContext StartDict();

    // Start building an array
    ArrayItemContext StartArray();

    // Add a key to a dictionary
    DictValueContext Key(const std::string& key);

    // Add a value to the current context
    BaseContext Value(const Node::Value& value);
    
    BaseContext GetCurrentContext();

    // End the current dictionary
    BaseContext EndDict();

    // End the current array
    BaseContext EndArray();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;  // Stack of pointers to open containers
    std::vector<std::string> keys_stack_;  // Stack of keys for dicts
    bool is_build_finished_ = false;

    bool IsInArray() const;
    bool IsInDict() const;
    bool CanAddValue() const;
};

} // namespace json
