Introduction
============

This manual describes cHealpix, a C library that implements the
Healpix spherical tesselation scheme described by Górski et al. (The
Astrophysical Journal, 622:759-771, 2005).

The purpose of HEALPix is to divide a sphere into patches of equal area
which have desirable properties for a number of calculations. It is
mostly used in astrophysics and cosmology, e.g.:

* Computation of the spherical Fourier transforms for a random field on
  the sphere;

* Efficient nearest-neighbour searches of pixels.

Traditionally, there has always been *one* implementation of the
HEALPix tesselation scheme: the so-called ``standard`` HEALPix library.

The standard Healpix implementation
-----------------------------------

The standard implementation of the HEALPix tesselation scheme is the
HEALPix library developed by Gorski et al.
(http://healpix.jpl.nasa.gov/). It provides a number of functions in
Fortran, C, C++, IDL and Java and is so far the most complete and
widely used implementation. Despite the fact that such library is
stable, tested and maintained, I believe there is the need for an
alternative library like CHealpix:

* There are no connections among the language bindings. (In fact,
  instead of speaking of "bindings" -- which imply that there is one
  library and many interfaces that binds to it -- one would rather
  speak of four different libraries: one for each language.) This
  implies that if one wants to implement some alternative algorithm
  for a HEALPix routine, she has to code the algorithm once for each
  language supported.

* The separation of the language bindings has had the effect of reduce
  the momentum in developing some of them. The C bindings appear to be
  the most neglected, for there is no facility for
  reading/saving/doing calculations on :math:`a_{\ell m}`
  coefficients. This is particularly limiting for those developers
  wanting to create bindings to other languages by exploiting Foreign
  Function Interfaces (FFIs), as C is the *lingua franca* used for
  FFIs.

* The C/C++ bindings of the library use Doxygen to produce the
  documentation. Although Doxygen is a powerful tool, the HEALPix
  function are usually poorly documented: there is no example of use
  for any of the C++ functions, and the description of each function
  is usually only a one-liner.

* The installation of HEALPix is not straightforward: the library
  requires the user to install CFITSIO first, which is a rather large
  library if compared with what Healpix uses (the largest part of the
  CFITSIO code implements functions for reading/writing images, while
  HEALPix only reads and write binary tables). Moreover, there is no
  standard facility for a program using HEALPix to find and link the
  Healpix library (i.e. no support for pkg-config).

Purpose of cHealpix with respect to Healpix
-------------------------------------------

cHealpix is meant to solve these issues. Of course, in order to do
this I had to do a few compromises, so that a number of users of the
Górski's library should stay with it. Here are the advantages of
CHealpix:

* Only the C language is supported, and functions are more
  "low-level". This reduces the size of the library and eases its
  development, at the expense of loosing Fortran/IDL developers (which
  are the majority of Healpix' library users).

* It only supports maps of double values (the C++ bindings of the
  Healpix library use template and the user can therefore create maps
  of ints, booleans and so on). Since any 32-bit integer can be
  represented exactly in a 8-byte double, this means that precision is
  rarely an issue, but memory can be easily wasted.

* Although it is meant as a basis for creating bindings to other
  languages, CHealpix itself only provides C bindings (i.e. no
  Fortran/IDL support). Also, even if you can use CHealpix in a C++
  program, the library is not going to use all those nice features of
  C++ like std::vector and templates.

* The library tries to support all the facilities provided by the
  standard HEALPix library, even those implemented in languages other
  than C (e.g. projecting maps to bitmapped images).

* The documentation (what you are reading) tries to be comprehensive
  and full of examples.

* The library uses the GNU Autotools to configure itself and supports
  pkg-config.
