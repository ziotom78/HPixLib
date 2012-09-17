/* test_npixel_to_nside.c -- check the implementation of
 * hpxlib_npixel_to_nside
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

int
main(void)
{
    int result = 1;
    unsigned short nside;

    for(nside = 1; nside < 1024; nside *= 2)
    {
	result = result && (hpxlib_npixel_to_nside(nside * nside * 12) == nside);
    }

    /* Check for failures */
    result = result && (hpxlib_npixel_to_nside(11) == 0);

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
