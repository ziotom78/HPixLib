Pixel functions
===============

In this section we describe the functions implemented by HPixLib that
allow to associate points on the sky sphere with pixels in the HEALPix
tessellation. These functions are the core of the library; HPixLib
uses the same algorithms implemented in the C++ bindings (version
3.00) of the reference Healpix library.

Converting NSIDE into the number of pixels and back
---------------------------------------------------

The Healpix tessellation subdivides the sphere in a set of pixels of
equal area. The number of pixels is uniquely specified through a
positive integer parameter, *nside*, which is related to the number of
pixels through a well-defined mathematical expression, implemented by
the function :c:func:`hpix_nside_to_npixel` (the inverse calculation
is implemented by :c:func:`hpix_npixel_to_nside`). The value of
*nside* must be an integer power of two. To check if a given integer
value satisfies these condition, HPixLib implements the function
:c:func:`hpix_valid_nside`:

.. code-block:: c

  hpix_nside_t nside;
  printf("Enter a value for nside: ");
  scanf("%u", &nside);
  if(hpix_valid_nside(nside)) {
    printf("The number of pixels in the map is %u\n",
           hpix_nside_to_npixel(nside));
  } else {
    printf("Invalid value for nside.\n");
  }

.. c:function:: _Bool hpix_valid_nside(hpix_nside_t nside)

  Return nonzero if the value of *nside* satisfies the following
  conditions:

  1. It is an integer greater than zero;
  2. It is an integer power of two.

.. c:function:: hpix_pixel_num_t hpix_nside_to_npixel(hpix_nside_t)

  Given a value for the *nside* parameter (any positive power of two),
  return the number of pixels the sky sphere is divided into. If
  *nside* is not valid, the function returns zero.

  This function is the inverse of :c:func:`hpix_npixel_to_nside()`.

.. code-block:: c

  hpix_pixel_num_t num;
  assert((num = hpix_nside_to_npixel(8)) > 0);

.. c:function:: hpix_nside_t hpix_npixel_to_nside(hpix_pixel_num_t)

  Given the number of pixels in the sky sphere, this function returns
  the value of *NSIDE* uniquely associated with it. The function does
  not accept arbitrary values for *num_of_pixels*: any invalid value
  will make the function return zero.

  This function is the inverse of :c:func:`hpix_nside_to_npixel()`.

Converting among angles, vectors and pixel centers
--------------------------------------------------

The following functions implement conversions between three different
representations of points on a sphere:

#. Angular positions. These are expressed by *theta* (colatitude, from
   0 to pi) and *phi* (longitude, from 0 to 2pi).
#. Versors (vectors normalized to have length one). These are
   expressed by three coordinates, *x*, *y*, *z*, with the constraint
   that `x*x + y*y + z*z == 1.0`.
#. Index of pixel centers. These can either be expressed using Healpix' `RING`
   or `NEST` numbering scheme, so technically it is not one but two
   representations.

It is important to note that any conversion involving pixel centers is
only approximate, e.g. you cannot convert *theta* and *phi* into a
pixel index and then back to *theta* and *phi*, and expect to get the
same values.

The following example shows how to identify the pixel in a map which
corresponds to a given coordinate pair. Note that you must ensure that
the map is expressed in the same coordinate system as the angle you
are providing: in the example, the position of M42 is specified in
Galactic coordinates, and therefore the map must have been created
using this coordinate system as well.

.. code-block:: c

  const hpix_nside_t NSIDE = 64;
  const double DEG2RAD = 0.01745;

  /* Position of OriA in Galactic coordinates (degrees) */
  double M42_position[] = { 209.01, -19.38 };

  /* Convert the latitude in colatitude */
  M42_position[0] = M42_position[0] - 180.0;

  /* Here we assume to work with maps in RING order */
  hpix_pixel_num_t pixel_index = 
      hpix_angles_to_ring_pixel(NSIDE,
                                M42_position[0] * DEG2RAD,
                                M42_position[1] * DEG2RAD);

Converting angular positions
............................

The functions described in this paragraph convert angular positions
(*theta*, *phi*) into some other representation.

.. c:function:: void hpix_angles_to_vector(double theta, double phi, double * x, double * y, double * z)

  Convert the pair of angles *theta*, *phi* into a versor (one-length
  vector) *x*, *y*, *z*. The function normalizes the angles before
  applying the conversion (e.g. if *phi* is equal to 3pi, it is
  converted into pi).

  See also :c:func:`hpix_vector_to_angles`.

