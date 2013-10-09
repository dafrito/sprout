#ifndef SPROUT_GRAMMAR_PASS_REMOVE_HEADER
#define SPROUT_GRAMMAR_PASS_REMOVE_HEADER

#include "../Grammar.hpp"
#include <unordered_set>

namespace sprout {
namespace grammar {
namespace pass {

template <class Type, class Value>
class Remove
{
    std::unordered_set<Type> _targets;
public:
    Remove(const std::unordered_set<Type>& targets) :
        _targets(targets)
    {
    }

    template <class OType, class OToken>
    void operator()(Grammar<OType, OToken>& grammar)
    {
        for (GNode& node : grammar) {
            operator()(node);
        }
    }

    void operator()(Node<Type, Value>& node)
    {
        for (int i = 0; i < node.size(); ++i) {
            Node<Type, Value>& child = node[i];
            if (_targets.find(child.type()) != _targets.end()) {
                node.erase(i--);
            } else {
                operator()(child);
            }
        }
    }
};

} // namespace pass
} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_PASS_REMOVE_HEADER

// vim: set ts=4 sw=4 :
