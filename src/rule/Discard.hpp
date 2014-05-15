#ifndef SPROUT_RULE_DISCARD_HEADER
#define SPROUT_RULE_DISCARD_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

/**
 * \brief A rule that ignores any results from its subrule.
 *
 * Runs the specified rule, but discards any matches. The cursor will be
 * moved on successful matches, so this is useful for skipping unnecessary
 * content like whitespace. The success of the subrule is forwarded to
 * this rule, and the iterator will be moved accordingly.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Discard : public RuleTraits<Input, Token>
{
    const Rule _rule;

    mutable Result<Token> trash;

public:
    Discard(const Rule& rule) :
        _rule(rule)
    {
        trash.suppress();
    }

    template <class Ignored>
    bool operator()(Cursor<Input>& iter, Result<Ignored>& result) const
    {
        return _rule(iter, trash);
    }
};

template <class Rule>
Discard<Rule> discard(const Rule& rule)
{
    return Discard<Rule>(rule);
}

template <class Input, class Token, class Rule>
Discard<Rule, Input, Token> discard(const Rule& rule)
{
    return Discard<Rule, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_DISCARD_HEADER

// vim: set ft=cpp ts=4 sw=4 :
