#include "init.hpp"

#include "AlternativeRule"
#include "MultipleRule"
#include "TokenRule"

using namespace sprout;

AlternativeRule<TokenRule<char, std::string>> createAltRule()
{
    TokenRule<char, std::string> a;
    a.setTarget("Cat");
    a.setToken("Heathen");

    TokenRule<char, std::string> b;
    b.setTarget("Dog");
    b.setToken("Civilized");

    TokenRule<char, std::string> c;
    c.setTarget("Calf");
    c.setToken("Cow");

    std::vector<TokenRule<char, std::string>> rules { a, b, c };
    return AlternativeRule<decltype(a)>(rules);
}

BOOST_AUTO_TEST_CASE(matchASingleAlternative)
{
    auto alternative = createAltRule();

    std::stringstream str("CatDogDogCalfCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = alternative.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(matchAnotherSingleAlternative)
{
    auto alternative = createAltRule();

    std::stringstream str("DogCalfCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = alternative.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(matchMultipleAlternatives)
{
    auto alternative = createAltRule();
    MultipleRule<decltype(alternative)> multiple(alternative);

    std::stringstream str("CatDogDogCalfCat");
    auto cursor = makeCursor<char>(str);

    auto tokens = multiple.parse(cursor);
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
