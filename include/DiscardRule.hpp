#ifndef SPROUT_DISCARDRULE_HEADER
#define SPROUT_DISCARDRULE_HEADER

#include <vector>
#include <algorithm>

#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

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
class DiscardRule : public RuleTraits<Input, Token>
{
    const Rule _rule;

    mutable Result<Token> trash;

public:
    DiscardRule(const Rule& rule) :
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

namespace make {

template <class Rule>
DiscardRule<Rule> discard(const Rule& rule)
{
    return DiscardRule<Rule>(rule);
}

template <class Input, class Token, class Rule>
DiscardRule<Rule, Input, Token> discard(const Rule& rule)
{
    return DiscardRule<Rule, Input, Token>(rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_DISCARDRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
