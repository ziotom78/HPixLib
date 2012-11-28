/* test_angles_to_pixel.c -- check the implementation of
 * chpx_angles_to_ring_pixel and chpx_angles_to_nest_pixel
 *
 * Copyright 2011-2012 Maurizio Tomasi.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <hpixlib/hpix.h>
#include <stdlib.h>
#include "libtest.h"

int
main(void)
{
    double theta, phi;

    TEST_INIT;

#define CHECK_ANGLE(nside, schema, pixel, target_theta, target_phi)	\
    hpix_ ## schema ## _pixel_to_angles(nside, pixel, &theta, &phi);	\
    TEST_CLOSE(theta, target_theta);					\
    TEST_CLOSE(phi, target_phi);

    CHECK_ANGLE(256, ring,  1861, 0.09891295, 0.07600627);
    CHECK_ANGLE(256, ring,  7567, 0.19806888, 0.08867399);
    CHECK_ANGLE(256, ring, 17117, 0.29771618, 0.09289656);

    CHECK_ANGLE(256, ring,  1863, 0.09891295, 0.17734797);
    CHECK_ANGLE(256, ring,  7571, 0.19806888, 0.19001568);
    CHECK_ANGLE(256, ring, 17123, 0.29771618, 0.19423826);

    CHECK_ANGLE(256, ring,  1865, 0.09891295, 0.27868967);
    CHECK_ANGLE(256, ring,  7575, 0.19806888, 0.29135738);
    CHECK_ANGLE(256, ring, 17129, 0.29771618, 0.29557995);

    CHECK_ANGLE(256, nest, 65196, 0.09891295, 0.07600627);
    CHECK_ANGLE(256, nest, 64177, 0.19806888, 0.08867399);
    CHECK_ANGLE(256, nest, 61128, 0.29771618, 0.09289656);

    CHECK_ANGLE(256, nest, 65200, 0.09891295, 0.17734797);
    CHECK_ANGLE(256, nest, 64193, 0.19806888, 0.19001568);
    CHECK_ANGLE(256, nest, 61044, 0.29771618, 0.19423826);

    CHECK_ANGLE(256, nest, 65180, 0.09891295, 0.27868967);
    CHECK_ANGLE(256, nest, 64113, 0.19806888, 0.29135738);
    CHECK_ANGLE(256, nest, 60856, 0.29771618, 0.29557995);

    TEST_EXIT;
}
