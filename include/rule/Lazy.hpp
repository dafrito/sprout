#ifndef SPROUT_RULE_LAZY_HEADER
#define SPROUT_RULE_LAZY_HEADER

#include "composite.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>

namespace sprout {
namespace rule {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Lazy : public RuleTraits<Input, Token>
{
    Rule _consumer;
    Rule _condition;

public:
    Lazy(const Rule& consumer, const Rule& condition) :
        _consumer(consumer),
        _condition(condition)
    {
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        auto iter = orig;
        while (!_condition(iter, result)) {
            if (!_consumer(iter, result)) {
                // Neither the conditon or the consumer matched, so fail
                return false;
            }
        }

        // Condition matched, so we match as a whole
        orig = iter;
        return true;
    }
};

template <class Input, class Token, class Rule>
Lazy<Rule, Input, Token> lazy(const Rule& consumer, const Rule& condition)
{
    return Lazy<Rule, Input, Token>(consumer, condition);
}

template <class Rule>
Lazy<Rule> lazy(const Rule& consumer, const Rule& condition)
{
    return Lazy<Rule>(consumer, condition);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_LAZY_HEADER

// vim: set ft=cpp ts=4 sw=4 :
