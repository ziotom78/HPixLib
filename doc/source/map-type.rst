The chpx_map_t type
===================

In this section we introduce the :c:type:`chpx_map_t` type, which is
used to hold information about a map, as well as a number of ancillary
types and functions.

Basic types
-----------

.. c:type:: chpx_ordering_t

  This ``enum`` type specifies the ordering scheme of the map. It can assume
  the values ``CHPX_ORDER_RING`` or ``CHPX_ORDER_NEST``.

.. c:type:: chpx_coordinates_t

  This ``enum`` type specifies the coordinate system used by the map.
  It can either be ``CHPX_COORD_GALACTIC`` (Galactic coordinates),
  ``CHPX_COORD_ECLIPTIC`` (ecliptic coordinates),
  ``CHPX_COORD_CELESTIAL`` or ``CHPX_COORD_CUSTOM`` (custom Euler
  rotation).

.. c:type:: chpx_map_t

  This is the basic type used to hold information about a Healpix
  map. It is a structure that should considered to be opaque, i.e.
  accessing its members is forbidden. You should instead use access
  functions like :c:func:`chpx_map_ordering()`, :c:func:`chpx_map_nside()`
  and :c:func:`chpx_map_pixels()`. See below for a complete list.

Map creation/distruction
------------------------

Functions :c:func:`chpx_create_map()` and
:c:func:`chpx_create_map_from_array` create a map in memory. The first
one is useful when you do now know in advance the value of the pixels
you're going to put into the pixel. The second one is handy if you
were able to retrieve pixel values from some medium and want to "wrap"
them into a :c:type:`chpx_map_t` structure in order to use them with
cHealpix.

.. c:function:: chpx_map_t * chpx_create_map(chpx_nside_t nside, chpx_ordering_t ordering)

  Create a zero-filled Healpix map with a resolution of *nside* and a
  ordering scheme equal to *ordering* (see :c:type:`chpx_ordering_t`
  for more information about the accepted values).

.. c:function:: chpx_map_t * chpx_create_map_from_array(double * array, size_t num_of_elements, chpx_ordering_t ordering)

  Create a Healpix map using the values in *array*. The value of
  *nside* is calculated from *num_of_pixels* using
  :c:func:`chpx_npixel_to_nside()`. By default, the map is considered to
  be in Galactic coordinates.

.. c:function:: void chpx_free_map(chpx_map_t * map)

  Free any memory associated with *map*. Once the function exits,
  *map* is no longer available.

Loading and saving maps
-----------------------

The following functions are used to load and save Healpix maps into
FITS files. Such files are fully compatible with the standard Healpix
library.

.. c:function:: int chpx_load_fits_component_from_fitsptr(fitsptr * fptr, unsigned short column_number, chpx_map_t ** map, char ** error_status)

  Load one component (I, Q, or U) from the FITS file specified by
  *fptr*, which must have been properly initialized using one of
  CFITSIO's functions, e.g. :c:func:`fits_open_table()` and
  :c:func:`fits_movabs_hdu()`.

  If any error occurs, the function returns zero. Otherwise, it makes
  *map* pointing to a new :c:type:`chpx_map_t` object that must be
  freed using :c:func:`chpx_free_map()` when it is no longer useful.
  Moreover, if *status* is not null, then it will be initialized with
  the appropriate CFITSIO error code.

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int chpx_load_fits_component_from_file(const char * file_name, unsigned short column_number, chpx_map_t ** map, char ** error_status)

  Wrapper to :c:func:`chpx_load_fits_component_from_fitsptr` which
  automatically opens the FITS file named *file_name* and moves to the
  first binary table HDU.

