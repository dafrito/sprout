#ifndef SPROUT_FLATTENPASS_HEADER
#define SPROUT_FLATTENPASS_HEADER

#include "Grammar.hpp"

namespace sprout {

class FlattenPass
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

} // namespace sprout

#endif // SPROUT_FLATTENPASS_HEADER

// vim: set ts=4 sw=4 :
