#include "init.hpp"

#include "DiscardRule.hpp"
#include "TokenRule.hpp"
#include "MultipleRule.hpp"
#include "AlternativeRule.hpp"
#include "SequenceRule.hpp"
#include "OptionalRule.hpp"
#include "ProxyRule.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testTrivialProxy)
{
    ProxyRule<char, std::string> rule(
        OrderedTokenRule<char, std::string>("Cat", "Animal")
    );
}

BOOST_AUTO_TEST_CASE(testProxyHandlesAClosure)
{
    ProxyRule<char, std::string> rule([](Cursor<char>& input, Result<std::string>& result) {
        ++input;
        return true;
    });
}

BOOST_AUTO_TEST_CASE(testSplittingByWhitespace)
{
    using namespace make;

    auto whitespace = optional(discard(multiple(
        AnyTokenRule<char, std::string>("_")
    )));

    auto rule = multiple(
        proxySequence<char, std::string>(
            whitespace,
            alternative(
                OrderedTokenRule<char, std::string>("Cat", "Heathen"),
                OrderedTokenRule<char, std::string>("Dog", "Civilized"),
                OrderedTokenRule<char, std::string>("Calf", "Cow")
            ),
            whitespace,
            OrderedTokenRule<char, std::string>(",", "Comma"),
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
