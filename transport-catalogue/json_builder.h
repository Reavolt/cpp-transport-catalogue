#include "json.h"

namespace json {
    class ItemContext;
    class KeyItemContext;
    class ValueContext;
    class DictItemContext;
    class ArrayItemContext;

    class Builder {
    public:
        Builder();
        KeyItemContext Key(std::string key);
        Builder& Value(Node value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
        Node Build();

    private:
        Node root_;
        std::vector<Node*> nodes_stack_;

        template <typename T>
        void InputResult(T elem) {
            if (nodes_stack_.back()->IsArray()) {
                const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(elem);
                nodes_stack_.emplace_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
            }
            else {
                *nodes_stack_.back() = elem;
            }
        }
    };

    class ItemContext {
    public:
        ItemContext(Builder& builder) :builder_(builder) {};
        KeyItemContext Key(std::string key);
        Builder& Value(Node value);
        DictItemContext StartDict();
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
    private:
        Builder& builder_;
    };

    class KeyItemContext :public ItemContext {
    public:
        KeyItemContext(Builder& builder) :ItemContext(builder) {};
        KeyItemContext Key(std::string key) = delete;
        ValueContext Value(Node value);
        Builder& EndDict() = delete;
        Builder& EndArray() = delete;
    };

    class ValueContext :public ItemContext {
    public:
        ValueContext(Builder& builder) :ItemContext(builder) {};
        Builder& Value(Node value) = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
        Builder& EndArray() = delete;
    };

    class DictItemContext :public ItemContext {
    public:
        DictItemContext(Builder& builder) :ItemContext(builder) {};
        Builder& Value(Node value) = delete;
        DictItemContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
        Builder& EndArray() = delete;
    };

    class ArrayItemContext :public ItemContext {
    public:
        ArrayItemContext(Builder& builder) :ItemContext(builder) {};
        KeyItemContext Key(std::string key) = delete;
        ArrayItemContext Value(Node value) { return ItemContext::Value(std::move(value)); }
        Builder& EndDict() = delete;
    };

}//namespace json
