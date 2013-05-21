The hpix_map_t type
===================

In this section we introduce the :c:type:`hpix_map_t` type, which is
used to hold information about a map, as well as a number of ancillary
types and functions.

The following example (`examples/mapinfo.c`) is a program which shows
information about a set of FITS temperature maps specified from the
command line. It is a good example of the way HPixLib functions are
meant to be used in a real program (albeit as simple as this is). In
the rest of this section a detailed documentation of every function
used in the example will be provided.

.. code-block:: c

  #include <hpixlib/hpix.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <math.h>
  #include <assert.h>
  
  /* Compute the peak-to-peak difference of the value of the
     pixels in the map */
  double peak_to_peak_amplitude(const hpix_map_t * map)
  {
    size_t idx;
    double min, max;
    double * pixels;
  
    assert(map);
  
    pixels = hpix_map_pixels(map);
    min = max = pixels[0];
    for(int idx = 1; idx < hpix_map_num_of_pixels(map); ++idx)
    {
      if(isnan(pixels[idx])) /* Skip unseen pixels */
        continue;
  
      if(min > pixels[idx])
        min = pixels[idx];
  
      if(max < pixels[idx])
        max = pixels[idx];
    }
  
    return max - min;
  }
  
  int main(int argc, char ** argv)
  {
    char * error_message = NULL;
  
    /* Skip the program name */
    ++argv; --argc;
  
    if(argc == 0) {
        fputs("Usage: mapinfo FILE1 [FILE2...]\n", stderr);
        return EXIT_SUCCESS;
    }
  
    while(argc--) {
        int cfitsio_status = 0;
        hpix_map_t * map;
  
        hpix_load_fits_component_from_file(argv[0], 1, &map, &cfitsio_status);
  
        if(map)
        {
  	  printf("File name: %s\n", *argv);
  	  printf("NSIDE: %u\n", hpix_map_nside(map));
  	  printf("Ordering: %s\n",
  		 hpix_map_ordering_scheme(map) == HPIX_ORDER_SCHEME_RING ?
  		 "RING" : "NEST");
  	  printf("Peak-to-peak variation: %.4g\n",
  		 peak_to_peak_amplitude(map));
  	  
  	  hpix_free_map(map);
        } else {
  	  fprintf(stderr, "Error: %s\n", error_message);
  	  hpix_free(error_message);
        }
        
        ++argv;
    }
    
    return EXIT_SUCCESS;
  }


Basic types
-----------

.. c:type:: hpix_ordering_scheme_t

  This ``enum`` type specifies the ordering scheme of the map. It can assume
  the values ``HPIX_ORDER_SCHEME_RING`` or ``HPIX_ORDER_SCHEME_NEST``.

.. c:type:: hpix_coordinates_t

  This ``enum`` type specifies the coordinate system used by the map.
  It can either be ``HPIX_COORD_GALACTIC`` (Galactic coordinates),
  ``HPIX_COORD_ECLIPTIC`` (ecliptic coordinates),
  ``HPIX_COORD_CELESTIAL`` or ``HPIX_COORD_CUSTOM`` (custom Euler
  rotation).

.. c:type:: hpix_resolution_t

  This structure is conceptually equivalent to a *nside* value, but it
  keeps a number of mathematical quantities (all derived by *nside*
  itself) that are handy for manipulating Healpix maps at that
  resolution. (It basically caches these values in order to save time
  in computations.)

.. c:type:: hpix_map_t

  This is the basic type used to hold information about a Healpix
  map. It is a structure that should considered to be opaque, i.e.
  accessing its members is forbidden. You should instead use access
  functions like :c:func:`hpix_map_ordering()`, :c:func:`hpix_map_nside()`
  and :c:func:`hpix_map_pixels()`. See below for a complete list.

Map creation/distruction
------------------------

Functions :c:func:`hpix_create_map()` and
:c:func:`hpix_create_map_from_array` create a map in memory. The first
one is useful when you do now know in advance the value of the pixels
you're going to put into the pixel. The second one is handy if you
were able to retrieve pixel values from some medium and want to "wrap"
them into a :c:type:`hpix_map_t` structure in order to use them with
HPixLib.

.. c:function:: hpix_map_t * hpix_create_map(hpix_nside_t nside, hpix_ordering_scheme_t ordering)

  Create a zero-filled Healpix map with a resolution of *nside* and a
  ordering scheme equal to *ordering* (see :c:type:`hpix_ordering_scheme_t`
  for more information about the accepted values).

.. c:function:: hpix_map_t * hpix_create_map_from_array(double * array, size_t num_of_elements, hpix_ordering_scheme_t ordering)

  Create a Healpix map using the values in *array*. The value of
  *nside* is calculated from *num_of_pixels* using
  :c:func:`hpix_npixel_to_nside()`. By default, the map is considered to
  be in Galactic coordinates.

.. c:function:: void hpix_free_map(hpix_map_t * map)

  Free any memory associated with *map*. Once the function exits,
  *map* is no longer available.

.. c:function:: hpix_map_t * hpix_create_copy_of_map(const hpix_map_t * map)

  Return a pointer to a copy of *map*. This is useful if you plan to
  modify *map* inplace (e.g. by means of a call to
  :c:func:`hpix_scale_pixels_by_constant_inplace`) but you want to
  keep a copy of the map as it was before the modification. Once no
  longer used, the new copy must be disposed using
  :c:func:`hpix_free_map` as usual.

Loading and saving maps
-----------------------

The following functions are used to load and save Healpix maps into
FITS files. Such files are fully compatible with those produced by the
standard Healpix library.

