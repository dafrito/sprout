lib_LTLIBRARIES = libsprout.la
libsprout_la_CPPFLAGS = -Wall -Werror @QT_CXXFLAGS@ -I$(top_srcdir)/include
libsprout_la_LIBADD = @QT_LIBS@
libsprout_la_LDFLAGS = -version-info 0:0:0

libsprout_la_SOURCES =

lib_LTLIBRARIES += libsproutcommon.la
libsproutcommon_la_CPPFLAGS = -Wall -Werror @QT_CXXFLAGS@ -I$(top_srcdir)/include
libsproutcommon_la_LIBADD = @QT_LIBS@
libsproutcommon_la_LDFLAGS = -version-info 0:0:0

libsproutcommon_la_SOURCES = \
	rules.hpp \
	rules.cpp

bin_PROGRAMS = sprout
sprout_CPPFLAGS = $(libsproutcommon_la_CPPFLAGS)
sprout_LDADD = libsproutcommon.la
sprout_SOURCES = \
	main.cpp

noinst_PROGRAMS = benchmark
benchmark_CPPFLAGS = $(libsproutcommon_la_CPPFLAGS)
benchmark_LDADD = libsproutcommon.la
benchmark_SOURCES = \
	benchmark.cpp
