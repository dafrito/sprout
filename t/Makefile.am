check_PROGRAMS = runtest
TESTS = $(check_PROGRAMS)

runtest_CXXFLAGS = -Wall -Werror @QT_CXXFLAGS@ $(AM_CXXFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src -DBOOST_TEST_DYN_LINK

# Don't try changing "../src" to use "top_srcdir" - this is referring to the
# built artifact, not something in the source directory
runtest_LDADD = ../src/libsprout.la @QT_LIBS@ @BOOST_UNIT_TEST_FRAMEWORK_LIB@

noinst_HEADERS = \
	init.hpp \
	node.hpp

runtest_SOURCES = \
	init.cpp \
	node.cpp \
	cursor.cpp \
	iterator.cpp \
	literal.cpp \
	multiple.cpp \
	alternative.cpp \
	sequence.cpp \
	discard.cpp \
	optional.cpp \
	operation.cpp \
	proxy.cpp \
	predicate.cpp \
	catching.cpp \
	reduce.cpp \
	recursive.cpp \
	grammar/pass_flatten.cpp \
	main.cpp
