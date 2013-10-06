#ifndef SPROUT_RULE_SEQUENCE_HEADER
#define SPROUT_RULE_SEQUENCE_HEADER

#include "composite.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <tuple>

namespace sprout {
namespace rule {

template <class Tuple, class Input, class Token, int remaining>
struct SequenceIterator
{
    static bool iterate(const Tuple& rules, Cursor<Input>& iter, Result<Token>& result)
    {
        if (!std::get<std::tuple_size<Tuple>::value - remaining>(rules)(iter, result)) {
            return false;
        }
        return SequenceIterator<Tuple, Input, Token, remaining - 1>::iterate(rules, iter, result);
    }
};

template <class Tuple, class Input, class Token>
struct SequenceIterator<Tuple, Input, Token, 0>
{
    static bool iterate(const Tuple& rules, Cursor<Input>& iter, Result<Token>& result)
    {
        return true;
    }
};

template <
    class Input,
    class Token,
    class... Rules
>
class TupleSequence : public RuleTraits<Input, Token>
{
    std::tuple<Rules...> _rules;

public:
    TupleSequence(Rules... rules) :
        _rules(rules...)
    {
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        auto iter = orig;
        auto head = result.head();
        auto matched = SequenceIterator<
                const decltype(_rules),
                Input,
                Token,
                std::tuple_size<decltype(_rules)>::value
            >::iterate(
            _rules, iter, result
        );
        if (!matched) {
            result.moveHead(head);
            return false;
        }
        return true;
    }
};

/**
 * \brief An ordered list of subrules.
 *
 * Sequence matches input against an ordered list of subrules.
 * All required subrules must match for Sequence to match. If
 * any subrule fails to match, the iterator is reset to its original
 * position.
 */
template <
    class Rule,
    class Input = typename Rule::input_type,
    class Token = typename Rule::token_type
>
class Sequence : public RuleTraits<Input, Token>
{
    std::vector<Rule> _rules;

public:
    template <class Container>
    Sequence(const Container& rules)
    {
        for (auto rule : rules) {
            *this << rule;
        }
    }

    Sequence()
    {
    }

    template <class T>
    Sequence& operator<<(const T& rule)
    {
        _rules.push_back(rule);
        return *this;
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        auto iter = orig;
        auto head = result.head();
        for (auto rule : _rules) {
            if (!rule(iter, result)) {
                result.moveHead(head);
                return false;
            }
        }
        orig = iter;
        return true;
    }
};

template <class Input, class Token, typename... Rules>
TupleSequence<Input, Token, Rules...> tupleSequence(Rules... rules)
{
    return TupleSequence<Input, Token, Rules...>(rules...);
}

template <class Input, class Token, class Rule, typename... Rules>
Sequence<Rule, Input, Token> sequence(const Rule& rule, Rules... rest)
{
    Sequence<Rule, Input, Token> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule, typename... Values>
Sequence<Rule> sequence(const Rule& rule, Values... rest)
{
    Sequence<Rule> sequence;
    populate(sequence, rule, rest...);
    return sequence;
}

template <class Rule>
Sequence<Rule> sequence()
{
    return Sequence<Rule>();
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_SEQUENCE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
