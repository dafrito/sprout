#ifndef SPROUT_SHAREDRULE_HEADER
#define SPROUT_SHAREDRULE_HEADER

#include <vector>
#include <memory>
#include <algorithm>

#include "RuleTraits.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class SharedRule : public RuleTraits<Input, Token>
{
    std::shared_ptr<Rule> _rule;

public:
    SharedRule() :
        _rule(new Rule)
    {
    }

    SharedRule(const Rule& rule) :
        _rule(new Rule(rule))
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        return (*_rule)(iter, result);
    }

    template <class InternalRule>
    SharedRule& operator=(const InternalRule& other)
    {
        *_rule = other;
        return *this;
    }

    template <class T>
    SharedRule& operator<<(T value)
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

namespace make {

template <class Rule>
SharedRule<Rule> shared(const Rule& rule)
{
    return SharedRule<Rule>(rule);
}

template <class Input, class Token, class Rule>
SharedRule<Rule, Input, Token> shared(const Rule& rule)
{
    return SharedRule<Rule, Input, Token>(rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_SHAREDRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
