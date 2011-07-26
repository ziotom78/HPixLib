% -*- tex -*-
% Copyright 2011 Maurizio Tomasi. All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions
% are met:
%
%   1. Redistributions of source code must retain the above copyright
%      notice, this list of conditions and the following disclaimer.
%
%   2. Redistributions in binary form must reproduce the above
%      copyright notice, this list of conditions and the following
%      disclaimer in the documentation and/or other materials provided
%      with the distribution.
%
% THIS SOFTWARE IS PROVIDED BY MAURIZIO TOMASI ``AS IS'' AND ANY
% EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
% PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MAURIZIO TOMASI OR
% CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
% SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
% LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
% USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
% ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
% OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
% OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
% SUCH DAMAGE.
%
% The views and conclusions contained in the software and
% documentation are those of the authors and should not be interpreted
% as representing official policies, either expressed or implied, of
% Maurizio Tomasi.

\let\ifpdf\relax
\ifx\eplain\undefined \input eplain \fi

@* Introduction.

This is the manual of the CHEALPix library, a small C library which
implements the HEALPix tesselation of the sphere. The purpose of the
HEALPix tesselation is to divide a sphere into patches of equal area
which have desirable properties for a number of calculations mostly
used in astrophysics and cosmology, e.g.:

\numberedlist
\li Computation of the spherical Fourier transforms for a
random field on the sphere;
\li Efficient nearest-neighbour searches of pixels.
\endnumberedlist

@ The standard implementation of the HEALPix tesselation scheme is the
HEALPix library developed by Gorski et al. It provides a number of
functions in Fortran, C, C++, IDL and Java and is so far the most
complete and widely used implementation. Still, it suffers of a number
of drawbacks:

\numberedlist
\li There are no connections among the language bindings. (In
fact, instead of speaking of ``bindings'' --- which imply that there
is one library and many interfaces that binds to it --- one would
rather speak of four different libraries: one for each language.) This
implies that if one wants to implement some alternative algorithm for
a HEALPix routine, she has to code the algorithm once for each
language supported.

\li The separation of the language bindings has had the effect of
reduce the momentum in developing some of them. The C bindings appear
to be the most neglected, for there is no facility for
reading/saving/doing calculations on $a_{\ell m}$ coefficients. This
is particularly limiting for those developers wanting to create
bindings to other languages by exploiting Foreign Function Interfaces
(FFIs), as C is the {\it lingua franca} used for FFI.

\li The C/C++ bindings of the library use Doxygen to produce
the documentation. Although Doxygen is a powerful tool, the HEALPix
function are usually poorly documented: there is no example of use for
any of the C++ functions, and the description of each function is
usually only a one-liner.

\li The installation of HEALPix is not straightforward: the
library requires the user to install CFITSIO first, which is a rather
large library if compared with what CFITSIO uses (the largest part of
the CFITSIO code implements functions for reading/writing images,
while HEALPix only reads and write binary tables). Moreover, there is
no standard facility for a program using HEALPix to find and link the
library.
\endnumberedlist

@ This document presents a new C library which implements the HEALPix
tesselation scheme, with the following advantages over the standard
HEALPix library:

\numberedlist

\li Only the C language is supported. This reduces the size of
the library and eases the development.

\li The library tries to support all the facilities provided by
the standard HEALPix library, even those implemented in languages
other than C.

\li The library is written using |CWEB|, a literate programming
tool developed by Donald E.\ Knuth and Silvio Levy which provides a
powerful way to document the code.

\li The library supports {\tt pkg-config}.

\endnumberedlist

@ This is the only header of the library.

@(chealpix.h@>=
#ifndef CHEALPIX_H
#define CHEALPIX_H

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

#include "qfits_table.h"

#define CHEALPIX_VERSION "0.1"

typedef unsigned long pixel_num_t;

@<Definition of common data types@>
@<Prototypes for generic pixel functions@>

#ifdef __cplusplus
};
#endif __cplusplus
#endif

@ And here is the skeleton of the source file that will contain {\it
all} the functions defined by the library.

@p
#include <chealpix.h>
#include <math.h>

@<Generic pixel functions@>


@* Common data types: maps, harmonic coefficients, power spectra. In
this section we define a number of data structures that are used by
the \.{chealpix} library.

@ We start with the definition of |chealpix_map_t|, a structure that
describes a Healpix map. The fields used by the original Healpix
library to identify a map are:

\numberedlist

\li The values of each pixel, listed in a specified ordering scheme;

\li The ordering scheme (either \.{RING} or \.{NESTED});

\li The value of \.{NSIDE}, which is used to determine the number of
pixels and the resolution of the map;

\li The coordinate system used by the map (e.g. Galactic).

\endnumberedlist

@<Definition of common data types@>=
typedef enum {
    CHEALPIX_ORDER_UNKNOWN,
    CHEALPIX_ORDER_RING,
    CHEALPIX_ODER_NEST
} chealpix_ordering_t;

