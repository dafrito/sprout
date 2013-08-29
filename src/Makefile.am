lib_LTLIBRARIES = libemit.la
libemit_la_CPPFLAGS = -I$(top_srcdir)/include
libemit_la_LIBADD =
libemit_la_LDFLAGS = -version-info 0:0:0

libemit_la_SOURCES = ExactMatchRule.cpp 

bin_PROGRAMS = emit
emit_CPPFLAGS = $(libemit_la_CPPFLAGS)
emit_LDADD = libemit.la $(libemit_la_LIBADD)

emit_SOURCES = \
	main.cpp
