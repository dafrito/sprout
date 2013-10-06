#include <rule/Sequence.hpp>
#include <rule/Literal.hpp>

#include "init.hpp"

using namespace sprout;

rule::Sequence<rule::OrderedLiteral<char, std::string>> createRule()
{
    return rule::sequence(
        rule::OrderedLiteral<char, std::string>("Cat", "Heathen"),
        rule::OrderedLiteral<char, std::string>("Dog", "Civilized"),
        rule::OrderedLiteral<char, std::string>("Zebra", "Cow")
    );
}

BOOST_AUTO_TEST_CASE(matchASingleSequence)
{
    auto rule = createRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogZebra");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK_EQUAL("Cow", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(sequencesFailCompletely)
{
    auto rule = createRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("DogCat");
    BOOST_CHECK(!rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(checkTupleSequence)
{
    auto rule = rule::tupleSequence<char, std::string>(
        rule::OrderedLiteral<char, std::string>("Cat", "Heathen"),
        rule::OrderedLiteral<char, std::string>("Dog", "Civilized"),
        rule::OrderedLiteral<char, std::string>("Zebra", "Cow")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("DogCat");
    BOOST_CHECK(!rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(partialSequencesAlsoFailCompletely)
{
    auto rule = createRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogWeb");
    BOOST_CHECK(!rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(sequenceWorksWithLambdas)
{
    // This test is slightly degenerate, since we can't actually store
    // more than one rule in this sequence, but it works to ensure
    // lambdas are supported.
    auto rule = rule::sequence<char, std::string>(
        [](Cursor<char>& iter, Result<std::string>& results) {
            if (iter && *iter == 'A') {
                results << "First";
                ++iter;
                return true;
            }
            return false;
        }
    );
}
