#ifndef SPROUT_RULE_RECURSIVE_HEADER
#define SPROUT_RULE_RECURSIVE_HEADER

#include "Sequence.hpp"
#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

template <
    class Terminal,
    class Recursor,
    class Merger,
    class Input = typename Terminal::input_type,
    class Token = typename Terminal::token_type
>
class Recursive : public RuleTraits<Input, Token>
{
    const Terminal _terminal;
    const Recursor _recursor;

    const Merger _merger;

public:
    Recursive(const Terminal& terminal, const Recursor& recursor, const Merger& merger) :
        _terminal(terminal),
        _recursor(recursor),
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
        while (_recursor(iter, result)) {
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

template <class Terminal, class Recursor, class Merger>
Recursive<Terminal, Recursor, Merger> recursive(const Terminal& terminal, const Recursor& recursor, const Merger& merger)
{
    return Recursive<Terminal, Recursor, Merger>(terminal, recursor, merger);
}

template <class Input, class Token, class Terminal, class Recursor, class Merger>
Recursive<Terminal, Recursor, Merger, Input, Token> recursive(const Terminal& terminal, const Recursor& recursor, const Merger& merger)
{
    return Recursive<Terminal, Recursor, Merger, Input, Token>(terminal, recursor, merger);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_RECURSIVE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
