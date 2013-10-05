#ifndef SPROUT_MULTIPLERULE_HEADER
#define SPROUT_MULTIPLERULE_HEADER

#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

/**
 * \brief A rule that matches as many instance of a rule as possible.
 *
 * MultipleRule matches as many instances of a specified rule as possible,
 * concatenating results of each match. This rule must match at least once to
 * successfully match.
 *
 *
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class MultipleRule : public RuleTraits<Input, Token>
{
    Rule _rule;

public:
    MultipleRule(const Rule& rule) :
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        bool found = false;
        while (_rule(iter, result)) {
            found = true;
        }
        return found;
    }
};

namespace make {

/**
 * Construct a MultipleRule.
 */
template <class Rule>
MultipleRule<Rule> multiple(const Rule& rule)
{
    return MultipleRule<Rule>(rule);
}

template <class Input, class Token, class Rule>
MultipleRule<Rule, Input, Token> multiple(const Rule& rule)
{
    return MultipleRule<Rule, Input, Token>(rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_MULTIPLERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
