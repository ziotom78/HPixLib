/* test_nside_to_npixel.c -- check the implementation of
 * hpix_nside_to_npixel
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
    int result = 1;

    result = result && (hpix_nside_to_npixel(64)   ==   64 *   64 * 12);
    result = result && (hpix_nside_to_npixel(1024) == 1024 * 1024 * 12);

    /* Check for failures */
    result = result && (hpix_nside_to_npixel(0) == 0);

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
