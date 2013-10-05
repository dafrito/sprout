#ifndef SPROUT_RULES_HEADER
#define SPROUT_RULES_HEADER

#include "RuleTraits.hpp"
#include "Proxy.hpp"

#include "../Cursor.hpp"
#include "../Result.hpp"

#include <iostream>

namespace sprout {
namespace rule {

template <class Token>
bool parseNeighborhood(Cursor<QChar>& orig, Result<Token>& results)
{
    auto iter = orig;
    QString str;
    QString indicator;

    const int NEIGHBOR_SIZE = 10;

    iter -= NEIGHBOR_SIZE;
    for (int i = 0; iter && i < NEIGHBOR_SIZE * 2 + 1; ++i) {
        auto c = *iter++;

        int size = 1;
        if (c == '\n') {
            str += "\\n";
            size = 2;
        } else if (c == '\t') {
            str += "\\t";
            size = 2;
        } else {
            str += c;
        }

        for (int j = 0; j < size; ++j) {
            indicator += i == NEIGHBOR_SIZE ? '^' : ' ';
        }
    }
    std::cout << str.toUtf8().constData() << std::endl;
    std::cout << indicator.toUtf8().constData() << std::endl;
    return true;
}

template <class Token>
auto neighborhood() -> decltype(rule<QChar, Token>(&parseNeighborhood<Token>))
{
    return rule<QChar, Token>(&parseNeighborhood<Token>);
}

template <class Token>
bool parseWhitespace(Cursor<QChar>& iter, Result<Token>& result)
{
    bool found = false;
    while (iter) {
        auto input = *iter;
        if (!input.isSpace()) {
            break;
        }
        found = true;
        ++iter;
    }
    return found;
}

template <class Token>
auto whitespace() -> decltype(rule<QChar, Token>(&parseWhitespace<Token>))
{
    return rule<QChar, Token>(&parseWhitespace<Token>);
}

bool parseQuotedString(Cursor<QChar>& input, Result<QString>& result);

Proxy<QChar, QString> lineComment(const QString& delimiter);

Proxy<QChar, QString> variable();

bool parseInteger(Cursor<QChar>& input, Result<long>& result);

bool parseFloating(Cursor<QChar>& input, Result<double>& result);

template <class Input, class Token>
bool EndRule(Cursor<Input>& iter, Result<Token>& result)
{
    return !iter;
}

template <class Input, class Token>
bool AnyRule(Cursor<Input>& iter, Result<Token>& result)
{
    if (!iter) {
        return false;
    }
    result << *iter;
    ++iter;
    return true;
}

template <class Input, class Token>
Rule<Input, Token> end()
{
    return EndRule<Input, Token>;
}

template <class Input, class Token>
Rule<Input, Token> any()
{
    return AnyRule<Input, Token>;
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULES_HEADER

// vim: set ts=4 sw=4 :
