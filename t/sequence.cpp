#include "init.hpp"

#include "SequenceRule"
#include "TokenRule"

using namespace sprout;

SequenceRule<OrderedTokenRule<char, std::string>> createRule()
{
    OrderedTokenRule<char, std::string> a;
    a.setTarget("Cat");
    a.setToken("Heathen");

    OrderedTokenRule<char, std::string> b;
    b.setTarget("Dog");
    b.setToken("Civilized");

    OrderedTokenRule<char, std::string> c;
    c.setTarget("Calf");
    c.setToken("Cow");

    std::vector<OrderedTokenRule<char, std::string>> rules { a, b, c };
    return SequenceRule<decltype(a)>(rules);
}

BOOST_AUTO_TEST_CASE(matchASingleSequence)
{
    auto rule = createRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogCalf");
    BOOST_CHECK(rule.parse(cursor, tokens));

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
    BOOST_CHECK(!rule.parse(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(partialSequencesAlsoFailCompletely)
{
    auto rule = createRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogCat");
    BOOST_CHECK(!rule.parse(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}
