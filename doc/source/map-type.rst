The hpxlib_map_t type
===================

In this section we introduce the :c:type:`hpxlib_map_t` type, which is
used to hold information about a map, as well as a number of ancillary
types and functions.

Basic types
-----------

.. c:type:: hpxlib_ordering_t

  This ``enum`` type specifies the ordering scheme of the map. It can assume
  the values ``HPXLIB_ORDER_RING`` or ``HPXLIB_ORDER_NEST``.

.. c:type:: hpxlib_coordinates_t

  This ``enum`` type specifies the coordinate system used by the map.
  It can either be ``HPXLIB_COORD_GALACTIC`` (Galactic coordinates),
  ``HPXLIB_COORD_ECLIPTIC`` (ecliptic coordinates),
  ``HPXLIB_COORD_CELESTIAL`` or ``HPXLIB_COORD_CUSTOM`` (custom Euler
  rotation).

.. c:type:: hpxlib_map_t

  This is the basic type used to hold information about a Healpix
  map. It is a structure that should considered to be opaque, i.e.
  accessing its members is forbidden. You should instead use access
  functions like :c:func:`hpxlib_map_ordering()`, :c:func:`hpxlib_map_nside()`
  and :c:func:`hpxlib_map_pixels()`. See below for a complete list.

Map creation/distruction
------------------------

Functions :c:func:`hpxlib_create_map()` and
:c:func:`hpxlib_create_map_from_array` create a map in memory. The first
one is useful when you do now know in advance the value of the pixels
you're going to put into the pixel. The second one is handy if you
were able to retrieve pixel values from some medium and want to "wrap"
them into a :c:type:`hpxlib_map_t` structure in order to use them with
HPixLib.

.. c:function:: hpxlib_map_t * hpxlib_create_map(hpxlib_nside_t nside, hpxlib_ordering_t ordering)

  Create a zero-filled Healpix map with a resolution of *nside* and a
  ordering scheme equal to *ordering* (see :c:type:`hpxlib_ordering_t`
  for more information about the accepted values).

.. c:function:: hpxlib_map_t * hpxlib_create_map_from_array(double * array, size_t num_of_elements, hpxlib_ordering_t ordering)

  Create a Healpix map using the values in *array*. The value of
  *nside* is calculated from *num_of_pixels* using
  :c:func:`hpxlib_npixel_to_nside()`. By default, the map is considered to
  be in Galactic coordinates.

.. c:function:: void hpxlib_free_map(hpxlib_map_t * map)

  Free any memory associated with *map*. Once the function exits,
  *map* is no longer available.

Loading and saving maps
-----------------------

The following functions are used to load and save Healpix maps into
FITS files. Such files are fully compatible with those produced by the
standard Healpix library.

.. c:function:: int hpxlib_load_fits_component_from_fitsptr(fitsptr * fptr, unsigned short column_number, hpxlib_map_t ** map, int * status)

  Load one component (I, Q, or U) from the FITS file specified by
  *fptr*, which must have been properly initialized using one of
  CFITSIO's functions, e.g. :c:func:`fits_open_table()` and
  :c:func:`fits_movabs_hdu()`.

  If any error occurs, the function returns zero. Otherwise, it makes
  *map* pointing to a new :c:type:`hpxlib_map_t` object that must be
  freed using :c:func:`hpxlib_free_map()` when it is no longer useful.
  Moreover, if *status* is not null, then it will be initialized with
  the appropriate CFITSIO error code.

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int hpxlib_load_fits_component_from_file(const char * file_name, unsigned short column_number, hpxlib_map_t ** map, int * status)

  Wrapper to :c:func:`hpxlib_load_fits_component_from_fitsptr` which
  automatically opens the FITS file named *file_name* and moves to the
  first binary table HDU.

.. c:function:: int hpxlib_create_empty_fits_table_for_map(fitsfile * fptr, const hpxlib_map_t * template_map, unsigned short num_of_components, const char * measure_unit, int * status)

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

.. c:function:: int hpxlib_save_fits_component_to_fitsfile(const char * file_name, const hpxlib_map_t * map, int data_type, int * status)

  Save *map* into a FITS file named *file_name*. The value of
  *data_type* is one of the possible types accepted by CFITSIO (e.g.
  ``TINT``, refer to the CFITSIO documentation for a full list).

  As for :c:func:`hpxlib_load_fits_component_from_file()`, if something
  went wrong then the function returns zero and initializes
  *error_status* with a newly-created string describing the error. (In
  this case you must free it using :c:func:`hpxlib_free()`.) Note that
  *error_status* can be set to ``NULL``: in this case, no information
  about the error type will be available.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int hpxlib_save_fits_component_to_file(const char * file_name, const hpxlib_map_t * map, int data_type, int * status)

  Wrapper to :c:func:`hpxlib_save_fits_component_to_fitsptr` which
  automatically create a FITS file named *file_name*.

