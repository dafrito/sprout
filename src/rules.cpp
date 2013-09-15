#include "rules.hpp"

#include <QChar>
#include <QString>

namespace sprout {
namespace rule {

bool parseQuotedString(Cursor<QChar>& orig, Result<QString>& result)
{
    auto iter = orig;

    if (!iter) {
        return false;
    }
    auto quote = *iter++;

    if (quote != '\'' && quote != '"') {
        return false;
    }

    QString str;
    while (iter) {
        auto c = *iter++;

        if (c == quote) {
            result.insert(str);
            orig = iter;
            return true;
        }

        if (c == '\\') {
            // Escaped string, so parse it accordingly
            if (!iter) {
                break;
            }

            auto escape = *iter++;
            if (escape == 'n') {
                c = '\n';
            } else if (escape == 'b') {
                c = '\b';
            } else if (escape == 'f') {
                c = '\f';
            } else if (escape == 'r') {
                c = '\r';
            } else if (escape == 't') {
                c = '\t';
            } else if (escape == 'u') {
                // Unicode
                QString unicodeValue;
                for (int i = 0; i < 4; ++i) {
                    if (!iter) {
                        return false;
                    }
                    unicodeValue += *iter++;
                }
                bool ok;
                auto realValue = unicodeValue.toInt(&ok, 16);
                if (!ok) {
                    return false;
                }
                c = QChar(realValue);
            }
        }

        str += c;
    }

    // Ran out of data before we could complete the string
    return false;
}

} // namespace rule
} // namespace sprout


// vim: set ts=4 sw=4 :
