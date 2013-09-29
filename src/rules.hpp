#ifndef SPROUT_RULES_HEADER
#define SPROUT_RULES_HEADER

#include "Cursor"
#include "Result"
#include "RuleTraits"

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
auto neighborhood() -> decltype(make::rule<QChar, Token>(&parseNeighborhood<Token>))
{
    return make::rule<QChar, Token>(&parseNeighborhood<Token>);
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
auto whitespace() -> decltype(make::rule<QChar, Token>(&parseWhitespace<Token>))
{
    return make::rule<QChar, Token>(&parseWhitespace<Token>);
}

bool parseQuotedString(Cursor<QChar>& input, Result<QString>& result);
auto quotedString = make::rule<QChar, QString>(&parseQuotedString);

bool parseInteger(Cursor<QChar>& input, Result<long>& result);
auto integer = make::rule<QChar, long>(&parseInteger);

bool parseFloating(Cursor<QChar>& input, Result<double>& result);
auto floating = make::rule<QChar, double>(&parseFloating);

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULES_HEADER

// vim: set ts=4 sw=4 :
