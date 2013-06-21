AC_INIT([hpixlib], [0.0.1])
AC_CONFIG_SRCDIR([src/map.c])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

m4_include([m4/pkg.m4])

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

AM_CONDITIONAL([CAIRO_PRESENT], [test x$cairo = xyes])

AC_OUTPUT(Makefile
	src/Makefile
	test/Makefile
	examples/Makefile
	utilities/Makefile
	libhpix.pc)

echo ""
echo "  HPixLib will be compiled with the following options:"
echo ""
echo "  cairo: $cairo"
echo ""
