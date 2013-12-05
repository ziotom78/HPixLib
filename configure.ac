AC_INIT([hpixlib], [0.0.1])
AC_CONFIG_SRCDIR([src/map.c])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

LT_INIT

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_CC_C99
AC_LANG_C

AC_HEADER_STDC

AC_CONFIG_HEADERS([src/config.h])
PKG_CHECK_MODULES([cairo], [cairo], 
	[cairo=yes
	 AC_DEFINE(HAVE_CAIRO, 1, [Define to 1 if you have cairo available])],
	[cairo=no])

AC_CHECK_LIB(cfitsio, ffopen,, AC_MSG_ERROR(Cannot find the CFITSIO library.))

########################################################################
# Check for OpenMP
#
# The logic of the code is as follows:
#
# 1. If the user specified --with-openmp, force a check and produce
#    an error message if OpenMP is not available
#
# 2. If the user did not specify --with-openmp, try to use OpenMP
#    but only print a warning if it is not available

AC_ARG_WITH([openmp],
	[AS_HELP_STRING([--with-openmp], 
			[Multi-threaded parallel version using OpenMP])],
	[],
	[with_openmp=check])

AS_IF([test "x$with_openmp" != xno],
      [AX_OPENMP([openmp=yes
                  AC_DEFINE(HAVE_OPENMP,
                            1,
                            [Define to 1 if you have OpenMP available])
                  CFLAGS="$CFLAGS $OPENMP_CFLAGS"
                 ],
                 [openmp=no
                  AC_MSG_WARN([The C compiler does not support OpenMP])
                  if test "x$with_openmp" != xcheck; then
                      AC_MSG_FAILURE([--with-openmp was specified, but no OpenMP support is available])
                  fi
                 ])
      ],
      [openmp=no])

########################################################################

AM_CONDITIONAL([CAIRO_PRESENT], [test x$cairo = xyes])
AM_CONDITIONAL([OPENMP_PRESENT], [test x$openmp = xyes])
AC_SUBST(havecairo, $cairo)
AC_SUBST(haveopenmp, $openmp)

AC_OUTPUT(Makefile
	src/Makefile
	test/Makefile
	examples/Makefile
	utilities/Makefile
	libhpix.pc
	libhpix_cairo.pc)

echo ""
echo "  HPixLib will be compiled with the following options:"
echo ""
echo "  cairo: $cairo"
echo "  OpenMP: $openmp"
echo ""
