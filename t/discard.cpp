#include "init.hpp"

#include "DiscardRule"
#include "TokenRule"
#include "MultipleRule"
#include "AlternativeRule"
#include "SequenceRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testDiscard)
{
    AnyTokenRule<char, std::string> tokenRule;
    tokenRule.setTarget("_-");

    auto rule = makeDiscard(tokenRule);

    std::stringstream str("_Dog");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_CHECK(tokens == rule.end());
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testDiscardWithMultiple)
{
    AnyTokenRule<char, std::string> tokenRule;
    tokenRule.setTarget("_-");

    auto rule = makeDiscard(makeMultiple(tokenRule));

    std::stringstream str("____----____Dog");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_CHECK(tokens == rule.end());
    BOOST_CHECK_EQUAL('D', *cursor);
}
