#include "init.hpp"

#include "MultipleRule.hpp"
#include "TokenRule.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(matchASingleMultiple)
{
    auto rule = make::multiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultiple)
{
    auto rule = make::multiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatCatCat");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_CHECK_EQUAL("Animal", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultipleWithANonMatch)
{
    auto rule = make::multiple(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Dog");
    BOOST_CHECK(!rule(cursor, tokens));
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultipleWithALambda)
{
    auto rule = make::multiple<char, std::string>(
        [](Cursor<char>& input, Result<std::string>& result) {
            if (input && *input == 'A') {
                ++input;
                return true;
            }
            return false;
        }
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("ABC");
    BOOST_CHECK(rule(cursor, tokens));
    BOOST_REQUIRE(!tokens);
}
