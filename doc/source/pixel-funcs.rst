Pixel functions
===============

In this section we describe the functions implemented by CHealpix that
allow to associate points on the sky sphere with pixels in the HEALPix
tessellation.

Converting NSIDE into the number of pixels and back
---------------------------------------------------

.. c:function:: chpx_pixel_num_t chpx_nside_to_npixel(chpx_nside_t)

  Given a value for the *nside* parameter (any positive power of two),
  return the number of pixels the sky sphere is divided into. If
  *nside* is not valid, the function returns zero.

  This function is the inverse of :c:func:`chpx_npixel_to_nside()`.

.. code-block:: c

  chpx_pixel_num_t num;
  assert((num = chpx_nside_to_npixel(8)) > 0);

.. c:function:: chpx_nside_t chpx_npixel_to_nside(chpx_pixel_num_t)

  Given the number of pixels in the sky sphere, this function returns
  the value of *NSIDE* uniquely associated with it. The function does
  not accept arbitrary values for *num_of_pixels*: any invalid value
  will make the function return zero.

  This function is the inverse of :c:func:`chpx_nside_to_npixel()`.

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

.. c:function:: void chpx_angles_to_3dvec(double theta, double phi, double * x, double * y, double * z)

  Convert the pair of angles *theta*, *phi* into a versor (one-length
  vector) *x*, *y*, *z*. The function normalizes the angles before
  applying the conversion (e.g. if *phi* is equal to 3pi, it is
  converted into pi).

  See also :c:func:`chpx_3dvec_to_angles`.

.. c:function:: chpx_pixel_num_t chpx_angles_to_ring_pixel(chpx_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`chpx_angles_to_nest_pixel`.

.. c:function:: chpx_pixel_num_t chpx_angles_to_nest_pixel(chpx_nside_t nside, double theta, double phi)

  Convert the pair of angles *theta*, *phi* into the `NESTED` index of
  the pixel for which the specified direction falls within.

  See also :c:func:`chpx_angles_to_ring_pixel`.
 
.. c:type:: typedef chpx_pixel_num_t chpx_angles_to_pixel_fn_t(chpx_nside_t, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`chpx_angles_to_ring_pixel` and
  :c:func:`chpx_angles_to_nest_pixel`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. Here's an example:

.. code-block:: c

  void
  function(chpx_nside_t nside,
           chpx_ordering_t order,
           const double * thetas,
           const double * phis,
           size_t num_of_pixels)
  {
      size_t idx;
      chpx_angles_to_pixel_fn_t * ang2pix_fn;
      if(order == CHPX_ORDER_RING)
          ang2pix_fn = chpx_angles_to_ring_pixel;
      else
          ang2pix_fn = chpx_angles_to_nest_pixel;

      for(idx = 0; idx < num_of_pixels; ++idx)
      {
          chpx_pixel_num_t pix_num;
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

.. c:function:: void chpx_3dvec_to_angles(double x, double y, double z, double * theta, double * phi)

  Convert the vector *x*, *y*, *z* into the pair of angles *theta*,
  *phi*. It is not necessary for the vector to have length one. The
  two angles will be properly normalized (i.e. *theta* will be within
  0 and pi, and *phi* will be within 0 and 2pi).

  See also :c:func:`chpx_angles_to_3dvec`.

.. c:function:: chpx_pixel_num_t chpx_3dvec_to_ring_pixel(chpx_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `RING` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`chpx_ring_pixel_to_3dvec`.

.. c:function:: chpx_pixel_num_t chpx_3dvec_to_nest_pixel(chpx_nside_t nside, double x, double y, double z)

  Convert the vector *x*, *y*, *z* into the `NESTED` index of the
  pixel for which the specified direction falls within.

  See also :c:func:`chpx_nest_pixel_to_3dvec`.

.. c:type:: typedef chpx_pixel_num_t chpx_3dvec_to_pixel_fn_t(chpx_nside_t, double, double, double)

  This defines a name for the prototype of the two functions
  :c:func:`chpx_3dvec_to_ring_pixel` and
  :c:func:`chpx_3dvec_to_nest_pixel`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`chpx_angles_to_pixel_fn_t` for a
  nice example.

Converting pixel indexes
........................

The functions described in this paragraph convert pixel indices,
either in `RING` or `NESTED` scheme, into some other representation.


.. c:function:: void chpx_ring_pixel_to_angles(chpx_nside_t nside, chpx_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`chpx_angles_to_ring_pixel`.

.. c:function:: void chpx_nest_pixel_to_angles(chpx_nside_t nside, chpx_pixel_num_t pixel, double * theta, double * phi)

  Convert the direction of the center of the pixel with `NESTED` index
  *pixel* into the two angles *theta* (colatitude) and *phi*
  (longitude).

  See also :c:func:`chpx_angles_to_nest_pixel`.

.. c:type:: typedef void chpx_pixel_to_angles(chpx_nside_t, chpx_pixel_num_t, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`chpx_ring_pixel_to_angles` and
  :c:func:`chpx_nest_pixel_to_angles`. It is useful if you plan to
  call many times one of the two functions, but you do not know in
  advance which one you'll use. See
  :c:type:`chpx_angles_to_pixel_fn_t` for a nice example.

.. c:function:: void chpx_ring_pixel_to_3dvec(chpx_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`chpx_3dvec_to_ring_pixel`.

.. c:function:: void chpx_nest_pixel_to_3dvec(chpx_nside_t nside, double * x, double * y, double * z)

  Convert the direction of the center of the pixel with `RING` index
  *pixel* into the components of a vector *x*, *y*, *z*. It is
  guaranteed that `x*x + y*y + z*z == 1.0`.

  See also :c:func:`chpx_3dvec_to_ring_pixel`.

.. c:type:: typedef void chpx_pixel_to_3dvec(chpx_nside_t, chpx_pixel_num_t, double *, double *, double *)

  This defines a name for the prototype of the two functions
  :c:func:`chpx_ring_pixel_to_3dvec` and
  :c:func:`chpx_nest_pixel_to_3dvec`. It is useful if you plan to call
  many times one of the two functions, but you do not know in advance
  which one you'll use. See :c:type:`chpx_angles_to_pixel_fn_t` for a
  nice example.
