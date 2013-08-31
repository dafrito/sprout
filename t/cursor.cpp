#include "init.hpp"
#include "Cursor"
#include "StreamIterator"
#include <QTextStream>

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkCursor)
{
    std::stringstream str("Cat");
    Cursor<std::istream_iterator<char>> cur(str);

    BOOST_CHECK(cur);
    BOOST_CHECK_EQUAL('C', *cur++);

    BOOST_CHECK(cur);
    BOOST_CHECK_EQUAL('a', *cur++);

    BOOST_CHECK(cur);
    BOOST_CHECK_EQUAL('t', *cur++);

    BOOST_CHECK(!cur);
}

BOOST_AUTO_TEST_CASE(checkCursorWorksWithoutIncrementing)
{
    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(str);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(checkCursorWithStreamIterator)
{
    std::stringstream str("Cat");
    Cursor<std::istream_iterator<char>> cur(str);
}

BOOST_AUTO_TEST_CASE(constructCursorWithMake)
{
    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(str);
}
