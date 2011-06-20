lib_LTLIBRARIES = libchealpix.la
libchealpix_la_SOURCES = chealpix.c
include_HEADERS = chealpix.h

EXTRA_DIST = chealpix.pdf

chealpix.c chealpix.h: chealpix.w
	ctangle -bhp chealpix.w

chealpix.pdf: chealpix.tex
	pdftex $<

chealpix.tex: chealpix.w
	cweave $<
