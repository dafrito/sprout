#ifndef SPROUT_SEQUENCERULE_HEADER
#define SPROUT_SEQUENCERULE_HEADER

#include <vector>

#include "composite.hpp"
#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

/**
 * \brief An ordered list of subrules.
 *
 * SequenceRule matches input against an ordered list of subrules.
 * All required subrules must match for SequenceRule to match. If
 * any subrule fails to match, the iterator is reset to its original
 * position.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class SequenceRule : public RuleTraits<Input, Token>
{
    std::vector<Rule> _rules;

public:
    template <class Container>
    SequenceRule(const Container& rules)
    {
        for (auto rule : rules) {
            *this << rule;
        }
    }

    SequenceRule()
    {
    }

    template <class T>
    SequenceRule& operator<<(const T& rule)
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

namespace make {

template <class Input, class Token, class Rule, typename... Rules>
SequenceRule<Rule, Input, Token> sequence(const Rule& rule, Rules... rest)
{
    SequenceRule<Rule, Input, Token> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule, typename... Values>
SequenceRule<Rule> sequence(const Rule& rule, Values... rest)
{
    SequenceRule<Rule> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule>
SequenceRule<Rule> sequence()
{
    return SequenceRule<Rule>();
}

} // namespace make

} // namespace sprout

#endif // SPROUT_SEQUENCERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :