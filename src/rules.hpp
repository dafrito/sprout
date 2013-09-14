#ifndef SPROUT_RULES_HEADER
#define SPROUT_RULES_HEADER

#include "Cursor"
#include "Result"
#include "RuleTraits"

namespace sprout {
namespace rule {

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

bool _quotedString(Cursor<QChar>& input, Result<QString>& result);
auto quotedString = make::rule<QChar, QString>(&_quotedString);

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULES_HEADER

// vim: set ts=4 sw=4 :
