#pragma once

#include "json.h"

#include <stdexcept>
#include <utility>

namespace json {

class Builder {
private:
    class BaseContext;
    class DictValueContext;
    class DictItemContext;
    class ArrayItemContext;

public:
    Builder();
    Node Build();

    DictValueContext Key(std::string key);
    BaseContext Value(Node::Value value);
    DictItemContext StartDict();
    BaseContext EndDict();
    ArrayItemContext StartArray();
    BaseContext EndArray();

private:    
    Node root_;
    std::vector<Node*> nodes_stack_;
    std::string current_key_;

    bool key_in_progress_ = false;
    bool is_build_finished_ = false;

    bool IsInArray() const;
    bool IsInDict() const;
    bool CanAddValue() const;

    class BaseContext {
    public:
        BaseContext(Builder& builder) : builder_(builder) {}
        Node Build() {
            return builder_.Build();
        }
        
        DictValueContext Key(std::string key);
        BaseContext Value(Node::Value value);
        BaseContext EndDict();
        BaseContext EndArray();
        
        DictItemContext StartDict() {
            return builder_.StartDict();
        }
        
        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }

    private:
        Builder& builder_;
    };

    // Context classes
    class DictItemContext : public BaseContext {
    public:    
        DictItemContext(BaseContext base) : BaseContext(base) {}
        Node Build() = delete;

        BaseContext Value(Node::Value value) = delete;
        BaseContext EndArray() = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
    };

    class ArrayItemContext : public BaseContext {
    public:
        ArrayItemContext(BaseContext base) : BaseContext(base) {}
        Node Build() = delete;
        
        DictValueContext Key(std::string key) = delete;
        BaseContext EndDict() = delete;
    };

    class DictValueContext : public BaseContext {
    public:
        DictValueContext(BaseContext base) : BaseContext(base) {}
        Node Build() = delete;

        BaseContext EndDict() = delete;
        DictValueContext Key(std::string key) = delete;
        BaseContext EndArray() = delete;
    };
};

} // namespace json