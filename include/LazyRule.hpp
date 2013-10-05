#ifndef SPROUT_LAZYRULE_HEADER
#define SPROUT_LAZYRULE_HEADER

#include <vector>

#include "composite.hpp"
#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class LazyRule : public RuleTraits<Input, Token>
{
    Rule _consumer;
    Rule _condition;

public:
    LazyRule(const Rule& consumer, const Rule& condition) :
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

namespace make {

template <class Input, class Token, class Rule>
LazyRule<Rule, Input, Token> lazy(const Rule& consumer, const Rule& condition)
{
    return LazyRule<Rule, Input, Token>(consumer, condition);
}

template <class Rule>
LazyRule<Rule> lazy(const Rule& consumer, const Rule& condition)
{
    return LazyRule<Rule>(consumer, condition);
}

} // namespace make
} // namespace sprout

#endif // SPROUT_LAZYRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
