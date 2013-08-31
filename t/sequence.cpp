#include "init.hpp"

#include "SequenceRule"
#include "ExactMatchRule"

using namespace sprout;

SequenceRule<ExactMatchRule<char, std::string>> createRule()
{
    ExactMatchRule<char, std::string> a;
    a.setTarget("Cat");
    a.setToken("Heathen");

    ExactMatchRule<char, std::string> b;
    b.setTarget("Dog");
    b.setToken("Civilized");

    ExactMatchRule<char, std::string> c;
    c.setTarget("Calf");
    c.setToken("Cow");

    std::vector<ExactMatchRule<char, std::string>> rules { a, b, c };
    return SequenceRule<decltype(a)>(rules);
}

BOOST_AUTO_TEST_CASE(matchASingleSequence)
{
    auto rule = createRule();

    std::stringstream str("CatDogCalf");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK_EQUAL("Cow", *tokens++);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(sequencesFailCompletely)
{
    auto rule = createRule();

    std::stringstream str("DogCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(!tokens);
}

BOOST_AUTO_TEST_CASE(partialSequencesAlsoFailCompletely)
{
    auto rule = createRule();

    std::stringstream str("CatDogCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(!tokens);
}
