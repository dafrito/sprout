#include "init.hpp"

#include <iterator>
#include <sstream>

#include "TokenRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkDirectMatch)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkBadMatch)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Dog");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK(rule.end() == rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkPreemptedMatch)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Ca");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK(rule.end() == rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkMatchWithTrailing)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Catca");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK_EQUAL("Animal", *rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkIteratorAtNextElementWhenMatchIsGood)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Catde");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
    BOOST_CHECK_EQUAL('d', *cursor);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtFirstElementIfMatchFails)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Calf");
    auto cursor = makeCursor<char>(str);
    rule.parse(cursor);

    BOOST_CHECK_EQUAL('C', *cursor);

    TokenRule<char, std::string> calfRule;
    calfRule.setTarget("Calf");
    calfRule.setToken("Part");

    BOOST_CHECK_EQUAL("Part", *calfRule.parse(cursor));
    BOOST_CHECK(!cursor);
}

BOOST_AUTO_TEST_CASE(rulesCanBeNested)
{
    TokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(str);
    auto tokens = rule.parse(cursor);

    Cursor<decltype(tokens)> tokenCursor(tokens, rule.end());
    BOOST_CHECK_EQUAL(std::string("Animal"), *tokenCursor);
    tokenCursor++;
    BOOST_CHECK(!tokenCursor);
}

// vim: set ts=4 sw=4 :
