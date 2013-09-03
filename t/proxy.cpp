#include "init.hpp"

#include "DiscardRule"
#include "TokenRule"
#include "MultipleRule"
#include "AlternativeRule"
#include "SequenceRule"
#include "ProxyRule"

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
    auto rule = makeMultiple(
        makeProxySequence<char, std::string>(
            makeDiscard(makeMultiple(
                AnyTokenRule<char, std::string>("_")
            )),
            makeAlternate(
                OrderedTokenRule<char, std::string>("Cat", "Heathen"),
                OrderedTokenRule<char, std::string>("Dog", "Civilized"),
                OrderedTokenRule<char, std::string>("Calf", "Cow")
            ),
            makeDiscard(makeMultiple(
                AnyTokenRule<char, std::string>("_")
            )),
            OrderedTokenRule<char, std::string>(",", "Comma"),
            makeDiscard(makeMultiple(
                AnyTokenRule<char, std::string>("_")
            ))
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
