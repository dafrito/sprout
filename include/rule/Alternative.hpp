#ifndef SPROUT_RULE_ALTERNATIVE_HEADER
#define SPROUT_RULE_ALTERNATIVE_HEADER

#include "composite.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>

namespace sprout {
namespace rule {

/**
 * \brief A rule that provides multiple choices for matching.
 *
 * Alternative matches input against an ordered list of candidate rules.
 * The first rule that matches will provide Alternative's result. The order
 * of alternatives will determine the precedence of those rules; this rule
 * makes no attempt to match the "longest" match, nor will it attempt to detect
 * if a rule is shadowed by a previous rule.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Alternative : public RuleTraits<Input, Token>
{
    std::vector<Rule> _rules;

public:
    Alternative()
    {
    }

    template <class Container>
    Alternative(const Container& rules) :
        _rules(rules)
    {
    }

    template <class T>
    Alternative& operator<<(const T& rule)
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

template <class Rule, typename... Rules>
Alternative<Rule> alternative(const Rule& rule, Rules... rest)
{
    Alternative<Rule> alternative;
    populate(alternative, rule, rest...);
    return alternative;
}

template <class Input, class Token, class Rule, typename... Rules>
Alternative<Rule, Input, Token> alternative(const Rule& rule, Rules... rest)
{
    Alternative<Rule, Input, Token> alternative;
    populate(alternative, rule, rest...);
    return alternative;
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_ALTERNATIVE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
