#ifndef SPROUT_RULE_CATCHING_HEADER
#define SPROUT_RULE_CATCHING_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

/**
 * \brief A rule that catches a specified exception, using it to indicate failure.
 */
template <
    class Rule,
    class Exception,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Catching : public RuleTraits<Input, Token>
{
    Rule _rule;

public:
    Catching(const Rule& rule) :
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        try {
            return _rule(iter, result);
        } catch (const Exception& ex) {
            return false;
        }
    }
};

template <class Exception, class Rule>
Catching<Rule, Exception> catching(const Rule& rule)
{
    return Catching<Rule, Exception>(rule);
}

template <class Exception, class Input, class Token, class Rule>
Catching<Rule, Exception, Input, Token> catching(const Rule& rule)
{
    return Catching<Rule, Exception, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_CATCHING_HEADER

// vim: set ft=cpp ts=4 sw=4 :
