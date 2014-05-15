#include <rule/Reduce.hpp>
#include <rule/Multiple.hpp>
#include <rule/Alternative.hpp>
#include <rule/Discard.hpp>
#include <rule/Literal.hpp>

#include "init.hpp"

using namespace sprout;
using namespace rule;

BOOST_AUTO_TEST_CASE(testReduce)
{
    auto rule = rule::reduce<std::string>(
        rule::multiple(
            rule::alternative(
                rule::OrderedLiteral<char, char>("-", '-'),
                rule::OrderedLiteral<char, char>("_", '_')
            )
        ),
        [](Result<std::string>& result, Result<char>& subResult) {
            std::string str;
            for (auto token : subResult) {
                if (token == '_') {
                    str += token;
                }
            }
            result << str;
        }
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_-_-Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("__", *tokens++);
    BOOST_CHECK(!tokens);

    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testReduceWithLambda)
{
    auto rule = rule::reduce<std::string, char, char>(
        [](Cursor<char>& iter, Result<char>& tokens) {
            bool found = false;
            while (iter) {
                auto c = *iter;
                if (c != '_' && c != '-') {
                    break;
                }
                found = true;
                tokens << c;
                ++iter;
            }
            return found;
        },
        [](Result<std::string>& result, Result<char>& subResult) {
            std::string str;
            for (auto token : subResult) {
                if (token == '_') {
                    str += token;
                }
            }
            result << str;
        }
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_-_-Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("__", *tokens++);
    BOOST_CHECK(!tokens);

    BOOST_CHECK_EQUAL('D', *cursor);
}

BOOST_AUTO_TEST_CASE(testReduceWithWrappedRule)
{
    auto rule = rule::reduce<std::string>(
        rule::wrap<char, char>([](Cursor<char>& iter, Result<char>& tokens) {
            bool found = false;
            while (iter) {
                auto c = *iter;
                if (c != '_' && c != '-') {
                    break;
                }
                found = true;
                tokens << c;
                ++iter;
            }
            return found;
        }),
        [](Result<std::string>& result, Result<char>& subResult) {
            std::string str;
            for (auto token : subResult) {
                if (token == '_') {
                    str += token;
                }
            }
            result << str;
        }
    );
    Result<std::string> tokens;

    auto cursor = makeCursor<char>("_-_-Dog");
    BOOST_CHECK(rule(cursor, tokens));

    BOOST_REQUIRE(tokens);
    BOOST_CHECK_EQUAL("__", *tokens++);
    BOOST_CHECK(!tokens);

    BOOST_CHECK_EQUAL('D', *cursor);
}
