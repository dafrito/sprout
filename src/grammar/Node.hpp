#ifndef SPROUT_GRAMMAR_NODE_HEADER
#define SPROUT_GRAMMAR_NODE_HEADER

#include <Result.hpp>

#include <vector>
#include <sstream>

namespace sprout {
namespace grammar {

template <class Type, class Value>
class Node {
    Type _type;
    Value _value;
    std::vector<Node<Type, Value>> _children;

public:
    typedef Type type_type;
    typedef Value value_type;

    Node()
    {
    }

    Node(const Type& type) :
        _type(type)
    {
    }

    Node(const Type& type, const std::vector<Node<Type, Value>>& children) :
        _type(type),
        _children(children)
    {
    }

    Node(const Type& type, const Value& value) :
        _type(type),
        _value(value)
    {
    }

    Node(const Type& type, const Value& value, const std::vector<Node<Type, Value>>& children) :
        _type(type),
        _value(value),
        _children(children)
    {
    }

    const Type& type() const
    {
        return _type;
    }

    void setType(const Type& type)
    {
        _type = type;
    }

    std::string typeName() const
    {
        std::stringstream str;
        str << type();
        return str.str();
    }

    const Value& value() const
    {
        return _value;
    }

    void setValue(const Value& value)
    {
        _value = value;
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
        _type = other._type;
        _value = other._value;
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

    bool operator==(const Node<Type, Value>& other) const
    {
        if (this == &other) {
            return true;
        }
        if (type() != other.type() || value() != other.value()) {
            return false;
        }
        if (size() != other.size()) {
            return false;
        }
        for (int i = 0; i < size(); ++i) {
            auto child = at(i);
            if (child != other[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Node<Type, Value>& other) const
    {
        return !(*this == other);
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

namespace std {

template <class Type, class Value>
std::ostream& operator<<(std::ostream& stream, const sprout::grammar::Node<Type, Value>& node)
{
    return stream << node.dump();
}

} // namespace std

#endif // SPROUT_GRAMMAR_NODE_HEADER

// vim: set ts=4 sw=4 :
