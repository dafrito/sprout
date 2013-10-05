#ifndef SPROUT_PREDICATERULE_HEADER
#define SPROUT_PREDICATERULE_HEADER

#include "Cursor.hpp"
#include "Result.hpp"
#include "RuleTraits.hpp"

namespace sprout {

template <class Input, class Token, class Matcher>
class PredicateRule : public RuleTraits<Input, Token>
{
    const Matcher _matcher;

public:
    PredicateRule(const Matcher& matcher) :
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

namespace make {

template <class Input, class Token = Input, class Matcher>
PredicateRule<Input, Token, Matcher> predicate(const Matcher& matcher)
{
    return PredicateRule<Input, Token, Matcher>(matcher);
}

template <class Input, class Token = Input, class Tester>
PredicateRule<Input, Token, SimplePredicate<Input, Token, Tester>> simplePredicate(const Tester& tester)
{
    SimplePredicate<Input, Token, Tester> matcher(tester);
    PredicateRule<Input, Token, decltype(matcher)> rule(matcher);
    return rule;
}

} // namespace make

} // namespace sprout

#endif // SPROUT_PREDICATERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :