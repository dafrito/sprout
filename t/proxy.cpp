#include <rule/Discard.hpp>
#include <rule/Literal.hpp>
#include <rule/Multiple.hpp>
#include <rule/Alternative.hpp>
#include <rule/Sequence.hpp>
#include <rule/Optional.hpp>
#include <rule/Proxy.hpp>

#include "init.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testTrivialProxy)
{
    rule::Proxy<char, std::string> rule(
        rule::OrderedLiteral<char, std::string>("Cat", "Animal")
    );
}

BOOST_AUTO_TEST_CASE(testProxyHandlesAClosure)
{
    rule::Proxy<char, std::string> rule([](Cursor<char>& input, Result<std::string>& result) {
        ++input;
        return true;
    });
}

BOOST_AUTO_TEST_CASE(testSplittingByWhitespace)
{
    auto whitespace = optional(discard(multiple(
        rule::AnyLiteral<char, std::string>("_")
    )));

    auto rule = multiple(
        rule::proxySequence<char, std::string>(
            whitespace,
            alternative(
                rule::OrderedLiteral<char, std::string>("Cat", "Heathen"),
                rule::OrderedLiteral<char, std::string>("Dog", "Civilized"),
                rule::OrderedLiteral<char, std::string>("Calf", "Cow")
            ),
            whitespace,
            rule::OrderedLiteral<char, std::string>(",", "Comma"),
            whitespace
        )
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("Dog,_Cat,_Calf,_Cat,");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK_EQUAL("Comma", *tokens++);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK_EQUAL("Comma", *tokens++);
    BOOST_CHECK_EQUAL("Cow", *tokens++);
    BOOST_CHECK_EQUAL("Comma", *tokens++);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK_EQUAL("Comma", *tokens++);
    BOOST_CHECK(!tokens);
}
