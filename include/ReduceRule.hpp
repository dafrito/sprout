#ifndef SPROUT_REDUCERULE_HEADER
#define SPROUT_REDUCERULE_HEADER

#include "Cursor.hpp"
#include "Result.hpp"
#include "RuleTraits.hpp"

namespace sprout {

template <
    class Rule,
    class Reducer,
    class Token,
    class Input = typename Rule::input_type,
    class SubToken = typename Rule::token_type
>
class ReduceRule : public RuleTraits<Input, Token>
{
    typedef SubToken subtoken_type;

    const Rule _rule;
    const Reducer _reducer;

public:
    ReduceRule(const Rule& rule, const Reducer& reducer) :
        _rule(rule),
        _reducer(reducer)
    {
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        Result<SubToken> subresults;
        if (!_rule(orig, subresults)) {
            return false;
        }
        _reducer(result, subresults);
        return true;
    }
};

namespace make {

template <class Token, class Rule, class Reducer>
ReduceRule<Rule, Reducer, Token> reduce(const Rule& rule, const Reducer& reducer)
{
    return ReduceRule<Rule, Reducer, Token>(rule, reducer);
}

template <class Token, class Input, class SubToken, class Rule, class Reducer>
ReduceRule<Rule, Reducer, Token, Input, SubToken> reduce(const Rule& rule, const Reducer& reducer)
{
    return ReduceRule<Rule, Reducer, Token, Input, SubToken>(rule, reducer);
}

template <class Reducer, class Token>
class AggregatingReducer
{
    const Reducer _reducer;
public:
    AggregatingReducer (const Reducer& reducer) :
        _reducer(reducer)
    {
    }

    template <class SubToken>
    void operator()(Result<Token>& result, Result<SubToken>& subtokens) const
    {
        Token aggregate;
        for (auto token : subtokens) {
            _reducer(aggregate, token);
        }
        result.insert(aggregate);
    }
};

template <class Token, class Rule, class Reducer>
ReduceRule<Rule, AggregatingReducer<Reducer, Token>, Token>
aggregate(const Rule& rule, const Reducer& reducer)
{
    return reduce<Token>(rule, AggregatingReducer<Reducer, Token>(reducer));
}

template <class Token, class Input, class SubToken, class Rule, class Reducer>
ReduceRule<Rule, AggregatingReducer<Reducer, Token>, Token, Input, SubToken>
aggregate(const Rule& rule, const Reducer& reducer)
{
    return reduce<Token, Input, SubToken>(rule, AggregatingReducer<Reducer, Token>(reducer));
}

template <class Reducer, class Token>
class ConvertingReducer
{
    const Reducer _reducer;
public:
    ConvertingReducer (const Reducer& reducer) :
        _reducer(reducer)
    {
    }

    template <class SubToken>
    void operator()(Result<Token>& result, Result<SubToken>& subtokens) const
    {
        for (auto token : subtokens) {
            result.insert(_reducer(token));
        }
    }
};

template <class Token, class Rule, class Reducer>
ReduceRule<Rule, ConvertingReducer<Reducer, Token>, Token>
convert(const Rule& rule, const Reducer& reducer)
{
    return reduce<Token>(rule, ConvertingReducer<Reducer, Token>(reducer));
}

template <class Token, class Input, class SubToken, class Rule, class Reducer>
ReduceRule<Rule, ConvertingReducer<Reducer, Token>, Token, Input, SubToken>
convert(const Rule& rule, const Reducer& reducer)
{
    return reduce<Token, Input, SubToken>(rule, ConvertingReducer<Reducer, Token>(reducer));
}

} // namespace make

} // namespace sprout

#endif // SPROUT_REDUCERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :