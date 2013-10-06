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
    class Consumer,
    class Condition,
    class Input = typename Consumer::input_type,
    class Token = typename Consumer::token_type
>
class Lazy : public RuleTraits<Input, Token>
{
    Consumer _consumer;
    Condition _condition;

public:
    Lazy(const Consumer& consumer, const Condition& condition) :
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

template <class Input, class Token, class Consumer, class Condition>
Lazy<Consumer, Condition, Input, Token> lazy(const Consumer& consumer, const Condition& condition)
{
    return Lazy<Consumer, Condition, Input, Token>(consumer, condition);
}

template <class Consumer, class Condition>
Lazy<Consumer, Condition> lazy(const Consumer& consumer, const Condition& condition)
{
    return Lazy<Consumer, Condition>(consumer, condition);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_LAZY_HEADER

// vim: set ft=cpp ts=4 sw=4 :