typedef enum {
    CHEALPIX_COORD_UNKNOWN,
    CHEALPIX_COORD_ECLIPTIC,
    CHEALPIX_COORD_GALACTIC
} chealpix_coordinates_t;

typedef struct {
    chealpix_ordering_t    order;
    chealpix_coordinates_t coord;
    unsigned short         nside;
    tfits_type             type;
    void                   * pixels;
} chealpix_map_t;


@* Generic pixel functions.
In this section we provide a number of generic functions which apply
to pixels but not to maps. Here we list the prototypes of the
functions we are going to implement in this section.

@<Prototypes for generic pixel functions@>=
pixel_num_t chealpix_nside_to_npixel(const unsigned short);
unsigned short chealpix_npixel_to_nside(const pixel_num_t);


@ The most easy functions to implement are those which convert between
the value of NSIDE and the number $n_p$ of pixels in a map. This is
specified by the following relation:
$$
n_p = 12 \times {\rm NSIDE}^2,
$$ which is defined for ${\rm NSIDE} > 0$. For both the direct
(${\rm NSIDE} \rightarrow n_p$) and inverse ($n_p \rightarrow {\rm
NSIDE}$) conversion I chose to return 0 if an invalid value for
\.{nside} is passed (since both |nside| and |npixels| are |unsigned|,
the only invalid value is zero for both). Note that in order to check
if a given value for $n_p$ is valid I use the fact that the
composition of the direct and inverse conversions must be the
identity.

@<Generic pixel functions@>=
pixel_num_t
chealpix_nside_to_npixel(const unsigned short nside)
{
    if(nside > 0)
        return 12 * nside * nside;
    else
        return 0;
}

unsigned short
chealpix_npixel_to_nside(const pixel_num_t npixels)
{
    unsigned short nside_estimate = sqrt(npixels / 12.0);
    if(chealpix_nside_to_npixel(nside_estimate) != npixels)
        return 0;
    else
        return nside_estimate;
}


@* Map I/O.

In this section we implement a number of functions for reading and
writing maps from and to FITS files. We rely on the \.{qfits} library,
as it is small enough to be included in the source code of
\.{chealpix} and is BSD-licensed.


@* Drawing maps.

We are now ready to implement the trickiest part of the library, that
is the code that produces a graphical representation of a map. To
better understand the difficulties of such task, let us consider how
this is accomplished by the standard Healpix library by Gorski {\it et
al} and by Healpy. The ``standard'' Healpix implementation is able to
plot maps in a number of ways:

\unorderedlist

\li The IDL library contains roughly 7\,600 lines of code
which implement {\tt MOLLVIEW} and similar functions. Such functions
are written in pure IDL and use the IDL plotting functions.

\li A standalone program, {\tt map2gif}, converts a map saved in a
FITS file into a GIF image. Such program is written in Fortran90 and
is roughly 1\,200 lines of code, plus the source code of the {\tt
gifdraw} library (roughly 12\,000 lines of code, in the directory {\tt
src/f90/lib} of the tarball).

\li A standalone program, {\tt map2tga}, does the same as {\tt
map2gif}, but it is written in C++ and takes 800 lines of code. It
uses no external graphics library to produce the plots.

\endunorderedlist

It is worth stressing that IDL is the only language binding that
implements a set of functions for plotting maps. Both {\tt map2tga}
and {\tt map2gif} are {\it standalone} programs and do not expose any
facility to the library user for incorporating their functionality
into a larger program (unless of couse the program calls them in
background to produce a image file which is then read by the program
itself).

Because of this situation, the creators of the Healpy Python library
decided to implement a set of plotting routines from scratch. More
than 2\,000 lines of code are needed to implement functions like {\tt
mollview} and {\tt mollzoom}; they are based on the well-known {\tt
matplotlib} library.

Our approach is to implement a very generic interface for map plotting
in cHealpix (i.e.\ one that is agnostic to the tool actually used to
draw the map: Quartz, Gtk+, Cairo\dots). Depending on the graphics
library, there are two possible approaches for drawing a map:

\numberedlist

\li Generate a bitmap. (This is the approach followed by the Healpix
library and by Healpy.) The output of the process is a $N\times M$
matrix of pixels whose elements are calculated using a ray-tracing
algorithm. The image has a fixed resolution, which implies that it
shows poor results when enlarged. The ray-tracing algorithm has the
advantage of being quite fast, and bitmaps can be displayed and saved
quickly. When saved, the size of the file scales with the number of
elements in the matrix, but it is independent of the number of pixels
in the map.

\li Generate a vector image. This solution has the drawback of
producing very large files when \.{NSIDE} is large, but vector maps
scale very well when enlarged. The typical formats used to store such
maps are Postscript and PDF.

\endnumberedlist

cHealpix provides two sets of functions to ease the production of
bitmapped and vector maps. Such functions need to be wrapped with some
glue code which actually writes the map on disk or display it on the
screen.

@ Generating bitmapped graphics.

First.

@
