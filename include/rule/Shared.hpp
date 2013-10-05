#ifndef SPROUT_RULE_SHARED_HEADER
#define SPROUT_RULE_SHARED_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <memory>
#include <algorithm>

namespace sprout {
namespace rule {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Shared : public RuleTraits<Input, Token>
{
    std::shared_ptr<Rule> _rule;

public:
    Shared() :
        _rule(new Rule)
    {
    }

    Shared(const Rule& rule) :
        _rule(new Rule(rule))
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        return (*_rule)(iter, result);
    }

    template <class InternalRule>
    Shared& operator=(const InternalRule& other)
    {
        *_rule = other;
        return *this;
    }

    template <class T>
    Shared& operator<<(T value)
    {
        (*_rule) << value;
        return *this;
    }

    Rule* operator->()
    {
        return _rule.get();
    }

    Rule& operator*()
    {
        return *_rule;
    }

    explicit operator bool() const
    {
        return static_cast<bool>(_rule);
    }
};

template <class Rule>
Shared<Rule> shared(const Rule& rule)
{
    return Shared<Rule>(rule);
}

template <class Input, class Token, class Rule>
Shared<Rule, Input, Token> shared(const Rule& rule)
{
    return Shared<Rule, Input, Token>(rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_SHARED_HEADER

// vim: set ft=cpp ts=4 sw=4 :
