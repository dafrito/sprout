#include "init.hpp"

#include "DiscardRule"
#include "TokenRule"
#include "MultipleRule"
#include "AlternativeRule"
#include "SequenceRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testDiscard)
{
    auto rule = make::discard(
        AnyTokenRule<char, std::string>("_-")
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testDiscardWithMultiple)
{
    auto rule = make::discard(
        make::multiple(
            AnyTokenRule<char, std::string>("_-")
        )
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("____----____Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_CHECK(!tokens);
    BOOST_CHECK_EQUAL('D', *cursor);
}
