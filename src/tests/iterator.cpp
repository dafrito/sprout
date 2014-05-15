#include <Cursor.hpp>
#include <StreamIterator.hpp>

#include "init.hpp"

#include <QTextStream>

using namespace sprout;

BOOST_AUTO_TEST_CASE(testStreamIterator)
{
    QString str("Dog");
    QTextStream stream(&str);
    StreamIterator<QChar, QTextStream> iter(&stream);
    StreamIterator<QChar, QTextStream> eof;

    BOOST_CHECK(QChar('D') == *iter);
    BOOST_CHECK(eof != iter);
    ++iter;

    BOOST_CHECK(QChar('o') == *iter);
    BOOST_CHECK(eof != iter);
    ++iter;

    BOOST_CHECK(QChar('g') == *iter);
    BOOST_CHECK(eof != iter);
    BOOST_CHECK(iter);
    ++iter;

    BOOST_CHECK(eof == iter);
    BOOST_CHECK(!iter);
}

BOOST_AUTO_TEST_CASE(iteratorWorksEvenWithStringStream)
{
    std::stringstream stream("Cat");
    StreamIterator<char, std::stringstream> iter(&stream);
    StreamIterator<char, std::stringstream> eof;

    BOOST_CHECK('C' == *iter);
    BOOST_CHECK(eof != iter);
    ++iter; // at 'a'

    BOOST_CHECK('a' == *iter);
    BOOST_CHECK(eof != iter);
    ++iter; // at 't'

    BOOST_CHECK('t' == *iter);
    BOOST_CHECK(eof != iter);
    BOOST_CHECK(iter);
    ++iter; // at eof

    BOOST_CHECK(eof == iter);
    BOOST_CHECK(!iter);
}

BOOST_AUTO_TEST_CASE(iteratorBehavesProperlyWithEmptyStream)
{
    QString emptyStr;
    QTextStream stream(&emptyStr);
    StreamIterator<QChar, QTextStream> iter(&stream);
    BOOST_CHECK(!iter);

    std::stringstream stdStream;
    StreamIterator<char, std::stringstream> stdIter(&stdStream);
    BOOST_CHECK(!stdIter);
}