.. c:function:: hpix_pixel_num_t hpix_angles_to_ring_pixel(hpix_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpix_angles_to_nest_pixel`.

.. c:function:: hpix_pixel_num_t hpix_angles_to_nest_pixel(hpix_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `NESTED` index of
  the pixel for which the specified direction falls within.

  See also :c:func:`hpix_angles_to_ring_pixel`.
 
.. c:type:: typedef hpix_pixel_num_t hpix_angles_to_pixel_fn_t(hpix_nside_t, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`hpix_angles_to_ring_pixel` and
  :c:func:`hpix_angles_to_nest_pixel`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. Here's an example:

.. code-block:: c

  void
  function(hpix_nside_t nside,
           hpix_ordering_t order,
           const double * thetas,
           const double * phis,
           size_t num_of_pixels)
  {
      size_t idx;
      hpix_angles_to_pixel_fn_t * ang2pix_fn;
      if(order == HPIX_ORDER_RING)
          ang2pix_fn = hpix_angles_to_ring_pixel;
      else
          ang2pix_fn = hpix_angles_to_nest_pixel;

      for(idx = 0; idx < num_of_pixels; ++idx)
      {
          hpix_pixel_num_t pix_num;
          /* Since ang2pix_fn has already been assigned, we
           * avoid using a `if` within the `for` cycle.
           */
          pix_num = ang2pix_fn(nside, thetas[idx], phis[idx]);

          /* Here you use `pix_num` */
      }
  }

Converting 3D vectors
.....................

The functions described in this paragraph convert 3D vectors into some
other representation. The vector does not need to have length one.

.. c:function:: void hpix_vector_to_angles(double x, double y, double z, double * theta, double * phi)

  Convert the vector *x*, *y*, *z* into the pair of angles *theta*,
  *phi*. It is not necessary for the vector to have length one. The
  two angles will be properly normalized (i.e. *theta* will be within
  0 and pi, and *phi* will be within 0 and 2pi).

  See also :c:func:`hpix_angles_to_vector`.

.. c:function:: hpix_pixel_num_t hpix_vector_to_ring_pixel(hpix_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpix_ring_pixel_to_vector`.

.. c:function:: hpix_pixel_num_t hpix_vector_to_nest_pixel(hpix_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `NESTED` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpix_nest_pixel_to_vector`.

.. c:type:: typedef hpix_pixel_num_t hpix_vector_to_pixel_fn_t(hpix_nside_t, double, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`hpix_vector_to_ring_pixel` and
  :c:func:`hpix_vector_to_nest_pixel`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`hpix_angles_to_pixel_fn_t` for a
  nice example.

Converting pixel indexes
........................

The functions described in this paragraph convert pixel indices,
either in `RING` or `NESTED` scheme, into some other representation.


.. c:function:: void hpix_ring_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`hpix_angles_to_ring_pixel`.

.. c:function:: void hpix_nest_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `NESTED` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`hpix_angles_to_nest_pixel`.

.. c:type:: typedef void hpix_pixel_to_angles(hpix_nside_t, hpix_pixel_num_t, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`hpix_ring_pixel_to_angles` and
  :c:func:`hpix_nest_pixel_to_angles`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. See
  :c:type:`hpix_angles_to_pixel_fn_t` for a nice example.

.. c:function:: void hpix_ring_pixel_to_vector(hpix_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`hpix_vector_to_ring_pixel`.

.. c:function:: void hpix_nest_pixel_to_vector(hpix_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`hpix_vector_to_ring_pixel`.

.. c:type:: typedef void hpix_pixel_to_vector(hpix_nside_t, hpix_pixel_num_t, double *, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`hpix_ring_pixel_to_vector` and
  :c:func:`hpix_nest_pixel_to_vector`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`hpix_angles_to_pixel_fn_t` for a
  nice example.

Converting RING into NESTED and back
------------------------------------

The following functions allow you to switch between the `RING` and
`NESTED` schemes. Each scheme has its own advantages: `RING` is good
when you want to decompose the map in spherical harmonics (because
pixels on the same latitude are contiguous), `NESTED` is useful if you
apply wavelet transforms or are looking for point sources (neighbour
points are easy to find with this scheme).

.. c:function:: hpix_pixel_num_t hpix_nest_to_ring_idx(hpix_nside_t nside, hpix_pixel_num_t nest_index)

Convert the index of pixel *nest_index* from `NESTED` to `RING`.

.. c:function:: hpix_pixel_num_t hpix_ring_to_nest_idx(hpix_nside_t nside, hpix_pixel_num_t ring_index)

Convert the index of pixel *nest_index* from `NESTED` to `RING`.

.. c:function:: void hpix_switch_order(hpix_map_t * map)

Switch the order of the map from `RING` to `NESTED` or vice versa,
depending on the current ordering of the map (see
:c:func:`hpix_map_ordering`). Note that the reordering is done
in-place: this means that no additional memory is needed during the
conversion, but if you want to access both maps you have to copy it
somewhere else before calling this function.
