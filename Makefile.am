ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src t

pkgdata_DATA = \
	sprout.m4

export RPMDIR ?= $$HOME/rpmbuild

run:
	./src/sprout $(top_srcdir)/src/lua.grammar

checkdebug: $(TESTS)
	cd t && gdb .libs/lt-runtest

checkvalgrind:
	cd t && valgrind -v --leak-check=full .libs/lt-runtest

debug: $(TESTS)
	cd src && gdb ./.libs/lt-sprout

valgrind:
	valgrind -v --leak-check=full --trace-children=yes ./src/sprout $(top_srcdir)/src/lua.grammar

lua:
	./src/luaparse $(top_srcdir)/src/simple.lua

llvm:
	./src/sproutllvm

llvmvalgrind:
	valgrind -v --leak-check=full --trace-children=yes ./src/sproutllvm

luavalgrind:
	valgrind -v --leak-check=full --trace-children=yes ./src/luaparse $(top_srcdir)/src/simple.lua

doc: sprout.doxygen
	doxygen $<
.PHONY: doc

sprout.doxygen: sprout.doxygen.in Makefile.am
	rm -f $@.tmp;
	cat $< >> $@.tmp;
	echo "INPUT += $(top_srcdir)/include $(top_srcdir)/src" >> $@.tmp;
	echo "INPUT += $(top_srcdir)/sprout.doc" >>$@.tmp
	mv $@.tmp $@

# This is a common typo for me
cehck: check
.PHONY: cehck

RPMFLAGS ?= --ba
SRCRPM=sprout-@PACKAGE_VERSION@-@PACKAGE_RELEASE@.src.rpm

rpm: $(SRCRPM)
$(SRCRPM): rpm.spec $(DISTFILES)
	$(MAKE) dist-gzip $(RPMDIR)
	cp -u $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz $(RPMDIR)/SOURCES
	cp -u rpm.spec $(RPMDIR)/SPECS/$(PACKAGE_NAME).spec
	rpmbuild $(RPMFLAGS) $(RPMDIR)/SPECS/$(PACKAGE_NAME).spec
	for package in `rpm -q --specfile rpm.spec`; do \
		arch=`echo $$package | grep -E -o '[^.]+$$'`; \
		filename="$(RPMDIR)/RPMS/$$arch/$$package.rpm"; \
		[ -e `basename $$filename` ] || ln -s $$filename; \
	done
	rm -f $@
	ln -s $(RPMDIR)/SRPMS/sprout-@PACKAGE_VERSION@-@PACKAGE_RELEASE@`rpmbuild -E '%{?dist}' rpm.spec`.src.rpm $@
.PHONY: rpm

$(RPMDIR):
	mkdir -p $@
	cd $@ && mkdir -p SOURCES SPECS BUILD RPMS SRPMS
