#include "init.hpp"

#include "PredicateRule"
#include "MultipleRule"
#include "ReduceRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testPredicate)
{
    auto rule = makeSimplePredicate<QChar, QString>([](const QChar& input) {
        return input.isLetter();
    });
    Result<QString> tokens;

    auto data = QString::fromUtf8("中国");
    auto cursor = makeCursor<QChar>(&data);
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK(QString::fromUtf8("中") == *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(testPredicateWithMultiple)
{
    auto rule = makeSimpleReduce<QString>(
        [](QString& str, const QChar& c) {
            str += c;
        },
        makeMultiple(makeSimplePredicate<QChar, QChar>([](const QChar& input) {
            return input.isLetter();
        }))
    );
    Result<QString> tokens;

    auto data = QString::fromUtf8("中国");
    auto cursor = makeCursor<QChar>(&data);
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK(QString::fromUtf8("中国") == *tokens++);
    BOOST_CHECK(!tokens);
}
