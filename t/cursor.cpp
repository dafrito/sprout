#include "init.hpp"
#include "Cursor.hpp"
#include "StreamIterator.hpp"
#include <QTextStream>

using namespace sprout;

BOOST_AUTO_TEST_CASE(checkCursor)
{
    std::stringstream str("Cat");
    Cursor<char> cur(
        (std::istream_iterator<char>(str)),
        (std::istream_iterator<char>())
    );

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
    auto cursor = makeCursor<char>(&str);
    BOOST_CHECK_EQUAL('C', *cursor);
}

BOOST_AUTO_TEST_CASE(checkCursorWithStreamIterator)
{
    std::stringstream str("Cat");
    Cursor<char> cur(
        (std::istream_iterator<char>(str)),
        (std::istream_iterator<char>())
    );
}

BOOST_AUTO_TEST_CASE(constructCursorWithMake)
{
    std::stringstream str("Cat");
    auto cursor = makeCursor<char>(&str);
}
