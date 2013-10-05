#include <rule/Predicate.hpp>
#include <rule/Multiple.hpp>
#include <rule/Reduce.hpp>

#include "init.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testPredicate)
{
    auto rule = rule::simplePredicate<QChar, QString>([](const QChar& input) {
        return input.isLetter();
    });
    Result<QString> tokens;

    auto data = QString::fromUtf8("中国");
    auto cursor = makeCursor<QChar>(&data);
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK(QString::fromUtf8("中") == *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(testPredicateWithMultiple)
{
    using namespace rule;

    auto rule = aggregate<QString>(
        multiple(simplePredicate<QChar, QChar>([](const QChar& input) {
            return input.isLetter();
        })),
        [](QString& str, const QChar& c) {
            str += c;
        }
    );
    Result<QString> tokens;

    auto data = QString::fromUtf8("中国");
    auto cursor = makeCursor<QChar>(&data);
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK(QString::fromUtf8("中国") == *tokens++);
    BOOST_CHECK(!tokens);
}
