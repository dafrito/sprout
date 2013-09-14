#ifndef SPROUT_RULES_HEADER
#define SPROUT_RULES_HEADER

#include "Cursor"
#include "Result"
#include "RuleTraits"

namespace sprout {
namespace rule {

bool _whitespace(Cursor<QChar>& input, Result<QString>& result);
auto whitespace = make::rule<QChar, QString>(&_whitespace);

bool _quotedString(Cursor<QChar>& input, Result<QString>& result);
auto quotedString = make::rule<QChar, QString>(&_quotedString);

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULES_HEADER

// vim: set ts=4 sw=4 :
