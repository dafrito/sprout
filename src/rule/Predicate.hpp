#ifndef SPROUT_RULE_PREDICATE_HEADER
#define SPROUT_RULE_PREDICATE_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

namespace sprout {
namespace rule {

template <class Input, class Token, class Matcher>
class Predicate : public RuleTraits<Input, Token>
{
    const Matcher _matcher;

public:
    Predicate(const Matcher& matcher) :
        _matcher(matcher)
    {
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        if (!orig) {
            return false;
        }

        auto iter = orig;

        Token aggregate;
        auto candidate = *iter++;
        if (!_matcher(candidate, aggregate)) {
            return false;
        }

        orig = iter;
        result.insert(aggregate);
        return true;
    }
};

template <class Input, class Token, class Tester>
class SimplePredicate
{
    const Tester tester;

public:
    SimplePredicate(const Tester& tester) :
        tester(tester)
    {
    }

    bool operator()(const Input& input, Token& aggregate) const
    {
        if (!tester(input)) {
            return false;
        }
        aggregate = input;
        return true;
    }
};

template <class Input, class Token = Input, class Matcher>
Predicate<Input, Token, Matcher> predicate(const Matcher& matcher)
{
    return Predicate<Input, Token, Matcher>(matcher);
}

template <class Input, class Token = Input, class Tester>
Predicate<Input, Token, SimplePredicate<Input, Token, Tester>> simplePredicate(const Tester& tester)
{
    SimplePredicate<Input, Token, Tester> matcher(tester);
    Predicate<Input, Token, decltype(matcher)> rule(matcher);
    return rule;
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_PREDICATE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
