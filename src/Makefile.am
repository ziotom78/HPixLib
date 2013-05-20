# Copyright 2011 Maurizio Tomasi. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY MAURIZIO TOMASI ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MAURIZIO TOMASI OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# The views and conclusions contained in the software and
# documentation are those of the authors and should not be interpreted
# as representing official policies, either expressed or implied, of
# Maurizio Tomasi.

AM_CFLAGS = -std=c99

if CAIRO_PRESENT
ADDITIONAL_SOURCES = cairo_interface.c
endif

lib_LTLIBRARIES = libhpix.la

LIBPSHT_SOURCES = \
	psht.c \
	ylmgen_c.c \
	c_utils.c \
	fftpack.c \
	ls_fft.c \
	bluestein.c

libhpix_la_SOURCES = \
	math.c \
	mem.c \
	bitmap.c \
	misc.c \
	order_conversion.c \
	map.c \
	integer_functions.c \
	io.c \
	palette.c \
	positions.c \
	mollweide_projection.c \
	query_disc.c \
	$(ADDITIONAL_SOURCES) \
	$(LIBPSHT_SOURCES)

libhpix_la_LDFLAGS = -version-info 0:0:0

nobase_include_HEADERS = hpixlib/hpix.h

