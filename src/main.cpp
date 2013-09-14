#include <iostream>
#include <cassert>

#include <QChar>
#include <QTextStream>
#include <QString>

#include "StreamIterator"
#include "TokenRule"
#include "DiscardRule"
#include "MultipleRule"
#include "PredicateRule"
#include "ProxyRule"
#include "AlternativeRule"
#include "ReduceRule"

#include <QRegExp>
#include <QElapsedTimer>

using namespace sprout;

int main(int argc, char* argv[])
{
    using namespace make;

    auto whitespace = discard(
        multiple(simplePredicate<QChar, QString>([](const QChar& input) {
            return input.isSpace();
        }))
    );

    auto name = aggregate<QString>(
        [](QString& aggregate, const QChar& c) {
            aggregate += c;
        },
        multiple(simplePredicate<QChar>([](const QChar& input) {
            return input.isLetter() || input == '_';
        }))
    );

    auto definition = reduce<QString>(
        [](Result<QString>& result, Result<QString>& subresults) {
            QString aggregate;
            for (auto c : subresults) {
                aggregate += c;
            }

            result.insert(aggregate);
        },
        proxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("var"),
            whitespace,
            name
        )
    );

    auto quotedString = [](Cursor<QChar>& orig, Result<QString>& result) {
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
    };

    auto parser = multiple(
        proxySequence<QChar, QString>(
            definition,
            whitespace,
            OrderedTokenRule<QChar, QString>("="),
            whitespace,
            quotedString,
            whitespace
        )
    );

    QTextStream stream(stdin);
    stream.setCodec("UTF-8");

    QString line;
    while (true) {
        line = stream.readLine();
        if (line.isNull()) {
            break;
        }
        QTextStream lineStream(&line);
        auto cursor = makeCursor<QChar>(&lineStream);
        Result<QString> results;

        if (parser(cursor, results) && results) {
            std::cout << "I came up with ";
            while (results) {
                std::cout << results->toUtf8().constData() << " ";
                ++results;
            }
            std::cout << std::endl;
        } else {
            std::cout << "I didn't match anything. :(" << std::endl;
        }
    }

    return 0;
}

// vim: set ts=4 sw=4 :
