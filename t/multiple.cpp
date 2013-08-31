#include "init.hpp"

#include "MultipleRule"
#include "TokenRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(matchASingleMultiple)
{
    OrderedTokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    MultipleRule<decltype(rule)> multiple(rule);

    std::stringstream str("CatDog");
    auto cursor = makeCursor<char>(str);

    auto tokens = multiple.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
    ++tokens;
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultiple)
{
    OrderedTokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    MultipleRule<decltype(rule)> multiple(rule);

    std::stringstream str("CatCatCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = multiple.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultipleWithANonMatch)
{
    OrderedTokenRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    MultipleRule<decltype(rule)> multiple(rule);

    std::stringstream str("Dog");
    auto cursor = makeCursor<char>(str);

    auto tokens = multiple.parse(cursor);
    BOOST_REQUIRE(!tokens);
}
