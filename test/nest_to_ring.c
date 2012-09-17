/* test_nest_to_ring.c -- check the implementation of
 * hpxlib_nest_to_ring_idx and hpxlib_nest_to_ring_map
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

    /* Check for some random numbers */
    TEST_EQUAL(hpxlib_nest_to_ring_idx( 64,    9632),    9010);
    TEST_EQUAL(hpxlib_nest_to_ring_idx(256,    1652),  324237);
    TEST_EQUAL(hpxlib_nest_to_ring_idx(512, 2966186), 2800416);

    /* Check for failures */
    TEST_EQUAL(hpxlib_nest_to_ring_idx(4, 1000000), 0);
    TEST_EQUAL(hpxlib_nest_to_ring_idx(0, 1),       0);

    TEST_EXIT;
}
