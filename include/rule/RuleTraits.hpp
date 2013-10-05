#ifndef SPROUT_RULETRAITS_HEADER
#define SPROUT_RULETRAITS_HEADER

#include "../Cursor.hpp"
#include "../Result.hpp"

namespace sprout {
namespace rule {

template <class Input, class Token>
using Rule = bool (*)(Cursor<Input>&, Result<Token>&);

template <class Input, class Token>
struct RuleTraits {
    typedef Input input_type;
    typedef Token token_type;
};

template <
    class Rule,
    class Input,
    class Token
>
class InlineRule : public RuleTraits<Input, Token> {
    Rule _rule;

public:
    InlineRule(const Rule& rule) :
        _rule(rule)
    {
    }

    inline bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        return _rule(iter, result);
    }
};

template <class Input, class Token, class Rule>
InlineRule<Rule, Input, Token> rule(const Rule& rule)
{
    return InlineRule<Rule, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULETRAITS_HEADER

// vim: set ft=cpp ts=4 sw=4 :
