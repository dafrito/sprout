#ifndef SPROUT_LOGRULE_HEADER
#define SPROUT_LOGRULE_HEADER

#include "Cursor.hpp"
#include "Result.hpp"
#include "RuleTraits.hpp"

namespace sprout {

template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class LogRule : public RuleTraits<Input, Token>
{
    const std::string _name;
    const Rule _rule;

public:
    LogRule(const std::string& name, const Rule& rule) :
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

namespace make {

template <class Rule>
LogRule<Rule> log(const std::string& name, const Rule& rule)
{
    return LogRule<Rule>(name, rule);
}

template <class Input, class Token, class Rule>
LogRule<Rule, Input, Token> log(const std::string& name, const Rule& rule)
{
    return LogRule<Rule, Input, Token>(name, rule);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_LOGRULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
