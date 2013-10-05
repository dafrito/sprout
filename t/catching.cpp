#include "init.hpp"

#include "CatchingRule.hpp"
#include "PredicateRule.hpp"

using namespace sprout;

struct FakeException {};

BOOST_AUTO_TEST_CASE(testCatching)
{
    using namespace make;

    auto rule = catching<FakeException>(
        simplePredicate<char, std::string>([](const char& input) {
            if (input == 'a') {
                throw FakeException();
            }
            return true;
        })
    );

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("abc");
        BOOST_CHECK(!rule(cursor, tokens));

        BOOST_CHECK(!tokens);
    }

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("bbb");
        BOOST_CHECK(rule(cursor, tokens));

        BOOST_REQUIRE(tokens);
        BOOST_CHECK(*tokens == "b");
    }
}

BOOST_AUTO_TEST_CASE(testCatchingWithLambda)
{
    using namespace make;

    auto rule = catching<FakeException, char, std::string>(
        [](Cursor<char>& iter, Result<std::string>& result) -> bool {
            throw FakeException();
        }
    );

    {
        Result<std::string> tokens;

        auto cursor = makeCursor<char>("abc");
        BOOST_CHECK(!rule(cursor, tokens));

        BOOST_CHECK(!tokens);
    }
}
