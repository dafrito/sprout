#ifndef SPROUT_RULE_OPTIONAL_HEADER
#define SPROUT_RULE_OPTIONAL_HEADER

#include "RuleTraits.hpp"
#include "Sequence.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Optional : public RuleTraits<Input, Token>
{
    const Rule _rule;

public:
    Optional(const Rule& rule) :
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        _rule(iter, result);
        return true;
    }
};

template <class Rule>
Optional<Rule> optional(const Rule& rule)
{
    return Optional<Rule>(rule);
}

template <class Input, class Token, class Rule>
Optional<Rule, Input, Token> optional(const Rule& rule)
{
    return Optional<Rule, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_OPTIONAL_HEADER

// vim: set ft=cpp ts=4 sw=4 :
