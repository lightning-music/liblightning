#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([liblightning], [0.1.0], [lightning-music@googlegroups.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([realtime.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([ac-config])
AM_INIT_AUTOMAKE([-Wall])

# parameters for aclocal
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AM_PROG_AR
AM_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lzmq':
AC_CHECK_LIB([zmq], [zmq_ctx_new])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset strdup])

AC_CONFIG_FILES([Makefile
                 examples/Makefile])
AC_OUTPUT
