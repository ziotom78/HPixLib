Pixel functions
===============

In this section we describe the functions implemented by HPixLib that
allow to associate points on the sky sphere with pixels in the HEALPix
tessellation.

Converting NSIDE into the number of pixels and back
---------------------------------------------------

.. c:function:: hpxlib_pixel_num_t hpxlib_nside_to_npixel(hpxlib_nside_t)

  Given a value for the *nside* parameter (any positive power of two),
  return the number of pixels the sky sphere is divided into. If
  *nside* is not valid, the function returns zero.

  This function is the inverse of :c:func:`hpxlib_npixel_to_nside()`.

.. code-block:: c

  hpxlib_pixel_num_t num;
  assert((num = hpxlib_nside_to_npixel(8)) > 0);

.. c:function:: hpxlib_nside_t hpxlib_npixel_to_nside(hpxlib_pixel_num_t)

  Given the number of pixels in the sky sphere, this function returns
  the value of *NSIDE* uniquely associated with it. The function does
  not accept arbitrary values for *num_of_pixels*: any invalid value
  will make the function return zero.

  This function is the inverse of :c:func:`hpxlib_nside_to_npixel()`.

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

Converting angular positions
............................

The functions described in this paragraph convert angular positions
(*theta*, *phi*) into some other representation.

.. c:function:: void hpxlib_angles_to_3dvec(double theta, double phi, double * x, double * y, double * z)

  Convert the pair of angles *theta*, *phi* into a versor (one-length
  vector) *x*, *y*, *z*. The function normalizes the angles before
  applying the conversion (e.g. if *phi* is equal to 3pi, it is
  converted into pi).

  See also :c:func:`hpxlib_3dvec_to_angles`.

.. c:function:: hpxlib_pixel_num_t hpxlib_angles_to_ring_pixel(hpxlib_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpxlib_angles_to_nest_pixel`.

.. c:function:: hpxlib_pixel_num_t hpxlib_angles_to_nest_pixel(hpxlib_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `NESTED` index of
  the pixel for which the specified direction falls within.

  See also :c:func:`hpxlib_angles_to_ring_pixel`.
 
.. c:type:: typedef hpxlib_pixel_num_t hpxlib_angles_to_pixel_fn_t(hpxlib_nside_t, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`hpxlib_angles_to_ring_pixel` and
  :c:func:`hpxlib_angles_to_nest_pixel`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. Here's an example:

.. code-block:: c

  void
  function(hpxlib_nside_t nside,
           hpxlib_ordering_t order,
           const double * thetas,
           const double * phis,
           size_t num_of_pixels)
  {
      size_t idx;
      hpxlib_angles_to_pixel_fn_t * ang2pix_fn;
      if(order == HPXLIB_ORDER_RING)
          ang2pix_fn = hpxlib_angles_to_ring_pixel;
      else
          ang2pix_fn = hpxlib_angles_to_nest_pixel;

      for(idx = 0; idx < num_of_pixels; ++idx)
      {
          hpxlib_pixel_num_t pix_num;
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

.. c:function:: void hpxlib_3dvec_to_angles(double x, double y, double z, double * theta, double * phi)

  Convert the vector *x*, *y*, *z* into the pair of angles *theta*,
  *phi*. It is not necessary for the vector to have length one. The
  two angles will be properly normalized (i.e. *theta* will be within
  0 and pi, and *phi* will be within 0 and 2pi).

  See also :c:func:`hpxlib_angles_to_3dvec`.

.. c:function:: hpxlib_pixel_num_t hpxlib_3dvec_to_ring_pixel(hpxlib_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpxlib_ring_pixel_to_3dvec`.

.. c:function:: hpxlib_pixel_num_t hpxlib_3dvec_to_nest_pixel(hpxlib_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `NESTED` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`hpxlib_nest_pixel_to_3dvec`.

.. c:type:: typedef hpxlib_pixel_num_t hpxlib_3dvec_to_pixel_fn_t(hpxlib_nside_t, double, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`hpxlib_3dvec_to_ring_pixel` and
  :c:func:`hpxlib_3dvec_to_nest_pixel`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`hpxlib_angles_to_pixel_fn_t` for a
  nice example.

Converting pixel indexes
........................

The functions described in this paragraph convert pixel indices,
either in `RING` or `NESTED` scheme, into some other representation.


.. c:function:: void hpxlib_ring_pixel_to_angles(hpxlib_nside_t nside, hpxlib_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`hpxlib_angles_to_ring_pixel`.

.. c:function:: void hpxlib_nest_pixel_to_angles(hpxlib_nside_t nside, hpxlib_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `NESTED` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`hpxlib_angles_to_nest_pixel`.

.. c:type:: typedef void hpxlib_pixel_to_angles(hpxlib_nside_t, hpxlib_pixel_num_t, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`hpxlib_ring_pixel_to_angles` and
  :c:func:`hpxlib_nest_pixel_to_angles`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. See
  :c:type:`hpxlib_angles_to_pixel_fn_t` for a nice example.

.. c:function:: void hpxlib_ring_pixel_to_3dvec(hpxlib_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`hpxlib_3dvec_to_ring_pixel`.

.. c:function:: void hpxlib_nest_pixel_to_3dvec(hpxlib_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`hpxlib_3dvec_to_ring_pixel`.

.. c:type:: typedef void hpxlib_pixel_to_3dvec(hpxlib_nside_t, hpxlib_pixel_num_t, double *, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`hpxlib_ring_pixel_to_3dvec` and
  :c:func:`hpxlib_nest_pixel_to_3dvec`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`hpxlib_angles_to_pixel_fn_t` for a
  nice example.

Converting RING into NESTED and back
------------------------------------

The following functions allow you to switch between the `RING` and
`NESTED` schemes. Each scheme has its own advantages: `RING` is good
when you want to decompose the map in spherical harmonics (because
pixels on the same latitude are contiguous), `NESTED` is useful if you
apply wavelet transforms or are looking for point sources (neighbour
points are easy to find with this scheme).

.. c:function:: hpxlib_pixel_num_t hpxlib_nest_to_ring_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t nest_index)

Convert the index of pixel *nest_index* from `NESTED` to `RING`.

.. c:function:: hpxlib_pixel_num_t hpxlib_ring_to_nest_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t ring_index)

Convert the index of pixel *nest_index* from `NESTED` to `RING`.

.. c:function:: void hpxlib_switch_order(hpxlib_map_t * map)

Switch the order of the map from `RING` to `NESTED` or vice versa,
depending on the current ordering of the map (see
:c:func:`hpxlib_map_ordering`). Note that the reordering is done
in-place: this means that no additional memory is needed during the
conversion.
