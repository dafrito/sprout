#include <rule/Discard.hpp>
#include <rule/Literal.hpp>
#include <rule/Multiple.hpp>
#include <rule/Alternative.hpp>
#include <rule/Sequence.hpp>

#include "init.hpp"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testDiscard)
{
    auto rule = rule::discard(
        rule::AnyLiteral<char, std::string>("_-")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testDiscardWithMultiple)
{
    auto rule = rule::discard(
        rule::multiple(
            rule::AnyLiteral<char, std::string>("_-")
        )
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("____----____Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testDiscardWithLambda)
{
    auto rule = rule::discard<char, std::string>(
        [](Cursor<char>& input, Result<std::string>& result) {
            std::string str;
            while (input && *input == '_') {
                str += *input;
                ++input;
            }
            auto found = !str.empty();
            if (found) {
                result << str;
            }
            return found;
        }
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}
