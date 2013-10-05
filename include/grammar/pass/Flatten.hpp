#ifndef SPROUT_GRAMMAR_PASS_FLATTEN_HEADER
#define SPROUT_GRAMMAR_PASS_FLATTEN_HEADER

#include "../Grammar.hpp"

namespace sprout {
namespace grammar {
namespace pass {

class Flatten
{
    void flatten(GNode& node);
public:
    template <class Type, class Value>
    void operator()(Grammar<Type, Value>& grammar)
    {
        for (GNode& node : grammar) {
            flatten(node);
        }
    }

};

} // namespace pass
} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_PASS_FLATTEN_HEADER

// vim: set ts=4 sw=4 :
