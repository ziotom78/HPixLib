/* test_switch_order.c -- check the implementation of
 * hpix_switch_order
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

int
main(void)
{
    /* A sample map with NSIDE = 2, assumed to be in RING ordering */
    int ring_idx[] = {  0,  1,  2,  3,  4,  5,  6,  7,
		        8,  9, 10, 11, 12, 13, 14, 15, 
		       16, 17, 18, 19, 20, 21, 22, 23, 
		       24, 25, 26, 27, 28, 29, 30, 31, 
		       32, 33, 34, 35, 36, 37, 38, 39, 
		       40, 41, 42, 43, 44, 45, 46, 47 };

    /* The same map, but with NEST ordering */
    int nest_idx[] = {   3,  7, 11, 15,  2,  1,  6,  5,
			10,  9, 14, 13, 19,  0, 23,  4,
			27,  8, 31, 12, 17, 22, 21, 26, 
			25, 30, 29, 18, 16, 35, 20, 39, 
			24, 43, 28, 47, 34, 33, 38, 37, 
			42, 41, 46, 45, 32, 36, 40, 44 };

    int result = 0; /* Implement the test! */

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
