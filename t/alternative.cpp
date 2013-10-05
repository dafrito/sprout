#include <rule/Alternative.hpp>
#include <rule/Multiple.hpp>
#include <rule/Literal.hpp>

#include "init.hpp"

using namespace sprout;

rule::Alternative<rule::OrderedLiteral<char, std::string>> createAltRule()
{
    rule::OrderedLiteral<char, std::string> a;
    a.setTarget("Cat");
    a.setToken("Heathen");

    rule::OrderedLiteral<char, std::string> b;
    b.setTarget("Dog");
    b.setToken("Civilized");

    rule::OrderedLiteral<char, std::string> c;
    c.setTarget("Calf");
    c.setToken("Cow");

    std::vector<rule::OrderedLiteral<char, std::string>> rules { a, b, c };
    return rule::Alternative<decltype(a)>(rules);
}

BOOST_AUTO_TEST_CASE(matchASingleAlternative)
{
    auto alternative = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogCalfCat");
    BOOST_CHECK(alternative(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(matchAnotherSingleAlternative)
{
    auto rule = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("DogCalfCat");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(matchMultipleAlternatives)
{
    auto rule = rule::multiple(createAltRule());
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogCalfCat");
    BOOST_CHECK(rule(cursor, tokens));

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

bool oneAlternative(Cursor<char>& iter, Result<std::string>& result) {
    return true;
};

bool twoAlternative(Cursor<char>& iter, Result<std::string>& result) {
    return true;
};

BOOST_AUTO_TEST_CASE(testAlternativeWithLambda)
{
    auto rule = rule::alternative<char, std::string, std::function<decltype(oneAlternative)>>(
        oneAlternative,
        twoAlternative
    );
}
