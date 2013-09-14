#include "init.hpp"

#include "ReduceRule"
#include "MultipleRule"
#include "AlternativeRule"
#include "DiscardRule"
#include "TokenRule"

using namespace sprout;

BOOST_AUTO_TEST_CASE(testReduce)
{
    auto rule = make::reduce<std::string>(
        make::multiple(
            make::alternative(
                OrderedTokenRule<char, char>("-", '-'),
                OrderedTokenRule<char, char>("_", '_')
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
    auto rule = make::reduce<std::string, char, char>(
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
    auto rule = make::reduce<std::string>(
        make::rule<char, char>([](Cursor<char>& iter, Result<char>& tokens) {
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
