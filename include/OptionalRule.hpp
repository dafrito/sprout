#ifndef SPROUT_OPTIONALRULE_HEADER
#define SPROUT_OPTIONALRULE_HEADER

#include <vector>
#include <algorithm>

#include "RuleTraits.hpp"
#include "SequenceRule.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class OptionalRule : public RuleTraits<Input, Token>
{
    const Rule _rule;

public:
    OptionalRule(const Rule& rule) :
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        _rule(iter, result);
        return true;
    }
};

namespace make {

template <class Rule>
OptionalRule<Rule> optional(const Rule& rule)
{
    return OptionalRule<Rule>(rule);
}

template <class Input, class Token, class Rule>
OptionalRule<Rule, Input, Token> optional(const Rule& rule)
{
    return OptionalRule<Rule, Input, Token>(rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_OPTIONALRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
