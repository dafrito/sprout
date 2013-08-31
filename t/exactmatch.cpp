#include "init.hpp"

#include <iterator>
#include <sstream>

#include "ExactMatchRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkDirectMatch)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(str);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
}

BOOST_AUTO_TEST_CASE(checkBadMatch)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Dog");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK(!rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkPreemptedMatch)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Ca");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK(!rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkMatchWithTrailing)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Catca");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK_EQUAL("Animal", *rule.parse(cursor));
}

BOOST_AUTO_TEST_CASE(checkIteratorAtNextElementWhenMatchIsGood)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Catde");
    auto cursor = makeCursor<char>(str);

    BOOST_CHECK(true);
    for (auto i = cursor; i; ++i) {
        std::cout << *i << std::endl;
    }
    BOOST_CHECK(true);

    auto tokens = rule.parse(cursor);
    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("Animal", *tokens);
    BOOST_CHECK_EQUAL('d', *cursor);
}

BOOST_AUTO_TEST_CASE(checkIteratorAtFirstElementIfMatchFails)
{
    ExactMatchRule<char, std::string> rule;
    rule.setTarget("Cat");
    rule.setToken("Animal");

    std::stringstream str("Calf");
    auto cursor = makeCursor<char>(str);
    rule.parse(cursor);

    BOOST_CHECK_EQUAL('C', *cursor);

    ExactMatchRule<char, std::string> calfRule;
    calfRule.setTarget("Calf");
    calfRule.setToken("Part");

    BOOST_CHECK_EQUAL("Part", *calfRule.parse(cursor));
    BOOST_CHECK(!cursor);
}

// vim: set ts=4 sw=4 :
