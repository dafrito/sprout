lib_LTLIBRARIES = libsprout.la
libsprout_la_CPPFLAGS = -Wall @QT_CXXFLAGS@ -I$(top_srcdir)/include
libsprout_la_LIBADD = @QT_LIBS@
libsprout_la_LDFLAGS = -version-info 0:0:0

libsprout_la_SOURCES = \
	rules.cpp \
	grammar/Grammar.cpp \
	grammar/pass/LeftRecursion.cpp \
	grammar/pass/Flatten.cpp

# Fundamental types
nobase_pkginclude_HEADERS = \
	StreamIterator.hpp \
	Result.hpp \
	Cursor.hpp

# Rule headers
nobase_pkginclude_HEADERS += \
	rule/composite.hpp \
	rule/RuleTraits.hpp \
	rule/Alternative.hpp \
	rule/Proxy.hpp \
	rule/Predicate.hpp \
	rule/Catching.hpp \
	rule/Reduce.hpp \
	rule/Log.hpp \
	rule/Optional.hpp \
	rule/Join.hpp \
	rule/Shared.hpp \
	rule/Lazy.hpp \
	rule/Multiple.hpp \
	rule/Operation.hpp

# Grammar headers
nobase_pkginclude_HEADERS += \
	grammar/Grammar.hpp \
	grammar/pass/LeftRecursion.hpp \
	grammar/pass/Remove.hpp \
	grammar/pass/Flatten.hpp

bin_PROGRAMS = sprout
sprout_CPPFLAGS = $(libsprout_la_CPPFLAGS)
sprout_LDADD = libsprout.la
sprout_SOURCES = \
	main.cpp

bin_PROGRAMS += sproutllvm
sproutllvm_CPPFLAGS = $(libsprout_la_CPPFLAGS) `llvm-config --cppflags`
sproutllvm_LDADD = libsprout.la
sproutllvm_LDFLAGS = `llvm-config --ldflags --libs core jit native`

nobase_pkginclude_HEADERS += \
	llvm.hpp

sproutllvm_SOURCES = \
	llvm.cpp

noinst_PROGRAMS = benchmark
benchmark_CPPFLAGS = $(libsprout_la_CPPFLAGS)
benchmark_LDADD = libsprout.la
benchmark_SOURCES = \
	benchmark.cpp
