#ifndef SPROUT_PROXYRULE_HEADER
#define SPROUT_PROXYRULE_HEADER

#include <memory>

#include "Cursor.hpp"
#include "Result.hpp"
#include "SequenceRule.hpp"
#include "AlternativeRule.hpp"
#include "LazyRule.hpp"
#include "RuleTraits.hpp"

namespace sprout {

template <class Input, class Token>
class RulePlaceholder {
public:
    virtual bool operator()(Cursor<Input>& iter, Result<Token>& result) const=0;
};

template <class Rule, class Input, class Token>
class RulePlaceholderImpl : public RulePlaceholder<Input, Token>
{
    Rule _rule;

public:
    RulePlaceholderImpl(const Rule& rule) :
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        return _rule(iter, result);
    }
};

/**
 * ProxyRule wraps a specified rule, and proxies all parsing and results
 * to it. The reason to do this is to allow containers of heterogenous
 * rule types, which is a common need. In this way, ProxyRule is similar
 * to std::function.
 */
template <class Input, class Token>
class ProxyRule : public RuleTraits<Input, Token>
{
    std::shared_ptr<RulePlaceholder<Input, Token>> _rule;

public:
    ProxyRule() = default;

    ProxyRule(const ProxyRule& other) :
        _rule(other._rule)
    {
    }

    template <class Rule, typename std::enable_if<
        !std::is_same<ProxyRule<Input, Token>, Rule>::value, int
    >::type = 0>
    ProxyRule(const Rule& rule) :
        _rule(new RulePlaceholderImpl<Rule, Input, Token>(rule))
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        if (!_rule) {
            return false;
        }
        return (*_rule)(iter, result);
    }

    template <class Rule>
    ProxyRule& operator=(const Rule& other)
    {
        _rule = decltype(_rule)(new RulePlaceholderImpl<Rule, Input, Token>(other));
        return *this;
    }

    explicit operator bool() const
    {
        return static_cast<bool>(_rule);
    }
};

namespace make {

template <class Input, class Token>
using ProxySequenceRule = SequenceRule<ProxyRule<Input, Token>>;

template <class Input, class Token, typename... Values>
ProxySequenceRule<Input, Token> proxySequence(Values... rest)
{
    ProxySequenceRule<Input, Token> sequence;
    populate(sequence, rest...);
    return sequence;
}

template <class Input, class Token>
using ProxyAlternativeRule = AlternativeRule<ProxyRule<Input, Token>>;

template <class Input, class Token, typename... Rules>
ProxyAlternativeRule<Input, Token> proxyAlternative(Rules... rest)
{
    ProxyAlternativeRule<Input, Token> alternative;
    populate(alternative, rest...);
    return alternative;
}

template <class Input, class Token>
using ProxyLazyRule = LazyRule<ProxyRule<Input, Token>>;

template <class Input, class Token, class Consumer, class Condition>
ProxyLazyRule<Input, Token> proxyLazy(Consumer consumer, Condition condition)
{
    return ProxyLazyRule<Input, Token>(consumer, condition);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_PROXYRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :