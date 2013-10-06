#ifndef SPROUT_RULE_LITERAL_HEADER
#define SPROUT_RULE_LITERAL_HEADER

#include "RuleTraits.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#ifdef HAVE_QT_CORE
#include <QChar>
#endif

#include <vector>
#include <algorithm>

namespace sprout {
namespace rule {

struct OrderedTokenMatcher
{
    template <class Input>
    static bool parse(Cursor<Input>& iter, const std::vector<Input>& target)
    {
        int i = 0;
        while (i < target.size()) {
            if (!iter || *iter++ != target[i++]) {
                return false;
            }
        }
        return true;
    }
};

struct AnyTokenMatcher
{
    template <class Input>
    static bool parse(Cursor<Input>& iter, const std::vector<Input>& target)
    {
        if (!iter) {
            return false;
        }
        auto value = *iter++;
        for (auto candidate : target) {
            if (value == candidate) {
                return true;
            }
        }
        return false;
    }
};

/**
 * Token is a rule that matches inputs against a vector of tokens. It
 * can be parameterized to support different matching algorithms, such as
 * "Match a single input against this group of candidates" or "Match an
 * ordered sequence of inputs against an ordered sequence of tokens"
 *
 * This class is a workhorse, but it strikes me as surprisingly limited
 * in its scope. The returned token is mandated, which relegates Token
 * to not being able to generate tokens with state. Since this limitation is
 * rather severe, Token will likely be changed in the future with a
 * different API.
 */
template <class Input, class Token, class Matcher>
class Literal : public RuleTraits<Input, Token>
{
    std::vector<Input> _target;
    Token _token;

public:
    Literal() :
        _target(),
        _token()
    {
    }

    template <class Target>
    Literal(const Target& target)
    {
        setTarget(target);
    }

    template <class Target, class Output>
    Literal(const Target& target, const Output& output) :
        _token(output)
    {
        setTarget(target);
    }

    void setTarget(const std::vector<Input>& target)
    {
        _target = target;
    }

    template <class Iterator>
    void setTarget(const Iterator& begin, const Iterator& end)
    {
        _target.clear();
        std::copy(begin, end, std::back_inserter(_target));
    }

    template <class Value>
    void setTarget(const Value& value)
    {
        setTarget(std::begin(value), std::end(value));
    }

    void setTarget(const char* value)
    {
        setTarget(std::string(value));
    }

    void setToken(const Token& token)
    {
        _token = token;
    }

    bool operator()(Cursor<Input>& orig, Result<Token>& result) const
    {
        auto iter = orig;
        if (!Matcher::parse(iter, _target)) {
            return false;
        }
        result.insert(_token);
        orig = iter;
        return true;
    }
};

template <class Input, class Token>
using OrderedLiteral = Literal<Input, Token, OrderedTokenMatcher>;

#ifdef HAVE_QT_CORE

template <class Token = QChar>
OrderedLiteral<QChar, Token> qLiteral(const QString& str)
{
    return OrderedLiteral<QChar, Token>(str);
}

template <class Token>
OrderedLiteral<QChar, Token> qLiteral(const QString& str, const Token& token)
{
    return OrderedLiteral<QChar, Token>(str, token);
}

#endif

template <class Input, class Token>
using AnyLiteral = Literal<Input, Token, AnyTokenMatcher>;

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_LITERAL_HEADER

// vim: set ft=cpp ts=4 sw=4 :
