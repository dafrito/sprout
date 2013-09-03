#include "init.hpp"

#include "MultipleRule"
#include "TokenRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(matchASingleMultiple)
{
    auto rule = makeMultiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDog");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultiple)
{
    auto rule = makeMultiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatCatCat");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultipleWithANonMatch)
{
    auto rule = makeMultiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Dog");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(!tokens);
}
