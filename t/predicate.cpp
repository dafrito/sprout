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

    QString str(QString::fromUtf8("中国"));
    QTextStream stream(&str);
    auto cursor = makeCursor<QChar>(stream);

    auto tokens = rule.parse(cursor);
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

    QString str(QString::fromUtf8("中国"));
    QTextStream stream(&str);
    auto cursor = makeCursor<QChar>(stream);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK(str == *tokens++);
    BOOST_CHECK(!tokens);
}
