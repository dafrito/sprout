#include <rule/Alternative.hpp>
#include <rule/Multiple.hpp>
#include <rule/Literal.hpp>

#include "init.hpp"

using namespace sprout;

rule::Alternative<rule::OrderedLiteral<char, std::string>> createAltRule()
{
    return rule::alternative(
        rule::OrderedLiteral<char, std::string>("Cat", "Heathen"),
        rule::OrderedLiteral<char, std::string>("Dog", "Civilized"),
        rule::OrderedLiteral<char, std::string>("Zebra", "Cow")
    );
}

BOOST_AUTO_TEST_CASE(matchASingleAlternative)
{
    auto alternative = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogZebraCat");
    BOOST_CHECK(alternative(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Heathen", *tokens++);
    BOOST_CHECK(!tokens);
}

BOOST_AUTO_TEST_CASE(matchAnotherSingleAlternative)
{
    auto rule = createAltRule();
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("DogZebraCat");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Civilized", *tokens++);
    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('Z', *cursor);
}

BOOST_AUTO_TEST_CASE(matchMultipleAlternatives)
{
    auto rule = rule::multiple(createAltRule());
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("CatDogDogZebraCat");
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
