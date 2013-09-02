lib_LTLIBRARIES = libsprout.la
libsprout_la_CPPFLAGS = -Wall -Werror @QT_CXXFLAGS@ -I$(top_srcdir)/include
libsprout_la_LIBADD = @QT_LIBS@
libsprout_la_LDFLAGS = -version-info 0:0:0

libsprout_la_SOURCES =

bin_PROGRAMS = sprout
sprout_CPPFLAGS = $(libsprout_la_CPPFLAGS)
sprout_LDADD = libsprout.la $(libsprout_la_LIBADD)
sprout_SOURCES = \
	main.cpp

bin_PROGRAMS += sproutbenchmark
sproutbenchmark_CPPFLAGS = $(libsprout_la_CPPFLAGS)
sproutbenchmark_LDADD = libsprout.la $(libsprout_la_LIBADD)
sproutbenchmark_SOURCES = \
	benchmark.cpp
