/* test_angles_to_pixel.c -- check the implementation of
 * hpxlib_angles_to_ring_pixel and hpxlib_angles_to_nest_pixel
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

#include <hpxlib.h>
#include <stdlib.h>
#include "libtest.h"

int
main(void)
{
    TEST_INIT;

    /* Check for the conversion to the RING scheme */
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.1, 0.1),  1861);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.2, 0.1),  7567);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.3, 0.1), 17117);

    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.1, 0.2),  1863);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.2, 0.2),  7571);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.3, 0.2), 17123);

    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.1, 0.3),  1865);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.2, 0.3),  7575);
    TEST_EQUAL(hpxlib_angles_to_ring_pixel(256, 0.3, 0.3), 17129);

    /* Check for the conversion to the NEST scheme */
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.1, 0.1), 65196);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.2, 0.1), 64177);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.3, 0.1), 61128);

    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.1, 0.2), 65200);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.2, 0.2), 64193);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.3, 0.2), 61044);

    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.1, 0.3), 65180);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.2, 0.3), 64113);
    TEST_EQUAL(hpxlib_angles_to_nest_pixel(256, 0.3, 0.3), 60856);

    TEST_EXIT;
}
