#include "init.hpp"

#include "PredicateRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testPredicate)
{
    auto rule = makeAddingPredicate<QChar, QString>([](const QChar& input) {
        return input.isLetter();
    });

    QString str(QString::fromUtf8("中国"));
    QTextStream stream(&str);
    auto cursor = makeCursor<QChar>(stream);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK(*tokens == str);
}

