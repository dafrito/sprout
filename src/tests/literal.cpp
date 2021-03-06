#include <rule/Literal.hpp>
#include <rule/Sequence.hpp>
#include <rule/Proxy.hpp>
#include <rule/Multiple.hpp>
#include <rule/rules.hpp>

#include "init.hpp"

#include <iterator>
#include <sstream>

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkDirectMatch)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Cat");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkBadMatch)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Dog");
    BOOST_CHECK(!rule(cursor, tokens));
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(checkPreemptedMatch)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Ca");
    BOOST_CHECK(!rule(cursor, tokens));
    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(checkMatchWithTrailing)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Catca");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtNextElementWhenMatchIsGood)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Catde");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
    BOOST_CHECK_EQUAL('d', *cursor);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtFirstElementIfMatchFails)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Calf");
    BOOST_CHECK(!rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);

    rule::OrderedLiteral<char, std::string> calfRule("Calf", "Cow");
    BOOST_CHECK(calfRule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Cow", *tokens);
}

BOOST_AUTO_TEST_CASE(rulesCanBeNested)
{
    rule::OrderedLiteral<char, std::string> rule("Cat", "Animal");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Cat");
    BOOST_CHECK(rule(cursor, tokens));
    BOOST_CHECK(tokens);

    auto tokenCursor = makeCursor<std::string>(tokens);
    BOOST_REQUIRE(tokenCursor);
    BOOST_CHECK_EQUAL(std::string("Animal"), *tokenCursor++);
    BOOST_CHECK(!tokenCursor);
}

BOOST_AUTO_TEST_CASE(anyLiteralRule)
{
    rule::AnyLiteral<char, std::string> rule(" _", "Whitespace");
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Whitespace", *tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(endTokenRule)
{
    auto rule = rule::proxySequence<char, std::string>(
        rule::multiple(
            rule::AnyLiteral<char, std::string>(" _", "Whitespace")
        ),
        rule::end<char, std::string>()
    );

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("_");
        BOOST_CHECK(rule(cursor, tokens));
        BOOST_REQUIRE(tokens);
        BOOST_CHECK_EQUAL(*tokens, "Whitespace");
    }

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("_D");
        BOOST_CHECK(!rule(cursor, tokens));
        BOOST_CHECK(!tokens);
    }
}

// vim: set ts=4 sw=4 :
