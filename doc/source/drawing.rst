Drawing maps
============

In this section we describe the most complex part of the library, that
is the code that produces a graphical representation of a map. To
better understand the difficulties of such task, let us consider how
this is accomplished by the standard HEALPix library and by `Healpy
<https://github.com/healpy/healpy>`_, a Python wrapper to HEALPix. The
"standard" Healpix implementation is able to plot maps in a number of
ways:

* The IDL library contains roughly 7\,600 lines of code which
  implement :func:`MOLLVIEW` and similar functions. Such functions are
  written in pure IDL and use the IDL plotting functions.

* Two standalone programs, map2gif and map2tga, convert a map saved in
  a FITS file into a GIF/TGA image. The first one is written in
  Fortran90 and is roughly 1,200 lines of code, plus the source code
  of the gifdraw library (roughly 12,000 lines of code, in the
  directory :file:`src/f90/lib` of the tarball). The same figures
  apply to map2tga as well.

It is worth stressing that IDL is the only language binding that
implements a set of functions for plotting maps. Both map2tga and
map2gif are *standalone* programs and do not expose any facility to
the library user for incorporating their functionality into a larger
program (unless of couse the program calls them in background to
produce a image file which is then read by the program itself).

Because of this situation, the creators of the Healpy Python library
decided to implement a set of plotting routines from scratch. More
than 2,000 lines of code are needed to implement functions like
:func:`mollview` and :func:`mollzoom`; they are based on the
well-known `matplotlib <http://matplotlib.sourceforge.net/>`_ library.

Our approach is to implement a very generic interface for map plotting
in CHealpix (i.e. one that is agnostic to the tool actually used to
draw the map: Quartz, `Gtk+ <http://www.gtk.org/>`_, `Cairo
<http://www.cairographics.org>`_ …). Depending on the graphics
library, there are two possible approaches for drawing a map:

* Generate a bitmap. (This is the approach followed by the Healpix
  library and by Healpy.) The output of the process is a
  :math:`N\times M` matrix of pixels whose elements are calculated
  using a ray-tracing algorithm. The image has a fixed resolution,
  which implies that it shows poor results when enlarged. The
  ray-tracing algorithm has the advantage of being quite fast, and
  bitmaps can be displayed and saved quickly. When saved, the size of
  the file scales with the number of elements in the matrix, but it is
  independent of the number of pixels in the map.

* Generate a vector image. This solution has the drawback of producing
  very large files when *nside* is large, but vector maps scale very
  well when enlarged. The typical formats used to store such maps are
  Postscript and PDF.

CHealpix provides two sets of functions to ease the production of
bitmapped and vector maps. Such functions need to be wrapped with some
glue code which actually writes the map on disk or display it on the
screen.

Bitmapped graphics
------------------

The interface provided by CHealpix for the generation of bitmapped
graphics clearly shows the ray-tracing algorithm on whom it is
grounded. In the following discussion we try to prevent the ambiguity
between a "pixel" in a Healpix map and a "pixel" in a bitmap by
referring to the second as "an element in the :math:`N \times M`
matrix," or "matrix element" for short. All the functions implemented
in this section have their name beginning with ``chpx_bmp_``.

.. c:type:: chpx_bmp_projection_t

   This type contains all the information needed to transform a
   Healpix map into a bi-dimensional bitmapped projection. It is an
   opaque structure, which means that you are not allowed to directly
   access/modify its members: you need to rely on functions defined in
   this section, like e.g. :c:func:`chpx_projection_width` and
   :c:func:`chpx_set_projection_width`.

.. c:function:: chpx_bmp_projection_t * chpx_create_bmp_projection(unsigned int width, unsigned int height)

   Create a new :c:type:`chpx_bmp_projection_t` object and initialize
   its width and height. This object must be deallocated using
   :c:func:`chpx_free_bmp_projection`.

.. c:function:: void chpx_free_bmp_projection(chpx_bmp_projection_t * proj)

Projection properties
---------------------

As said above, :c:type:`chpx_bmp_projection_t` is an opaque structure
and as such you cannot read/modify its members directly: you have to
use the facilities provided by the library.his.

.. c:function:: unsigned int chpx_projection_width(const chpx_bmp_projection_t * proj)

   Return the width of the bitmap, i.e. the number of columns.

.. c:function:: unsigned int chpx_projection_height(const chpx_bmp_projection_t * proj)

   Return the height of the bitmap, i.e. the number of rows.

.. c:function:: void chpx_set_projection_width(chpx_bmp_projection_t * proj, unsigned int width)

   Change the width of the bitmap.

.. c:function:: void chpx_set_projection_height(chpx_bmp_projection_t * proj, unsigned int height)

   Change the height of the bitmap.

Painting functions
------------------

.. c:function:: double * chpx_bmp_trace_bitmap(const chpx_bmp_projection_t * proj, const chpx_map_t * map, double * min_value, double * max_value)

   This function creates a bitmap (rectangular array of numbers)
   representing *map*. The details of the projection are specified by
   the *proj* parameter (size of the bitmap, set of coordinates to be
   used and so on). The bitmap is an array of floating-point values,
   each using the same scale as in the original map (i.e. if the map
   represents a set of temperatures in Kelvin, then each pixel in the
   bitmap will be measured in Kelvin as well).
  
   When the bitmap returned by this function is no longer useful, you
   must free it using :c:func:`chpx_free`.
  
   The typical usage is to produce a bitmap, then use *min_value* and
   *max_value* to scale it from the map measure unit into a color
   space. In the following example we imagine to use a graphics
   library which implements two functions: ``paint_pixel``, which draw
   a pixel at a specified coordinate with a given color, and
   ``level_to_RGB``, which converts a number between 0.0 and 1.0 into
   a RGB color. Here is the code:

.. code-block:: c

   chpx_bmp_projection_t * proj;
   double * bitmap;
   double * cur_pixel;
   double min, max;
   size_t i, x, y; 
  
   proj = chpx_new_projection(640, 480, COORD_GALACTIC);
   bitmap = chpx_bmp_trace_bitmap(proj, map, &min, &max);
  
   cur_pixel = bitmap;
   for(y = 0; y < chpx_projection_height(proj); ++y)
   {
       for(x = 0; x < chpx_projection_width(proj); ++x)
       {
           float red, green, blue;
           level_to_RGB((cur_pixel++ - min) / (max - min),
	                &red, &green, &blue);
           paint_pixel(x, y, red, green, blue);
       }
   }
  
   chpx_free(bitmap);
   chpx_free_projection(proj);


Vector graphics
---------------
