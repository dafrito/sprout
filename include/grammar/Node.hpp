#ifndef SPROUT_GRAMMAR_NODE_HEADER
#define SPROUT_GRAMMAR_NODE_HEADER

#include <Result.hpp>

#include <vector>
#include <sstream>

namespace sprout {
namespace grammar {

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

    void insert(const sprout::Result<Node<Type, Value>>& result)
    {
        while (result) {
            insert(*result);
            ++result;
        }
    }

    void erase(const int pos)
    {
        _children.erase(_children.begin() + pos);
    }

    const Node<Type, Value>& at(const int index) const
    {
        return _children.at(index);
    }

    Node<Type, Value>& at(const int index)
    {
        return _children.at(index);
    }

    const Node<Type, Value>& operator[](const int index) const
    {
        return _children.at(index);
    }

    Node<Type, Value>& operator[](const int index)
    {
        return _children.at(index);
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

    unsigned int size() const
    {
        return children().size();
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

template <class Type, class Value, class T>
Node<Type, Value>& operator<<(Node<Type, Value>& parent, const T& child)
{
    parent.insert(child);
    return parent;
}

} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_NODE_HEADER

// vim: set ts=4 sw=4 :