.. c:function:: int hpxlib_load_fits_pol_from_file(const char * file_name, hpxlib_map_t ** map_i, hpxlib_map_t ** map_q, hpxlib_map_t ** map_u, char ** error_status)

  Load the three components of a IQU map from a FITS file named
  *file_name*. The three components are read from the first table
  extension of the FITS file. Note that it is an error to call this
  function on temperature-only maps.

  The double pointers *map_i*, *map_q* and *map_u* must point to
  ``hpxlib_map_t *`` variables, which are automatically allocated by the
  function, and they must be freed using :c:func:`hpxlib_free_map()`.

  If any error occurs, the function returns ``NULL``, otherwise it
  returns a new :c:type:`hpxlib_map_t` object that must be freed using
  :c:func:`hpxlib_free_map()` when it is no longer useful. Moreover, if
  *status* is not null, then it will be initialized with the
  appropriate CFITSIO error code.

  Note that pixels marked as ``UNSEEN`` are converted to NaN. This is
  different from what the standard Healpix library does.

.. c:function:: int hpxlib_save_fits_pol_to_file(const char * file_name, const hpxlib_map_t * map_i, const hpxlib_map_t * map_q, const hpxlib_map_t * map_u, int data_type, char ** error_status)

  Save the three I, Q, U maps into a FITS file named *file_name*. The
  value of *data_type* is one of the possible types accepted by
  CFITSIO (e.g. ``TINT``, refer to the CFITSIO documentation for a
  full list).

  As for :c:func:`hpxlib_load_fits_pol_from_file()`, if something went
  wrong and *status* is not null, then it will be initialized with the
  appropriate CFITSIO error code.

  If there are NaN values in the map pixels, they will be converted
  into the standard Healpix's ``UNSEEN`` value.

.. c:function:: int hpxlib_is_iqu_fits_map(const char * file_name)

  This helper functions can be used to establish if the FITS file
  named *file_name* contains a temperature map (I Stokes component) or
  a temperature+polarization map (I, Q and U Stokes components).

  This function can be useful to determine if you can call
  :c:func:`hpxlib_load_fits_pol_map()` or not.

Accessing map information
-------------------------

The following functions provide a quick access to a
:c:type:`hpxlib_map_t` type. They run in constant time and are therefore
pretty cheap to call.

.. c:function:: hpxlib_ordering_t hpxlib_map_ordering(const hpxlib_map_t * map)

  Return the ordering of the map. See the definition of
  :c:type:`hpxlib_ordering_t` for an explanation of the return value.

.. c:function:: hpxlib_coordinates_t hpxlib_map_coordinate_system(const hpxlib_map_t * map)

  Return the coordinate system used by the map. See the definition of
  :c:type:`hpxlib_coordinates_t` for an explanation of the return value.

.. c:function:: hpxlib_nside_t hpxlib_map_nside(const hpxlib_map_t * map)

  Return the value of *nside* for *map*.

.. c:function:: size_t hpxlib_num_of_pixels(const hpxlib_map_t * map)

  Return the number of pixels in *map*. This is always equal to
  ``hpxlib_nside_to_npixel(hpxlib_map_nside(map))``.

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
  double peak_to_peak_amplitude(const hpxlib_map_t * map)
  {
    size_t idx;
    double min, max;
    double * pixels;

    assert(map);

    pixels = hpxlib_map_pixels(map);
    min = max = pixels[0];
    for(int idx = 1; idx < hpxlib_map_num_of_pixels(map); ++idx)
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
      hpxlib_map_t * map = hpxlib_load_map(*argv, 1, &error_message);

      if(map)
      {
        printf("File name: %s\n", *argv);
        printf("NSIDE: %ud\n", hpxlib_map_nside(map));
        printf("Ordering: %s\n\n",
               hpxlib_map_ordering(map) == HPXLIB_ORDER_RING ?
               "RING" : "NEST");
        printf("Peak-to-peak variation: %.4g\n",
               peak_to_peak_amplitude(map));

        hpxlib_free_map(map);
      } else {
        fprintf(stderr, "Error: %s\n", error_message);
        hpxlib_free(error_message);
      }

      ++argv;
    }

    return EXIT_SUCCESS;
  }
