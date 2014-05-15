# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sprout], [1.0], [dafrito@gmail.com], [sprout], [http://www.dafrito.com])
AC_SUBST([PACKAGE_RELEASE], [1])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.hpp])
AC_CONFIG_MACRO_DIR([m4])

# Check for build programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Initialize libtool
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT
AC_PROG_LIBTOOL

# Check for compiler characteristics.
AC_LANG([C++])
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AX_CXX_COMPILE_STDCXX_11([noext],[AM_CXXFLAGS])
AC_SUBST(AM_CXXFLAGS)

# Check for Boost
AX_BOOST_BASE
AX_BOOST_UNIT_TEST_FRAMEWORK

# Check for Qt
AX_HAVE_QT_CORE

AC_CONFIG_FILES([sprout.m4 Makefile src/Makefile src/tests/Makefile rpm.spec])
AC_OUTPUT
