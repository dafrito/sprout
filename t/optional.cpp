#include <rule/Discard.hpp>
#include <rule/Optional.hpp>
#include <rule/Literal.hpp>
#include <rule/Multiple.hpp>
#include <rule/Alternative.hpp>
#include <rule/Sequence.hpp>

#include "init.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testOptional)
{
    auto rule = rule::optional(
        rule::AnyLiteral<char, std::string>("_-")
    );

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("_Dog");
        BOOST_CHECK(rule(cursor, tokens));
        BOOST_CHECK_EQUAL('D', *cursor);
    }

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("ABC");
        BOOST_CHECK(rule(cursor, tokens));
        BOOST_CHECK_EQUAL('A', *cursor);
    }
}

BOOST_AUTO_TEST_CASE(testOptionalWithLambda)
{
    auto rule = rule::optional<char, std::string>(
        [](Cursor<char>& input, Result<std::string>& result) {
            std::string str;
            while (input && *input == '_') {
                str += *input;
                ++input;
            }
            if (!str.empty()) {
                result << str;
            }
            return str;
        }
    );

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("___Dog");
        BOOST_CHECK(rule(cursor, tokens));
        BOOST_REQUIRE(tokens);
        BOOST_CHECK_EQUAL("___", *tokens++);
        BOOST_CHECK(!tokens);
        BOOST_CHECK_EQUAL('D', *cursor);
    }

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("Dog");
        BOOST_CHECK(rule(cursor, tokens));
        BOOST_CHECK(!tokens);
        BOOST_CHECK_EQUAL('D', *cursor);
    }
}
