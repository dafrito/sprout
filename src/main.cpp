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
        [](Result<QString>& result, Result<QString>& subresults) {
            QString aggregate;
            for (auto c : subresults) {
                aggregate += c;
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
        auto cursor = makeCursor<QChar>(&lineStream);
        Result<QString> results;

        if (definition.parse(cursor, results)) {
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
