#include "init.hpp"

#include <iterator>
#include <sstream>

#include "TokenRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkDirectMatch)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Cat");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkBadMatch)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Dog");
    BOOST_CHECK(!rule.parse(cursor, tokens));
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(checkPreemptedMatch)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Ca");
    BOOST_CHECK(!rule.parse(cursor, tokens));
    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(checkMatchWithTrailing)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Catca");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtNextElementWhenMatchIsGood)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Catde");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
    BOOST_CHECK_EQUAL('d', *cursor);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtFirstElementIfMatchFails)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Calf");
    BOOST_CHECK(!rule.parse(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);

    OrderedTokenRule<char, std::string> calfRule("Calf", "Cow");
    BOOST_CHECK(calfRule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Cow", *tokens);
}

BOOST_AUTO_TEST_CASE(rulesCanBeNested)
{
    OrderedTokenRule<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Cat");
    BOOST_CHECK(rule.parse(cursor, tokens));
    BOOST_CHECK(tokens);

    auto tokenCursor = makeCursor<std::string>(tokens);
    BOOST_REQUIRE(tokenCursor);
    BOOST_CHECK_EQUAL(std::string("Animal"), *tokenCursor++);
    BOOST_CHECK(!tokenCursor);
}

BOOST_AUTO_TEST_CASE(anyTokenRule)
{
    AnyTokenRule<char, std::string> rule(" _", "Whitespace");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_Dog");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Whitespace", *tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

// vim: set ts=4 sw=4 :
