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

PKG_CHECK_MODULES([cairo], [cairo], 
	[cairo=yes
	 AC_DEFINE(HAVE_CAIRO, 1, [Define to 1 if you have cairo available])],
	[cairo=no])

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
