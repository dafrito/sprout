lib_LTLIBRARIES = libsprout.la
libsprout_la_CPPFLAGS = -Wall -Werror @QT_CXXFLAGS@ -I$(top_srcdir)/include
libsprout_la_LIBADD = @QT_LIBS@
libsprout_la_LDFLAGS = -version-info 0:0:0

libsprout_la_SOURCES = \
	rules.cpp \
	grammar/Grammar.cpp \
	grammar/pass/LeftRecursion.cpp \
	grammar/pass/Flatten.cpp

bin_PROGRAMS = sprout
sprout_CPPFLAGS = $(libsprout_la_CPPFLAGS)
sprout_LDADD = libsprout.la
sprout_SOURCES = \
	main.cpp

bin_PROGRAMS += sproutllvm
sproutllvm_CPPFLAGS = $(libsprout_la_CPPFLAGS) `llvm-config --cppflags`
sproutllvm_LDADD = libsprout.la
sproutllvm_LDFLAGS = `llvm-config --ldflags --libs core jit native`

nobase_pkginclude_HEADERS = \
	llvm.hpp

sproutllvm_SOURCES = \
	llvm.cpp

noinst_PROGRAMS = benchmark
benchmark_CPPFLAGS = $(libsprout_la_CPPFLAGS)
benchmark_LDADD = libsprout.la
benchmark_SOURCES = \
	benchmark.cpp
