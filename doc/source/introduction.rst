Introduction
============

This manual describes HPixLib, a C library that implements the Healpix
spherical tesselation scheme described by Górski et al. (The
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
Fortran, C, C++, IDL and Java and is so far the most complete, tested
and widely used implementation.

HPixLib has not been designed to compete with the Healpix library. It
should instead be considered as an orthogonal project, which tries to
address some of the following issues, without trying to re-implement
everything:

* In Healpix there are no connections among the language bindings. (In
  fact, instead of speaking of "bindings" -- which imply that there is
  one library and many interfaces that binds to it -- one would rather
  speak of four different implementations: one for each language.)
  Apart from some code duplication, this has had the effect of
  reducing the momentum in developing some of them. The C bindings
  appear to be the most neglected, for there is no facility for
  reading/saving/doing calculations on :math:`a_{\ell m}`
  coefficients. This is particularly limiting for those developers
  wanting to create bindings to other languages by exploiting Foreign
  Function Interfaces (FFIs), as C is the *lingua franca* used for
  FFIs.

* The fact that the C binding is the most neglected is however
  unfortunate, as this is the standard de facto for writing bindings
  to other languages (e.g. Python, GNU R...).

* The installation of HEALPix is not straightforward: the library
  requires the user to install CFITSIO first, which is a rather large
  library if compared with what Healpix uses (the largest part of the
  CFITSIO code implements functions for reading/writing images, while
  HEALPix only reads and write binary tables). Moreover, there is no
  standard facility for a program using HEALPix to find and link the
  Healpix library (i.e. no support for pkg-config).

* No facilities to draw maps are provided in the C/C++ library. (A
  shortcut is to can use the standalone programs map2tga or map2gif to
  create a file which you then read back in your program.)

Purpose of HPixLib with respect to Healpix
------------------------------------------

HPixLib is meant to solve these issues. Of course, in order to do this
I had to do a few compromises, so that a number of users of the
Górski's library should stay with it. Here are the advantages of
HPixLib:

* Only the C language is supported, and functions are more
  "low-level". This reduces the size of the library and eases its
  development, at the expense of loosing the majority of the
  scientists (which usually use Fortran, IDL or Python).

* It only supports maps of double values (the C++ bindings of the
  Healpix library use template and the user can therefore create maps
  of ints, booleans and so on). Since any 32-bit integer can be
  represented exactly in a 8-byte double, this means that precision is
  rarely an issue. However, using this approach you can easily waste
  lot of memory.

* Although it is meant as a basis for creating bindings to other
  languages, HPixLib itself only provides C bindings (i.e. no
  Fortran/IDL support). Also, even if you can use HPixLib in a C++
  program, the library is not going to use all those nice features of
  C++ like std::vector and templates.

* The library provides an extensible interface to draw maps (as well
  as a standalone program, map2fig, which is able to produce bitmapped
  graphics as well as vector graphics).

* The library uses the GNU Autotools to configure itself and supports
  pkg-config.
