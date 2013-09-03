#include "init.hpp"

#include "AlternativeRule"
#include "MultipleRule"
#include "TokenRule"

using namespace sprout;

AlternativeRule<OrderedTokenRule<char, std::string>> createAltRule()
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
    return AlternativeRule<decltype(a)>(rules);
}

BOOST_AUTO_TEST_CASE(matchASingleAlternative)
{
    auto alternative = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogCalfCat");
    BOOST_CHECK(alternative.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(matchAnotherSingleAlternative)
{
    auto alternative = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("DogCalfCat");
    BOOST_CHECK(alternative.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(matchMultipleAlternatives)
{
    auto rule = makeMultiple(createAltRule());
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogCalfCat");
    BOOST_CHECK(rule.parse(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Cow", *tokens++);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_REQUIRE(!tokens);
}
