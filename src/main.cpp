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
    auto whitespace = makeDiscard(
        makeMultiple(makeSimplePredicate<QChar, QString>([](const QChar& input) {
            return input.isSpace();
        }))
    );

    auto name = makeSimpleReduce<QString>(
        [](QString& aggregate, const QChar& c) {
            aggregate += c;
        },
        makeMultiple(makeSimplePredicate<QChar>([](const QChar& input) {
            return input.isLetter() || input == '_';
        }))
    );

    auto definition = makeReduce<QString>(
        [](Result<QString>& result, Result<QString>& begin, const Result<QString>& end) {
            QString aggregate;
            for (auto i = begin; i != end; ++i) {
                aggregate += *i;
            }

            result.insert(aggregate);
        },
        makeProxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("var"),
            whitespace,
            name
        )
    );

    auto parser = makeMultiple(
        makeProxySequence<QChar, QString>(
            whitespace,
            makeProxyAlternate<QChar, QString>(
                OrderedTokenRule<QChar, QString>("Cat", "Heathen"),
                OrderedTokenRule<QChar, QString>("Dog", "Civilized"),
                OrderedTokenRule<QChar, QString>("Calf", "Cow"),
                name
            ),
            whitespace
        )
    );

    QTextStream stream(stdin);
    QString line;
    while (true) {
        line = stream.readLine();
        if (line.isNull()) {
            break;
        }
        QTextStream lineStream(&line);
        auto cursor = makeCursor<QChar>(lineStream);

        auto tokens = definition.parse(cursor);
        if (tokens) {
            std::cout << "I came up with ";
            while (tokens) {
                std::cout << tokens->toUtf8().constData() << " ";
                ++tokens;
            }
            std::cout << std::endl;
        } else {
            std::cout << "I didn't match anything. :(" << std::endl;
        }
    }

    return 0;
}

// vim: set ts=4 sw=4 :