.. c:function:: int chpx_save_fits_component_to_file(const char * file_name, const chpx_map_t * map, int data_type, char ** error_status)

  Save *map* into a FITS file named *file_name*. The value of
  *data_type* is one of the possible types accepted by CFITSIO (e.g.
  ``TINT``, refer to the CFITSIO documentation for a full list).
  
  As for :c:func:`chpx_load_fits_component_from_file()`, if something
  went wrong then the function returns zero and initializes
  *error_status* with a newly-created string describing the error. (In
  this case you must free it using :c:func:`chpx_free()`.) Note that
  *error_status* can be set to ``NULL``: in this case, no information
  about the error type will be available.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int chpx_load_fits_pol_from_file(const char * file_name, chpx_map_t ** map_i, chpx_map_t ** map_q, chpx_map_t ** map_u, char ** error_status)

  Load the three components of a IQU map from a FITS file named
  *file_name*. The three components are read from the first table
  extension of the FITS file. Note that it is an error to call this
  function on temperature-only maps.

  The double pointers *map_i*, *map_q* and *map_u* must point to
  ``chpx_map_t *`` variables, which are automatically allocated by the
  function, and they must be freed using :c:func:`chpx_free_map()`.

  If any error occurs, the function returns ``NULL``, otherwise it
  returns a new :c:type:`chpx_map_t` object that must be freed using
  :c:func:`chpx_free_map()` when it is no longer useful. Moreover, if
  *error_status* is not null, then it will be initialized to a string
  containing an explanation of what went wrong. (If an error occurs,
  then you should free the string using :c:func:`chpx_free()`.)

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int chpx_save_fits_pol_to_file(const char * file_name, const chpx_map_t * map_i, const chpx_map_t * map_q, const chpx_map_t * map_u, int data_type, char ** error_status)

  Save *map* into a FITS file named *file_name*. The value of
  *data_type* is one of the possible types accepted by CFITSIO (e.g.
  ``TINT``, refer to the CFITSIO documentation for a full list).
  
  As for :c:func:`chpx_load_fits_pol_from_file()`, if something went
  wrong then the function returns zero and initializes *error_status*
  with a newly-created string describing the error. (In this case you
  must free it using :c:func:`chpx_free()`.) Note that *error_status*
  can be set to ``NULL``: in this case, no information about the error
  type will be available.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int chpx_is_iqu_fits_map(const char * file_name)

  This helper functions can be used to establish if the FITS file
  named *file_name* contains a temperature map (I Stokes component) or
  a temperature+polarization map (I, Q and U Stokes components).

  This function can be useful to determine if you can call
  :c:func:`chpx_load_fits_pol_map()` or not.

Accessing map information
-------------------------

The following functions provide a quick access to a
:c:type:`chpx_map_t` type. They run in constant time and are therefore
pretty cheap to call.

.. c:function:: chpx_ordering_t chpx_map_ordering(const chpx_map_t * map)

  Return the ordering of the map. See the definition of
  :c:type:`chpx_ordering_t` for an explanation of the return value.

.. c:function:: chpx_coordinates_t chpx_map_coordinate_system(const chpx_map_t * map)

  Return the coordinate system used by the map. See the definition of
  :c:type:`chpx_coordinates_t` for an explanation of the return value.

.. c:function:: chpx_nside_t chpx_map_nside(const chpx_map_t * map)

  Return the value of *nside* for *map*.

.. c:function:: size_t chpx_num_of_pixels(const chpx_map_t * map)

  Return the number of pixels in *map*. This is always equal to
  ``chpx_nside_to_npixel(chpx_map_nside(map))``.

Example: a Healpix map dumper
-----------------------------

We close this section with a fully developed example: a program which
shows information about a set of FITS temperature maps specified from
the command line.

.. code-block:: c

  #include <stdio.h>
  #include <stdlib.h>
  #include <math.h>
  #include <chpx.h>

  /* Compute the peak-to-peak variation */
  double peak_to_peak_amplitude(const chpx_map_t * map)
  {
    size_t idx;
    double min, max;
    double * pixels;

    assert(map);

    pixels = chpx_map_pixels(map);
    min = max = pixels[0];
    for(int idx = 1; idx < chpx_map_num_of_pixels(map); ++idx)
    {
      if(isnan(pixels[idx]))
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

    while(argc--)
    {
      chpx_map_t * map = chpx_load_map(*argv, 1, &error_message);

      if(map)
      {
        printf("File name: %s\n", *argv);
        printf("NSIDE: %ud\n", chpx_map_nside(map));
        printf("Ordering: %s\n\n",
               chpx_map_ordering(map) == CHPX_ORDER_RING ?
               "RING" : "NEST");
        printf("Peak-to-peak variation: %.4g\n",
               peak_to_peak_amplitude(map));

        chpx_free_map(map);
      } else {
        fprintf(stderr, "Error: %s\n", error_message);
        chpx_free(error_message);
      }

      ++argv;
    }

    return EXIT_SUCCESS;
  }
