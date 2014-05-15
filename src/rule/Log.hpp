#ifndef SPROUT_RULE_LOG_HEADER
#define SPROUT_RULE_LOG_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

namespace sprout {
namespace rule {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Log : public RuleTraits<Input, Token>
{
    const std::string _name;
    const Rule _rule;

public:
    Log(const std::string& name, const Rule& rule) :
        _name(name),
        _rule(rule)
    {
    }

    bool operator()(Cursor<Input>& iter, Result<Token>& result) const
    {
        auto rv = _rule(iter, result);
        if (rv) {
            std::cout << "Rule '" << _name << "' matched\n";
        } else {
            std::cout << "Rule '" << _name << "' failed\n";
        }
        return rv;
    }
};

template <class Rule>
Log<Rule> log(const std::string& name, const Rule& rule)
{
    return Log<Rule>(name, rule);
}

template <class Input, class Token, class Rule>
Log<Rule, Input, Token> log(const std::string& name, const Rule& rule)
{
    return Log<Rule, Input, Token>(name, rule);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_LOG_HEADER

// vim: set ft=cpp ts=4 sw=4 :
