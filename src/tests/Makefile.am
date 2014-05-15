check_PROGRAMS = runtest
TESTS = $(check_PROGRAMS)

runtest_CXXFLAGS = -Wall @QT_CXXFLAGS@ $(AM_CXXFLAGS) -I$(top_srcdir)/src -DBOOST_TEST_DYN_LINK
runtest_LDADD = ../libsprout.la @QT_LIBS@ @BOOST_UNIT_TEST_FRAMEWORK_LIB@

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
	grammar/pass_remove.cpp \
	main.cpp