.. c:function:: int hpix_load_fits_component_from_fitsptr(fitsptr * fptr, unsigned short column_number, hpix_map_t ** map, int * status)

  Load one component (I, Q, or U) from the FITS file specified by
  *fptr*, which must have been properly initialized using one of
  CFITSIO's functions, e.g. :c:func:`fits_open_table()` and
  :c:func:`fits_movabs_hdu()`.

  If any error occurs, the function returns zero. Otherwise, it makes
  *map* pointing to a new :c:type:`hpix_map_t` object that must be
  freed using :c:func:`hpix_free_map()` when it is no longer useful.
  Moreover, if *status* is not null, then it will be initialized with
  the appropriate CFITSIO error code.

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int hpix_load_fits_component_from_file(const char * file_name, unsigned short column_number, hpix_map_t ** map, int * status)

  Wrapper to :c:func:`hpix_load_fits_component_from_fitsptr` which
  automatically opens the FITS file named *file_name* and moves to the
  first binary table HDU.

.. c:function:: int hpix_create_empty_fits_table_for_map(fitsfile * fptr, const hpix_map_t * template_map, unsigned short num_of_components, const char * measure_unit, int * status)

  Create a new HDU in an already-opened FITS file pointed by *fptr*
  and write a set of keywords that describe the shape of a map like
  *template_map*. The parameter *num_of_components* tells how many
  `TDOUBLE` columns the HDU will have: it must be a number between 1
  and 3. (No checking is done on this.)

  The parameter *measure_unit* should be a string identifying the unit
  of measure of all the columns. You should use short names, e.g. `K`
  instead of `Kelvin`.

  If the function is successful, it returns nonzero. If there is an
  error and *status* is not null, then it will be initialized with the
  appropriate CFITSIO code.

  Note that write-access must be granted to *fptr*, otherwise the
  function will fail.

.. c:function:: int hpix_save_fits_component_to_fitsfile(const char * file_name, const hpix_map_t * map, int data_type, int * status)

  Save *map* into a FITS file named *file_name*. The value of
  *data_type* is one of the possible types accepted by CFITSIO (e.g.
  ``TINT``, refer to the CFITSIO documentation for a full list).

  As for :c:func:`hpix_load_fits_component_from_file()`, if something
  went wrong then the function returns zero and initializes
  *error_status* with a newly-created string describing the error. (In
  this case you must free it using :c:func:`hpix_free()`.) Note that
  *error_status* can be set to ``NULL``: in this case, no information
  about the error type will be available.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int hpix_save_fits_component_to_file(const char * file_name, const hpix_map_t * map, int data_type, int * status)

  Wrapper to :c:func:`hpix_save_fits_component_to_fitsptr` which
  automatically create a FITS file named *file_name*.

.. c:function:: int hpix_load_fits_pol_from_file(const char * file_name, hpix_map_t ** map_i, hpix_map_t ** map_q, hpix_map_t ** map_u, char ** error_status)

  Load the three components of a IQU map from a FITS file named
  *file_name*. The three components are read from the first table
  extension of the FITS file. Note that it is an error to call this
  function on temperature-only maps.

  The double pointers *map_i*, *map_q* and *map_u* must point to
  ``hpix_map_t *`` variables, which are automatically allocated by the
  function, and they must be freed using :c:func:`hpix_free_map()`.

  If any error occurs, the function returns ``NULL``, otherwise it
  returns a new :c:type:`hpix_map_t` object that must be freed using
  :c:func:`hpix_free_map()` when it is no longer useful. Moreover, if
  *status* is not null, then it will be initialized with the
  appropriate CFITSIO error code.

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int hpix_save_fits_pol_to_file(const char * file_name, const hpix_map_t * map_i, const hpix_map_t * map_q, const hpix_map_t * map_u, int data_type, char ** error_status)

  Save the three I, Q, U maps into a FITS file named *file_name*. The
  value of *data_type* is one of the possible types accepted by
  CFITSIO (e.g. ``TINT``, refer to the CFITSIO documentation for a
  full list).

  As for :c:func:`hpix_load_fits_pol_from_file()`, if something went
  wrong and *status* is not null, then it will be initialized with the
  appropriate CFITSIO error code.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int hpix_is_iqu_fits_map(const char * file_name)

  This helper functions can be used to establish if the FITS file
  named *file_name* contains a temperature map (I Stokes component) or
  a temperature+polarization map (I, Q and U Stokes components).

  This function can be useful to determine if you can call
  :c:func:`hpix_load_fits_pol_map()` or not.

Accessing map information
-------------------------

The following functions provide a quick access to a
:c:type:`hpix_map_t` type. They run in constant time and are therefore
pretty cheap to call.

.. c:function:: hpix_ordering_scheme_t hpix_map_ordering(const hpix_map_t * map)

  Return the ordering of the map. See the definition of
  :c:type:`hpix_ordering_scheme_t` for an explanation of the return value.

.. c:function:: hpix_coordinates_t hpix_map_coordinate_system(const hpix_map_t * map)

  Return the coordinate system used by the map. See the definition of
  :c:type:`hpix_coordinates_t` for an explanation of the return value.

.. c:function:: hpix_nside_t hpix_map_nside(const hpix_map_t * map)

  Return the value of *nside* for *map*.

.. c:function:: size_t hpix_num_of_pixels(const hpix_map_t * map)

  Return the number of pixels in *map*. This is always equal to
  ``hpix_nside_to_npixel(hpix_map_nside(map))``.

.. c:function:: const hpix_resolution_t * hpix_map_resolution(const hpix_map_t * map)

  Return a const pointer to a :c:type:`hpix_resolution_t` structure.
