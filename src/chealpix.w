@ This is the manual of the CHEALPix library, a small C library which
implements the HEALPix tesselation of the sphere. The purpose of the
HEALPix tesselation is to divide a sphere into patches of equal area
which have desirable properties for a number of calculations mostly
used in astrophysics and cosmology, e.g.:

\item{(1)}{Computation of the spherical Fourier transforms for a
random field on the sphere;}
\item{(2)}{Efficient nearest-neighbour searches of pixels.}

@ The standard implementation of the HEALPix tesselation scheme is the
HEALPix library developed by Gorski et al. It provides a number of
functions in Fortran, C, C++, IDL and Java and is so far the most
complete and widely used implementation. Still, it suffers of a number
of drawbacks:

\item{(1)}{There are no connections among the language bindings. (In
fact, instead of speaking of ``bindings'' --- which imply that there
is one library and many interfaces that binds to it --- one would
rather speak of four different libraries: one for each language.) This
implies that if one wants to implement some alternative algorithm for
a HEALPix routine, she has to code the algorithm once for each
language supported.}

\item{(2)}{The separation of the language bindings has had the effect
of reduce the momentum in developing some of them. The C bindings
appear to be the most neglected, for there is no facility for
reading/saving/doing calculations on $a_{\ell m}$ coefficients. This
is particularly limiting for those developers wanting to create
bindings to other languages, as C is the {\it lingua franca} used for
Foreign Function Interfaces (FFIs).}

\item{(3)}{The C/C++ bindings of the library use Doxygen to produce
the documentation. Although Doxygen is a powerful tool, the HEALPix
function are usually poorly documented: there is no example of use for
any of the C++ functions, and the description of each function is
usually only a one-liner.}

\item{(4)}{The installation of HEALPix is not straightforward: the
library requires the user to install CFITSIO first, which is a rather
large library if compared with what CFITSIO uses (the largest part of
the CFITSIO code implements functions for reading/writing images,
while HEALPix only reads and write binary tables). Moreover, there is
no standard facility for a program using HEALPix to find and link the
library.}

@ This document presents a new C library which implements the HEALPix
tesselation scheme, with the following advantages over the standard
HEALPix library:

\item{(1)}{Only the C language is supported. This reduces the size of
the library and eases the development.}

\item{(2)}{The library tries to support all the facilities provided by
the standard HEALPix library, even those implemented in languages
other than C.}

\item{(3)}{The library is written using |CWEB|, a literate programming
tool developed by Donald E.\ Knuth and Silvio Levy which provides a
powerful way to document the code.}

\item{(4)}{The library supports {\tt pkg-config}.}

@ This is the main header of the library.

@(chealpix.h@>=
#ifndef CHEALPIX_H
#define CHEALPIX_H

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

#define CHEALPIX_VERSION "0.1"

typedef unsigned long pixel_num_t;

pixel_num_t chealpix_nside_to_npixel(const unsigned short);

#ifdef __cplusplus
};
#endif __cplusplus
#endif

@* 

@ We begin with something easy: 

@p
#include <chealpix.h>

pixel_num_t
chealpix_nside_to_npixel(const unsigned short nside)
{
	return 12 * nside * nside;
}
