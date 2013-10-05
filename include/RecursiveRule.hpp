#ifndef SPROUT_RECURSIVERULE_HEADER
#define SPROUT_RECURSIVERULE_HEADER

#include <vector>
#include <algorithm>

#include "RuleTraits.hpp"
#include "SequenceRule.hpp"
#include "Cursor.hpp"
#include "Result.hpp"

namespace sprout {

template <
    class Terminal,
    class Recursive,
    class Merger,
    class Input = typename Terminal::input_type,
    class Token = typename Terminal::token_type
>
class RecursiveRule : public RuleTraits<Input, Token>
{
    const Terminal _terminal;
    const Recursive _recursive;

    const Merger _merger;

public:
    RecursiveRule(const Terminal& terminal, const Recursive& recursive, const Merger& merger) :
        _terminal(terminal),
        _recursive(recursive),
        _merger(merger)
    {
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& cumulative) const
    {
        auto iter = orig;
        bool success = false;

        Result<Token> result;
        if (!_terminal(iter, result)) {
            return false;
        }
        while (_recursive(iter, result)) {
            success = true;
            _merger(result);
        }

        if (success) {
            orig = iter;
            cumulative.insert(result);
        }

        return success;
    }
};

namespace make {

template <class Terminal, class Recursive, class Merger>
RecursiveRule<Terminal, Recursive, Merger> recursive(const Terminal& terminal, const Recursive& recursive, const Merger& merger)
{
    return RecursiveRule<Terminal, Recursive, Merger>(terminal, recursive, merger);
}

template <class Input, class Token, class Terminal, class Recursive, class Merger>
RecursiveRule<Terminal, Recursive, Merger, Input, Token> recursive(const Terminal& terminal, const Recursive& recursive, const Merger& merger)
{
    return RecursiveRule<Terminal, Recursive, Merger, Input, Token>(terminal, recursive, merger);
}

} // namespace make

} // namespace sprout

#endif // SPROUT_RECURSIVERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :