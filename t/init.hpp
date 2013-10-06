#ifndef SPROUT_TEST_INIT_HEADER
#define SPROUT_TEST_INIT_HEADER

#include <boost/test/unit_test.hpp>

#include <QString>
#include <iostream>

std::ostream& operator<<(std::ostream& stream, const QString& str);

#endif // SPROUT_TEST_INIT_HEADER

// vim: set ts=4 sw=4 :
