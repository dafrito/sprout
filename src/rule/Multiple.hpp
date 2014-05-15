#ifndef SPROUT_RULE_MULTIPLE_HEADER
#define SPROUT_RULE_MULTIPLE_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

namespace sprout {
namespace rule {

/**
 * \brief A rule that matches as many instance of a rule as possible.
 *
 * Multiple matches as many instances of a specified rule as possible,
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
class Multiple : public RuleTraits<Input, Token>
{
    Rule _rule;

public:
    Multiple(const Rule& rule) :
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

/**
 * Construct a Multiple.
 */
template <class Rule>
Multiple<Rule> multiple(const Rule& rule)
{
    return Multiple<Rule>(rule);
}

template <class Input, class Token, class Rule>
Multiple<Rule, Input, Token> multiple(const Rule& rule)
{
    return Multiple<Rule, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_MULTIPLE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
