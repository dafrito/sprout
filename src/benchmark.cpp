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

QString runRegex(QString str, QRegExp& re)
{
    assert(re.indexIn(str, 0) == 0);
    auto matchedName = re.cap(1);
    return matchedName;
}

template <class Parser>
QString runSprout(QString str, Parser& parser)
{
    auto cursor = makeCursor<QChar>(str);

    return *parser.parse(cursor);
}

struct NameStruct {
    Result<QString> _eof;
    const Result<QString> end() const
    {
        return _eof;
    }

    const Result<QString> parse(Cursor<QChar>& orig) const
    {
        if (!orig) {
            return end();
        }

        auto iter = orig;
        QString aggr;
        while (iter) {
            auto c = *iter;
            if (c.isLetter() || c == '_') {
                aggr += c;
            } else {
                break;
            }
            ++iter;
        }
        orig = iter;
        return Result<QString>({aggr});
    }
};

struct WhitespaceStruct {
    Result<QString> _eof;
    const Result<QString> end() const
    {
        return _eof;
    }

    const Result<QString> parse(Cursor<QChar>& orig) const
    {
        if (!orig) {
            return end();
        }

        while (orig) {
            if (!orig->isSpace()) {
                break;
            }
            ++orig;
        }

        return end();
    }
};

int main()
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
            Optional,
            WhitespaceStruct(),
            Required,
            NameStruct()
        )
    );

    const int RUNS = 1e5;
    std::cout << "Running " << RUNS << " iteration" << (RUNS == 1 ? "" : "s") << "\n";

    {
        QRegExp re("^var\\s+(\\w+)");
        QElapsedTimer timer;
        timer.start();

        QString result;
        for (int i = 0; i < RUNS; ++i) {
            result = runRegex("var foo", re);
        }
        std::cout << "RegExp found " << result.toStdString() << " and took " << timer.elapsed() << " milliseconds\n";
    }

    {
        auto benchmark = makeSequence(
            OrderedTokenRule<QChar, QString>("var"),
            OrderedTokenRule<QChar, QString>(" "),
            OrderedTokenRule<QChar, QString>("foo", "foo")
        );
        QElapsedTimer timer;
        timer.start();

        QString result;
        for (int i = 0; i < RUNS; ++i) {
            result = runSprout("var foo", definition);
        }
        std::cout << "Sprout found " << result.toStdString() << " and took " << timer.elapsed() << " milliseconds\n";
    }

    {
        QElapsedTimer timer;
        timer.start();

        QString result;
        for (int i = 0; i < RUNS; ++i) {
            QString str("var foo");

            if (str.startsWith("var")) {
                int mark = 3;
                while (str.at(mark).isSpace()) {
                    ++mark;
                }
                QString aggr;
                while (mark < str.size() && str.at(mark).isLetter()) {
                    aggr += str.at(mark++);
                }
                result = aggr;
            }
        }
        std::cout << "Inline found " << result.toStdString() << " and took " << timer.elapsed() << " milliseconds\n";
    }

    return 0;
}

// vim: set ts=4 sw=4 :
