#include "init.hpp"

#include "CatchingRule"
#include "PredicateRule"

using namespace sprout;

struct FakeException {};

BOOST_AUTO_TEST_CASE(testCatching)
{
    auto rule = makeCatching<FakeException>(
        makeAddingPredicate<char, std::string>([](const char& input) {
            if (input == 'a') {
                throw FakeException();
            }
            return true;
        })
    );

    {
        std::stringstream str("abc");
        auto cursor = makeCursor<char>(str);

        auto tokens = rule.parse(cursor);
        BOOST_CHECK(!tokens);
    }

    {
        std::stringstream str("bbb");
        auto cursor = makeCursor<char>(str);

        auto tokens = rule.parse(cursor);
        BOOST_REQUIRE(tokens);
        BOOST_CHECK(*tokens == "bbb");
    }
}
