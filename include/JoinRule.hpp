#ifndef SPROUT_JOINRULE_HEADER
#define SPROUT_JOINRULE_HEADER

#include <vector>
#include <algorithm>

#include "RuleTraits.hpp"
#include "SequenceRule.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

template <
    class Rule,
    class SeparatorRule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class JoinRule : public RuleTraits<Input, Token>
{
    const Rule _content;
    const SeparatorRule _separator;

public:
    JoinRule(const Rule& content, const SeparatorRule& separator) :
        _content(content),
        _separator(separator)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        if (!_content(iter, result)) {
            return false;
        }
        auto saved = iter;
        auto head = result.head();
        while (_separator(iter, result)) {
            if (_content(iter, result)) {
                saved = iter;
                head = result.head();
                continue;
            }
            // Matched the separator, but didn't match the content
            // so disregard the separator match and return
            iter = saved;
            result.moveHead(head);
            return true;
        }
        return true;
    }
};

namespace make {

template <class Rule, class SeparatorRule>
JoinRule<Rule, SeparatorRule>
join(const Rule& content, const SeparatorRule& separator)
{
    return JoinRule<Rule, SeparatorRule>(content, separator);
}

template <class Input, class Token, class Rule, class SeparatorRule>
JoinRule<Rule, SeparatorRule, Input, Token>
join(const Rule& content, const SeparatorRule& separator)
{
    return JoinRule<Rule, SeparatorRule, Input, Token>(content, separator);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_JOINRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
