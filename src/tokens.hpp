#ifndef SPROUT_TOKENS_HEADER
#define SPROUT_TOKENS_HEADER

#include <vector>
#include <sstream>
#include <iostream>

template <class Type, class Value>
struct Token {
    Type type;
    Value value;

    Token() :
        type(),
        value()
    {
    }

    Token(const Type& type) :
        type(type),
        value()
    {
    }

    Token(const Type& type, const Value& value) :
        type(type),
        value(value)
    {
    }
};

template <class Type, class Value>
struct Node {
    Token<Type, Value> token;
    std::vector<Node<Type, Value>> _children;

    Node() :
        Node(Token<Type, Value>())
    {
    }

    Node(const Token<Type, Value>& token) :
        token(token)
    {
    }

    Node(const Token<Type, Value>& token, std::vector<Node<Type, Value>> children) :
        token(token),
        _children(children)
    {
    }

    Node(const Type& type, const Value& value) :
        token(Token<Type, Value>(type, value))
    {
    }

    const Type& type() const
    {
        return token.type;
    }

    void setType(const Type& type)
    {
        token.type = type;
    }

    std::string typeName() const
    {
        std::stringstream str;
        str << type();
        return str.str();
    }

    const Value& value() const
    {
        return token.value;
    }

    void setValue(const Value& value)
    {
        token.value = value;
    }

    std::string valueString() const
    {
        std::stringstream str;
        str << value();
        return str.str();
    }

    void insert(const Node<Type, Value>& child)
    {
        _children.push_back(child);
    }

    void erase(const int pos)
    {
        _children.erase(_children.begin() + pos);
    }

    const Node<Type, Value>& operator[](const int index) const
    {
        return _children[index];
    }

    Node<Type, Value>& operator[](const int index)
    {
        return _children[index];
    }

    Node<Type, Value>& operator=(const Node<Type, Value>& other)
    {
        if (this == &other) {
            return *this;
        }
        token = other.token;
        _children = other._children;
        return *this;
    }

    const std::vector<Node<Type, Value>>& children() const
    {
        return _children;
    }

    std::vector<Node<Type, Value>>& children()
    {
        return _children;
    }

    void dump(std::stringstream& str, const std::string& indent) const
    {
        str << typeName();
        if (!value().isNull()) {
            str << ":" << valueString();
        }
        if (!_children.empty()) {
            auto childIndent = indent + "    ";
            str << " [\n" << childIndent;
            for (unsigned int i = 0; i < _children.size(); ++i) {
                if (i > 0) {
                    str << ",\n" << childIndent;
                }
                _children[i].dump(str, childIndent);
            }
            str << "\n" << indent << "]";
        }
    }

    std::string dump() const
    {
        std::stringstream str;
        dump(str, "");
        return str.str();
    }
};

#endif // SPROUT_TOKENS_HEADER

// vim: set ts=4 sw=4 :
