Pixel functions
===============

In this section we describe the functions implemented by CHealpix that
allow to associate points on the sky sphere with pixels in the HEALPix
tessellation.

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
