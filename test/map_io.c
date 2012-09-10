/* test_map_io.c -- check the routines that save/load maps to and from
 * disk
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

#include <fitsio.h>
#include <chpx.h>
#include <stdlib.h>
#include "libtest.h"

#define FILE_NAME "test.fits"

int
main(void)
{
    chpx_map_t * map_to_save = chpx_create_map(3, sizeof(unsigned char));
    chpx_map_t * loaded_map;
    size_t index;
    char * error_status;

    TEST_INIT;

    /* Initialize each pixel in the map with its own index */
    for(index = 0; index < chpx_num_of_pixels(map_to_save); ++index)
	*(((unsigned char *) map_to_save->pixels) + index) = index;

    /* Try to save this map */
    TEST_TRUE(chpx_save_fits_map(FILE_NAME, map_to_save,
				 TINT, &error_status) != 0);
    
    /* Load the map back again */
    TEST_TRUE((loaded_map = chpx_load_fits_map("test.fits", 1, &error_status)) == NULL);

    /* Now check that the two maps (map_to_save and loaded_map) are
     * actually the same. Note that we use integer types (unsigned
     * char) so that every comparison is exact. */

    TEST_EQUAL(map_to_save->nside, loaded_map->nside);

    for(index = 0; index < chpx_num_of_pixels(map_to_save); ++index)
    {
	unsigned char pixel1 = CHPX_MAP_PIXEL(map_to_save, index, unsigned char);
	unsigned char pixel2 = CHPX_MAP_PIXEL(loaded_map, index, unsigned char);

	TEST_EQUAL(pixel1, pixel2);
    }

    chpx_free_map(map_to_save);
    chpx_free_map(loaded_map);

    TEST_EXIT;
}
