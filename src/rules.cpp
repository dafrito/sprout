#include "rules.hpp"

#include <QChar>
#include <QString>
#include <cmath>
#include <TokenRule>
#include <DiscardRule>

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

bool parseInteger(Cursor<QChar>& orig, Result<long>& tokens)
{
    auto iter = orig;

    if (!iter) {
        return false;
    }

    int sign = 1;
    long cumulative = 0;

    if (*iter == '-') {
        sign = -1;
        ++iter;
    }

    bool found = false;
    while (iter) {
        auto digit = (*iter).digitValue();
        if (digit == -1) {
            break;
        }
        found = true;

        cumulative *= 10;
        cumulative += digit;

        ++iter;
    }

    if (!found) {
        return false;
    }

    orig = iter;
    tokens << (sign * cumulative);
    return true;
}

bool parseFloating(Cursor<QChar>& orig, Result<double>& tokens)
{
    auto iter = orig;

    if (!iter) {
        return false;
    }

    Result<long> nums;

    long whole = 0;
    if (*iter != '.') {
        if (parseInteger(iter, nums)) {
            whole = *nums++;
        } else {
            return false;
        }
    }

    double fractionValue = 0;
    if (iter && *iter == '.') {
        ++iter;

        long fraction = 0;
        int magnitude = 0;
        while (iter) {
            auto digit = (*iter).digitValue();
            if (digit < 0) {
                break;
            }
            fraction *= 10;
            fraction += digit;
            ++magnitude;
            ++iter;
        }

        if (magnitude == 0) {
            // Nothing in the fractional component was found, so fail
            return false;
        }

        fractionValue = fraction * std::pow(10, -magnitude);
    }

    int exponent = 0;
    if (iter && (*iter == 'e' || *iter == 'E')) {
        ++iter;

        if (*iter == '+') {
            ++iter;
        }

        if (!parseInteger(iter, nums)) {
            return false;
        }

        exponent = *nums;
    }

    orig = iter;
    tokens << (whole + fractionValue) * std::pow(10, exponent);
    return true;
}

ProxyRule<QChar, QString> lineComment(const QString& delimiter)
{
    auto delimiterRule = make::discard(OrderedTokenRule<QChar, QString>(delimiter));
    return [delimiterRule](Cursor<QChar>& iter, Result<QString>& result) {
        if (!delimiterRule(iter, result)) {
            return false;
        }
        QString str;
        while (iter && *iter != '\n') {
            str += *iter;
            ++iter;
        }
        result << str;
        return true;
    };
}

} // namespace rule
} // namespace sprout


// vim: set ts=4 sw=4 :
