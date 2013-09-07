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

const int RUNS = 1e5;

template <class Runner>
void runBenchmark(const char* name, Runner runner)
{
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < RUNS; ++i) {
        runner();
    }

    std::cout << name << " took " << timer.elapsed() << " ms\n";
}

int main()
{
    using namespace make;

    auto whitespace = discard(
        multiple(predicate<QChar, QString>([](const QChar& input, QString&) {
            return input.isSpace();
        }))
    );

    auto fastWhitespace = [](Cursor<QChar>& iter, Result<QString>& result) {
        while (iter) {
            auto input = *iter;
            if (!input.isSpace()) {
                break;
            }
            ++iter;
        }
        return true;
    };

    auto name = simpleReduce<QString>(
        [](QString& aggregate, const QChar& c) {
            aggregate += c;
        },
        multiple(simplePredicate<QChar>([](const QChar& input) {
            return input.isLetter() || input == '_';
        }))
    );

    auto fastName = [](Cursor<QChar>& orig, Result<QString>& result) {
        auto iter = orig;
        QString aggr;
        while (iter) {
            auto input = *iter++;
            if (input.isLetter() || input == '_') {
                aggr += input;
            } else {
                break;
            }
        }
        if (aggr.isNull()) {
            return false;
        }
        result.insert(aggr);
        orig = iter;
        return true;
    };

    std::cout << "Running " << RUNS << " iteration" << (RUNS == 1 ? "" : "s") << "\n";

    std::cout << std::endl;
    std::cout << "=== Parser Benchmarks ===\n";

    const QString inputString("var bar");
    const QString targetString("bar");

    {
        QRegExp re("^var\\s+(\\w+)");
        runBenchmark("RegExp", [&]() {
            assert(re.indexIn(inputString, 0) == 0);
            assert(re.cap(1) == targetString);
        });
    }

    {
        auto benchmark = proxySequence<QChar, QString>(
            discard(OrderedTokenRule<QChar, QString>("var")),
            whitespace,
            name
        );
        runBenchmark("Sprout", [&]() {
            auto cursor = makeCursor<QChar>(&inputString);
            Result<QString> results;

            assert(benchmark(cursor, results));
            assert(targetString == *results);
        });
    }

    {
        auto benchmark = proxySequence<QChar, QString>(
            discard(OrderedTokenRule<QChar, QString>("var")),
            fastWhitespace,
            fastName
        );
        runBenchmark("Spfast", [&]() {
            auto cursor = makeCursor<QChar>(&inputString);
            Result<QString> results;

            assert(benchmark(cursor, results));
            assert(targetString == *results);
        });
    }

    {
        runBenchmark("Inline", [&]() {
            QString result;

            if (!inputString.startsWith("var")) {
                assert(false);
            }
            int mark = 3;
            while (mark < inputString.size() && inputString.at(mark).isSpace()) {
                ++mark;
            }
            QString aggr;
            while (mark < inputString.size() && inputString.at(mark).isLetter()) {
                aggr += inputString.at(mark++);
            }
            assert(aggr == targetString);
        });
    }

    std::cout << std::endl;
    std::cout << "=== Direct Match Benchmarks ===\n";

    const QString simpleTarget("var");

    {
        QRegExp re("^var");
        runBenchmark("RegExp", [&]() {
            assert(re.indexIn(inputString, 0) == 0);
            assert(re.cap(0) == simpleTarget);
        });
    }

    {
        auto benchmark = OrderedTokenRule<QChar, QString>("var", "var");
        runBenchmark("Sprout", [&]() {
            auto cursor = makeCursor<QChar>(&inputString);
            Result<QString> results;

            assert(benchmark(cursor, results));
            assert(*results == simpleTarget);
        });
    }

    {
        QString varStr("var");
        auto benchmark = [&](Cursor<QChar>& orig, Result<QString>& results) {
            auto iter = orig;
            for (int i = 0; i < varStr.size(); ++i) {
                if (!iter || *iter++ != varStr.at(i)) {
                    return false;
                }
            }
            orig = iter;
            results.insert(varStr);
            return true;
        };
        runBenchmark("Spfast", [&]() {
            auto cursor = makeCursor<QChar>(&inputString);
            Result<QString> results;

            assert(benchmark(cursor, results));
            assert(*results == simpleTarget);
        });
    }

    std::cout << std::endl;
    std::cout << "=== Aggregating Match Benchmarks ===\n";

    {
        const QString inputString("varbar");
        const QString targetString("bar");

        {
            QRegExp re("^var(\\w+)");
            runBenchmark("RegExp", [&]() {
                assert(re.indexIn(inputString, 0) == 0);
                assert(re.cap(1) == targetString);
            });
        }

        {
            auto benchmark = proxySequence<QChar, QString>(
                discard(OrderedTokenRule<QChar, QString>("var")),
                name
            );

            runBenchmark("Sprout", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
            });
        }

        {
            auto benchmark = proxySequence<QChar, QString>(
                discard(OrderedTokenRule<QChar, QString>("var")),
                fastName
            );

            runBenchmark("Spfast", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
            });
        }
    }

    std::cout << std::endl;
    std::cout << "=== Whitespace Match Benchmarks ===\n";

    {
        const QString inputString(" foo");
        const QString targetString("foo");

        {
            QRegExp re("^\\s+(foo)");
            runBenchmark("RegExp", [&]() {
                assert(re.indexIn(inputString, 0) == 0);
                assert(re.cap(1) == targetString);
            });
        }

        {
            auto benchmark = proxySequence<QChar, QString>(
                whitespace,
                OrderedTokenRule<QChar, QString>("foo", "foo")
            );

            runBenchmark("Sprout", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
            });
        }

        {
            auto benchmark = proxySequence<QChar, QString>(
                fastWhitespace,
                OrderedTokenRule<QChar, QString>("foo", "foo")
            );

            runBenchmark("Spfast", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
            });
        }
    }

    std::cout << std::endl;
    std::cout << "=== Cursor Benchmarks ===\n";

    {
        QString target("var foo");
        QString str("var foo");
        runBenchmark("Sprout", [&]() {
            auto cursor = makeCursor<QChar>(&str);
            for (int j = 0; j < target.size(); ++j) {
                if (target.at(j) != *cursor++) {
                    assert(false);
                }
            }
        });
    }

    {
        QString target("var foo");
        QString str("var foo");
        runBenchmark("Inline", [&]() {
            QString result;
            auto cursor = makeCursor<QChar>(&str);
            result = "foo";
            for (int j = 0; j < target.size(); ++j) {
                if (target.at(j) != str.at(j)) {
                    result = "";
                }
            }
        });
    }

    return 0;
}

// vim: set ts=4 sw=4 :
