#ifndef SPROUT_GRAMMAR_PASS_FLATTEN_HEADER
#define SPROUT_GRAMMAR_PASS_FLATTEN_HEADER

#include "../Grammar.hpp"
#include <unordered_set>

namespace sprout {
namespace grammar {
namespace pass {

template <class Type, class Value>
class Flatten
{
    std::unordered_set<Type> _targets;
public:
    Flatten(const std::unordered_set<Type>& targets) :
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
        for (Node<Type, Value>& child : node.children()) {
            operator()(child);
        }
        if (_targets.find(node.type()) != _targets.end()) {
            if (node.children().size() == 1) {
                node = node[0];
            }
        }
    }
};

} // namespace pass
} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_PASS_FLATTEN_HEADER

// vim: set ts=4 sw=4 :
