#ifndef SPROUT_ALTERNATIVERULE_HEADER
#define SPROUT_ALTERNATIVERULE_HEADER

#include <vector>

#include "composite.hpp"
#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

/**
 * \brief A rule that provides multiple choices for matching.
 *
 * AlternativeRule matches input against an ordered list of candidate rules.
 * The first rule that matches will provide AlternativeRule's result. The order
 * of alternatives will determine the precedence of those rules; this rule
 * makes no attempt to match the "longest" match, nor will it attempt to detect
 * if a rule is shadowed by a previous rule.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class AlternativeRule : public RuleTraits<Input, Token>
{
    std::vector<Rule> _rules;

public:
    AlternativeRule()
    {
    }

    template <class Container>
    AlternativeRule(const Container& rules) :
        _rules(rules)
    {
    }

    template <class T>
    AlternativeRule& operator<<(const T& rule)
    {
        _rules.push_back(rule);
        return *this;
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        for (auto rule : _rules) {
            if (rule(iter, result)) {
                return true;
            }
        }
        return false;
    }
};

namespace make {

template <class Rule, typename... Rules>
AlternativeRule<Rule> alternative(const Rule& rule, Rules... rest)
{
    AlternativeRule<Rule> alternative;
    populate(alternative, rule, rest...);
    return alternative;
}

template <class Input, class Token, class Rule, typename... Rules>
AlternativeRule<Rule, Input, Token> alternative(const Rule& rule, Rules... rest)
{
    AlternativeRule<Rule, Input, Token> alternative;
    populate(alternative, rule, rest...);
    return alternative;
}

} // namespace make

} // namespace sprout

#endif // SPROUT_ALTERNATIVERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :