/* bmp_projection.c -- check the accessor functions for
 * chpx_bmp_projection_t
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
    hpix_bmp_projection_t * proj;
    TEST_INIT;

    proj = hpix_create_bmp_projection(10, 20);
    TEST_EQUAL(hpix_bmp_projection_width(proj), 10);
    TEST_EQUAL(hpix_bmp_projection_height(proj), 20);

    hpix_set_bmp_projection_width(proj, 15);
    hpix_set_bmp_projection_height(proj, 25);
    TEST_EQUAL(hpix_bmp_projection_width(proj), 15);
    TEST_EQUAL(hpix_bmp_projection_height(proj), 25);

    hpix_free_bmp_projection(proj);

    TEST_EXIT;
}
