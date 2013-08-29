#include "init.hpp"

#include "ExactMatchRule"


BOOST_AUTO_TEST_CASE(checkSanity)
{
    ExactMatchRule rule("Yes");
    BOOST_CHECK(!rule.match("AAA BBB CCC"));
    BOOST_CHECK(rule.match("Yes"));
}

// vim: set ts=4 sw=4 :
