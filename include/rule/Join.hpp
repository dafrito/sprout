#ifndef SPROUT_RULE_JOIN_HEADER
#define SPROUT_RULE_JOIN_HEADER

#include "rule/RuleTraits.hpp"
#include "rule/Sequence.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

template <
    class Rule,
    class SeparatorRule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Join : public RuleTraits<Input, Token>
{
    const Rule _content;
    const SeparatorRule _separator;

public:
    Join(const Rule& content, const SeparatorRule& separator) :
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

template <class Rule, class SeparatorRule>
Join<Rule, SeparatorRule>
join(const Rule& content, const SeparatorRule& separator)
{
    return Join<Rule, SeparatorRule>(content, separator);
}

template <class Input, class Token, class Rule, class SeparatorRule>
Join<Rule, SeparatorRule, Input, Token>
join(const Rule& content, const SeparatorRule& separator)
{
    return Join<Rule, SeparatorRule, Input, Token>(content, separator);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_JOIN_HEADER

// vim: set ft=cpp ts=4 sw=4 :
