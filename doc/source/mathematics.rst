Mathematical operations on map pixels
=====================================

This section describes the HPixLib functions that perform mathematical
calculations on the pixels of a map. The number of calculations
implemented here are surely not exhaustive, but they are the most used
operations. Nevertheless, any non-trivial operation on the pixels can
be implemented by directly accessing the array containing the pixel
values through the function :c:func:`hpix_map_pixels`.

The following simple example uses
:c:func:`hpix_scale_pixels_by_constant_inplace` to optionally converts
a map containing pixel temperatures in K into a map where temperatures
are expressed in muK.

.. code-block:: c

  hpix_map_t * map;

  /* Read/initialize `map' in some way... */
  ...

  char choice;
  printf("Do you want values to be expressed as microK (y/n)? ");
  scanf("%c", &choice);

  if(choice == 'y') {
    const double KELVIN_TO_MICROKELVIN = 1.0e6;
    hpix_scale_pixels_by_constant_inplace(map, KELVIN_TO_MICROKELVIN);
  }
  
  /* Do some calculations on `map' */
  ...

In-place transformations
------------------------

The following functions change the value of the pixels in a map. If
you are interested in keeping the old values, you should copy the map
before calling them.

.. c:function:: hpix_map_t * hpix_scale_pixels_by_constant_inplace(hpix_map_t * map, double constant)

  Multiply the value of every unmasked pixel in *map* by the
  floating-point number *constant*. Masked pixels are left as they
  are.

.. c:function:: hpix_map_t * hpix_add_constant_to_pixels_inplace(hpix_map_t * map, double constant)

  Add *constant* to the value of every pixel in *map*.

.. c:function:: void hpix_remove_monopole_from_map_inplace(hpix_map_t * map)

  Subtract the average value of the unmasked pixels from the map.

Statistical estimators
----------------------

.. c:function:: double hpix_average_pixel_value(const hpix_map_t * map)

  Return the average value of the unmasked pixels in the map.
