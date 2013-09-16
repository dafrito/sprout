#include <iostream>
#include <cassert>

#include <QChar>
#include <QTextStream>
#include <QString>

#include "StreamIterator"
#include "TokenRule"
#include "OptionalRule"
#include "DiscardRule"
#include "MultipleRule"
#include "PredicateRule"
#include "ProxyRule"
#include "AlternativeRule"
#include "ReduceRule"
#include "config.h"

#include <QRegExp>
#include <QElapsedTimer>

#ifdef HAVE_BOOST
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#endif

#include "rules.hpp"

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

    auto fastWhitespace = rule::whitespace<QString>();

    auto whitespace = discard(
        multiple(predicate<QChar, QString>([](const QChar& input, QString&) {
            return input.isSpace();
        }))
    );

    auto name = aggregate<QString>(
        multiple(simplePredicate<QChar>([](const QChar& input) {
            return input.isLetter() || input == '_';
        })),
        [](QString& aggregate, const QChar& c) {
            aggregate += c;
        }
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

    {
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
    }

    {
        std::cout << "=== Direct Match Benchmarks ===\n";

        const QString inputString("var");
        const QString targetString("var");

        {
            QRegExp re("^var");
            runBenchmark("RegExp", [&]() {
                assert(re.indexIn(inputString, 0) == 0);
                assert(re.cap(0) == targetString);
            });
        }

        {
            auto benchmark = OrderedTokenRule<QChar, QString>("var", "var");
            runBenchmark("Sprout", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
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
                assert(*results == targetString);
            });
        }

        #ifdef HAVE_BOOST
        {
            std::string input("varbar");
            std::string target("bar");
            runBenchmark("Booost", [&]() {
                namespace qi = boost::spirit::qi;
                namespace ascii = boost::spirit::ascii;
                namespace phoenix = boost::phoenix;

                using qi::_1;

                std::string word;
                auto result = qi::phrase_parse(
                    input.begin(),
                    input.end(),
                    (
                        "var" >> qi::string("bar")[phoenix::ref(word) = "bar"]
                    ),
                    ascii::space
                );

                assert(result);
                assert(word == target);
            });
        }
        #endif

        std::cout << std::endl;
    }


    {
        std::cout << "=== Aggregating Match Benchmarks ===\n";

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

        #ifdef HAVE_BOOST
        {
            std::string inputString = "varbar";
            std::string targetString = "bar";
            runBenchmark("Booost", [&]() {
                namespace qi = boost::spirit::qi;
                namespace ascii = boost::spirit::ascii;
                namespace phoenix = boost::phoenix;

                using qi::double_;
                using qi::_1;

                std::vector<char> letters;
                auto result = qi::phrase_parse(
                    inputString.begin(),
                    inputString.end(),
                    (
                        "var" >> (+ascii::char_)[phoenix::ref(letters) = _1]
                    ),
                    ascii::space
                );
                std::string word(letters.begin(), letters.end());

                assert(result);
                assert(word == targetString);
            });
        }
        #endif

        std::cout << std::endl;
    }

    {
        std::cout << "=== Whitespace Match Benchmarks ===\n";

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

        #ifdef HAVE_BOOST
        {
            std::string input(" foo");
            std::string target("foo");
            runBenchmark("Booost", [&]() {
                namespace qi = boost::spirit::qi;
                namespace ascii = boost::spirit::ascii;
                namespace phoenix = boost::phoenix;

                using qi::_1;

                std::string word;
                auto result = qi::phrase_parse(
                    input.begin(),
                    input.end(),
                    (
                        *ascii::space >> qi::string("foo")[phoenix::ref(word) = "foo"]
                    ),
                    ascii::space
                );

                assert(result);
                assert(word == target);
            });
        }
        #endif

        std::cout << std::endl;
    }

    {
        std::cout << "=== Lazy Match Benchmarks ===\n";

        const QString inputString("foo #notime");
        const QString targetString("foo");

        {
            QRegExp re("^(foo)\\s*(#.*)?$");
            runBenchmark("RegExp", [&]() {
                assert(re.indexIn(inputString, 0) == 0);
                assert(re.cap(1) == targetString);
            });
        }

        {
            auto benchmark = proxySequence<QChar, QString>(
                OrderedTokenRule<QChar, QString>("foo", "foo"),
                discard(rule::whitespace<QString>()),
                discard(proxySequence<QChar, QString>(
                    OrderedTokenRule<QChar, QString>("#"),
                    proxyLazy<QChar, QString>(
                        any<QChar, QString>(),
                        proxyAlternative<QChar, QString>(
                            OrderedTokenRule<QChar, QString>("\n"),
                            make::end<QChar, QString>()
                        )
                    )
                ))
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
                OrderedTokenRule<QChar, QString>("foo", "foo"),
                discard(optional(fastWhitespace)),
                [](Cursor<QChar>& iter, Result<QString>& result) {
                    if (!iter || *iter != '#') {
                        return false;
                    }
                    while (iter && *iter != '\n') {
                        // Skip commented characters
                        ++iter;
                    }
                    return true;
                }
            );

            runBenchmark("Spfast", [&]() {
                auto cursor = makeCursor<QChar>(&inputString);
                Result<QString> results;

                assert(benchmark(cursor, results));
                assert(*results == targetString);
            });
        }

        #ifdef HAVE_BOOST
        {
            auto input = inputString.toStdString();
            auto target = targetString.toStdString();
            runBenchmark("Booost", [&]() {
                namespace qi = boost::spirit::qi;
                namespace ascii = boost::spirit::ascii;
                namespace phoenix = boost::phoenix;

                using qi::double_;
                using qi::_1;

                std::string word;
                auto result = qi::phrase_parse(
                    input.begin(),
                    input.end(),
                    (
                        qi::string("foo")[phoenix::ref(word) = "foo"] >> *ascii::space >> -("#" >> *ascii::char_)
                    ),
                    ascii::space
                );

                assert(result);
                assert(word == target);
            });
        }
        #endif

        std::cout << std::endl;
    }

    {
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
    }

    return 0;
}

// vim: set ts=4 sw=4 :
