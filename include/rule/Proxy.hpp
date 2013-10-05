#ifndef SPROUT_RULE_PROXY_HEADER
#define SPROUT_RULE_PROXY_HEADER

#include "Sequence.hpp"
#include "Alternative.hpp"
#include "Lazy.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <memory>

namespace sprout {
namespace rule {

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
 * Proxy wraps a specified rule, and proxies all parsing and results
 * to it. The reason to do this is to allow containers of heterogenous
 * rule types, which is a common need. In this way, Proxy is similar
 * to std::function.
 */
template <class Input, class Token>
class Proxy : public RuleTraits<Input, Token>
{
    std::shared_ptr<RulePlaceholder<Input, Token>> _rule;

public:
    Proxy() = default;

    Proxy(const Proxy& other) :
        _rule(other._rule)
    {
    }

    template <class Rule, typename std::enable_if<
        !std::is_same<Proxy<Input, Token>, Rule>::value, int
    >::type = 0>
    Proxy(const Rule& rule) :
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
    Proxy& operator=(const Rule& other)
    {
        _rule = decltype(_rule)(new RulePlaceholderImpl<Rule, Input, Token>(other));
        return *this;
    }

    explicit operator bool() const
    {
        return static_cast<bool>(_rule);
    }
};

template <class Input, class Token>
using ProxySequence = Sequence<Proxy<Input, Token>>;

template <class Input, class Token, typename... Values>
ProxySequence<Input, Token> proxySequence(Values... rest)
{
    ProxySequence<Input, Token> sequence;
    populate(sequence, rest...);
    return sequence;
}

template <class Input, class Token>
using ProxyAlternative = Alternative<Proxy<Input, Token>>;

template <class Input, class Token, typename... Rules>
ProxyAlternative<Input, Token> proxyAlternative(Rules... rest)
{
    ProxyAlternative<Input, Token> alternative;
    populate(alternative, rest...);
    return alternative;
}

template <class Input, class Token>
using ProxyLazy = Lazy<Proxy<Input, Token>>;

template <class Input, class Token, class Consumer, class Condition>
ProxyLazy<Input, Token> proxyLazy(Consumer consumer, Condition condition)
{
    return ProxyLazy<Input, Token>(consumer, condition);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_PROXY_HEADER

// vim: set ft=cpp ts=4 sw=4 :
