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
#include <check.h>

#define FILE_NAME "test.fits"

START_TEST(input_output)
{
    hpix_map_t * map_to_save = hpix_create_map(16, HPIX_ORDER_SCHEME_RING);
    hpix_map_t * loaded_map;
    int status = 0;

    /* Initialize each pixel in the map with its own index */
    for(hpix_pixel_num_t index = 0;
	index < hpix_map_num_of_pixels(map_to_save); 
	++index)
    {
	*(hpix_map_pixels(map_to_save) + index) = index;
    }

    /* Try to save this map */
    fail_unless(hpix_save_fits_component_to_file("!" FILE_NAME, map_to_save,
						 TULONG, "", &status) != 0,
		"Unable to save a map into a FITS file");
    
    /* Load the map back again */
    hpix_load_fits_component_from_file(FILE_NAME, 1, &loaded_map, &status);
    fail_unless(loaded_map != NULL,
	"Unable to load the map I've just saved into file " FILE_NAME);

    /* Now check that the two maps (map_to_save and loaded_map) are
     * actually the same. Note that we use integer types (unsigned
     * char) so that every comparison is exact. */

    ck_assert_int_eq(hpix_map_nside(map_to_save), hpix_map_nside(loaded_map));

    for(hpix_pixel_num_t index = 0;
	index < hpix_map_num_of_pixels(map_to_save);
	++index)
    {
	unsigned long pixel1 =
	    (unsigned long) HPIX_MAP_PIXEL(map_to_save, index);
	unsigned long pixel2 = 
	    (unsigned long) HPIX_MAP_PIXEL(loaded_map, index);

	ck_assert_int_eq(pixel1, pixel2);
    }

    hpix_free_map(map_to_save);
    hpix_free_map(loaded_map);
}
END_TEST

/************************************************************************/

void
add_io_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, input_output);
}

/************************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Map input/output");

    TCase * tc_core = tcase_create("Saving and loading a FITS file");
    add_io_tests_to_testcase(tc_core);

    suite_add_tcase(suite, tc_core);
    return suite;
}

/**********************************************************************/

int
main(void)
{
    int number_failed;
    Suite * palette_suite = create_hpix_test_suite();
    SRunner * runner = srunner_create(palette_suite);
    srunner_run_all(runner, CK_VERBOSE);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
