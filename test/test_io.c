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

#include <hpixlib/hpix.h>
#include <fitsio.h>
#include <stdlib.h>
#include "libtest.h"

#define FILE_NAME "test.fits"

int
main(void)
{
    hpix_map_t * map_to_save = hpix_create_map(4, HPIX_ORDER_RING);
    hpix_map_t * loaded_map;
    size_t index;
    int status = 0;

    TEST_INIT;

    /* Initialize each pixel in the map with its own index */
    for(index = 0; index < hpix_map_num_of_pixels(map_to_save); ++index)
	*(hpix_map_pixels(map_to_save) + index) = index;

    /* Try to save this map */
    TEST_TRUE(hpix_save_fits_component_to_file(FILE_NAME, map_to_save,
					       TULONG, "", &status) != 0);
    
    /* Load the map back again */
    hpix_load_fits_component_from_file("test.fits", 1, &loaded_map, &status);
    TEST_TRUE(loaded_map != NULL);

    /* Now check that the two maps (map_to_save and loaded_map) are
     * actually the same. Note that we use integer types (unsigned
     * char) so that every comparison is exact. */

    TEST_EQUAL(hpix_map_nside(map_to_save), hpix_map_nside(loaded_map));

    for(index = 0; index < hpix_map_num_of_pixels(map_to_save); ++index)
    {
	unsigned long pixel1 =
	    (unsigned long) HPIX_MAP_PIXEL(map_to_save, index);
	unsigned long pixel2 = 
	    (unsigned long) HPIX_MAP_PIXEL(loaded_map, index);

	TEST_EQUAL(pixel1, pixel2);
    }

    hpix_free_map(map_to_save);
    hpix_free_map(loaded_map);

    TEST_EXIT;
}
