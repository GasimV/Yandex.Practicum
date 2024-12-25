#pragma once

#include "json.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

namespace json {

class Builder;
class DictItemContext;
class KeyValueContext;
class ArrayItemContext;
class RootContext;

// ------------------------
// Главный класс Builder
// ------------------------
class Builder {
public:
    Builder();

    Node Build();

    // Методы для контекстов
    Builder& Key(std::string key);
    Builder& Value(Node::Value value);
    Builder& StartDict();
    Builder& EndDict();
    Builder& StartArray();
    Builder& EndArray();

    RootContext Start();

private:
    friend class DictItemContext;
    friend class KeyValueContext;
    friend class ArrayItemContext;
    friend class RootContext;

    Node root_;
    std::vector<Node*> nodes_stack_;
    std::string current_key_;
    bool key_in_progress_ = false;
    bool is_build_finished_ = false;

    bool IsInArray() const;
    bool IsInDict() const;
    bool CanAddValue() const;
};

class RootContext {
public:
    explicit RootContext(Builder& b);
    RootContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Node Build();

private:
    Builder& b_;
};

class DictItemContext {
public:
    explicit DictItemContext(Builder& b);
    KeyValueContext Key(std::string key);
    RootContext EndDict();

private:
    Builder& b_;
};

class KeyValueContext {
public:
    explicit KeyValueContext(Builder& b);
    DictItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();

private:
    Builder& b_;
};

class ArrayItemContext {
public:
    explicit ArrayItemContext(Builder& b);
    ArrayItemContext Value(Node::Value value);
    ArrayItemContext StartDict();
    ArrayItemContext StartArray();
    RootContext EndArray();

private:
    Builder& b_;
};

} // namespace json