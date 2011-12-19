/* test_ring_to_nest.c -- check the implementation of
 * chpx_ring_to_nest_idx
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

#include <chpx.h>
#include <stdlib.h>

int
main(void)
{
    int result = 1;

    /* Check for some random numbers */
    result = result && (chpx_ring_to_nest_idx( 64,    9010) ==    9632);
    result = result && (chpx_ring_to_nest_idx(256,  324237) ==    1652);
    result = result && (chpx_ring_to_nest_idx(512, 2800416) == 2966186);

    /* Check for failures */
    result = result && (chpx_ring_to_nest_idx(4, 1000000) == 0);
    result = result && (chpx_ring_to_nest_idx(0, 1) == 0);

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
