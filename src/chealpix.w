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

\li The coordinate system used by the map (e.g. Galactic,
Ecliptic\ldots).
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

@ 
