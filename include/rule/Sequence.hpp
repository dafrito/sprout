#ifndef SPROUT_RULE_SEQUENCE_HEADER
#define SPROUT_RULE_SEQUENCE_HEADER

#include "composite.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>

namespace sprout {
namespace rule {

/**
 * \brief An ordered list of subrules.
 *
 * Sequence matches input against an ordered list of subrules.
 * All required subrules must match for Sequence to match. If
 * any subrule fails to match, the iterator is reset to its original
 * position.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Sequence : public RuleTraits<Input, Token>
{
    std::vector<Rule> _rules;

public:
    template <class Container>
    Sequence(const Container& rules)
    {
        for (auto rule : rules) {
            *this << rule;
        }
    }

    Sequence()
    {
    }

    template <class T>
    Sequence& operator<<(const T& rule)
    {
        _rules.push_back(rule);
        return *this;
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        auto iter = orig;
        auto head = result.head();
        for (auto rule : _rules) {
            if (!rule(iter, result)) {
                result.moveHead(head);
                return false;
            }
        }
        orig = iter;
        return true;
    }
};

template <class Input, class Token, class Rule, typename... Rules>
Sequence<Rule, Input, Token> sequence(const Rule& rule, Rules... rest)
{
    Sequence<Rule, Input, Token> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule, typename... Values>
Sequence<Rule> sequence(const Rule& rule, Values... rest)
{
    Sequence<Rule> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule>
Sequence<Rule> sequence()
{
    return Sequence<Rule>();
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_SEQUENCE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
