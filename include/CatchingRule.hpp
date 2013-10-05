#ifndef SPROUT_CATCHINGRULE_HEADER
#define SPROUT_CATCHINGRULE_HEADER

#include <vector>
#include <algorithm>

#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

/**
 * \brief A rule that catches a specified exception, using it to indicate failure.
 */
template <
    class Rule,
    class Exception,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class CatchingRule : public RuleTraits<Input, Token>
{
    Rule _rule;

public:
    CatchingRule(const Rule& rule) :
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

namespace make {

template <class Exception, class Rule>
CatchingRule<Rule, Exception> catching(const Rule& rule)
{
    return CatchingRule<Rule, Exception>(rule);
}

template <class Exception, class Input, class Token, class Rule>
CatchingRule<Rule, Exception, Input, Token> catching(const Rule& rule)
{
    return CatchingRule<Rule, Exception, Input, Token>(rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_CATCHINGRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
