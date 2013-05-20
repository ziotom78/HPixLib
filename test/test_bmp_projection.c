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
#include <math.h>
#include <check.h>

START_TEST(projection_size)
{
    hpix_bmp_projection_t * proj;

    proj = hpix_create_bmp_projection(10, 20);

    ck_assert_int_eq(hpix_bmp_projection_width(proj), 10);
    ck_assert_int_eq(hpix_bmp_projection_height(proj), 20);

    hpix_free_bmp_projection(proj);
}
END_TEST

/**********************************************************************/

hpix_bmp_projection_t * mollweide_proj = NULL;
double mollweide_proj_min = 0.0;
double mollweide_proj_max = 0.0;
double * mollweide_bmp = NULL;

/**********************************************************************/

void
setup_mollweide_test(void)
{
    hpix_map_t * map = hpix_create_map(8, HPIX_ORDER_SCHEME_RING);
    double *restrict array_of_pixels = hpix_map_pixels(map);

    for(hpix_pixel_num_t index = 0;
	index < hpix_map_num_of_pixels(map);
	++index)
    {
	array_of_pixels[index] = index;
    }

    mollweide_proj = hpix_create_bmp_projection(64, 64);
    mollweide_bmp = hpix_bmp_to_mollweide_proj(mollweide_proj, map,
					       &mollweide_proj_min,
					       &mollweide_proj_max);
}

/**********************************************************************/

void
teardown_mollweide_test(void)
{
    hpix_free_bmp_projection(mollweide_proj);
}

/**********************************************************************/

#include "reference_map.inc.c"

START_TEST(mollweide_projection)
{
    fail_unless(mollweide_proj_min == 0.0,
	"hpix_bmp_to_mollweide_proj failed to set the minimum value");
    fail_unless(mollweide_proj_max == 767.0,
	"hpix_bmp_to_mollweide_proj failed to set the maximum value");

    /* Check that the corners fall outside the projection... */
    fail_unless(! hpix_mollweide_is_xy_inside(mollweide_proj,  0,  0));
    fail_unless(! hpix_mollweide_is_xy_inside(mollweide_proj, 63,  0));
    fail_unless(! hpix_mollweide_is_xy_inside(mollweide_proj,  0, 63));
    fail_unless(! hpix_mollweide_is_xy_inside(mollweide_proj, 63, 63));

    /* ...and that the center doesn't */
    fail_unless(hpix_mollweide_is_xy_inside(mollweide_proj,  32,  32));

    /* Compare the bitmap with the reference saved in file
     * "reference_map.inc.c" */
    size_t index = 0;
    for(unsigned y = 0; y < 64; ++y)
    {
	for(unsigned x = 0; x < 64; ++x)
	{
	    double cur_pixel = mollweide_bmp[index];
	    if(isnan(cur_pixel))
		fail_unless(isnan(reference_bitmap[index]), 
			    "Reference pixel at x=%u, y=%u is not NAN",
			    x, y);
	    else if(isinf(cur_pixel))
		fail_unless(isinf(reference_bitmap[index]), 
			    "Reference pixel at x=%u, y=%u is not INFINITY",
			    x, y);
	    else
		fail_unless(reference_bitmap[index] == cur_pixel,
			    "Difference with reference pixel at x=%u, y=%u",
			    x, y);

	    ++index;
	}
    }
}
END_TEST

/**********************************************************************/

void
add_projection_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, projection_size);
}

/**********************************************************************/

void
add_mollweide_tests_to_testcase(TCase * testcase)
{
    tcase_add_checked_fixture(testcase,
			      setup_mollweide_test,
			      teardown_mollweide_test);
    tcase_add_test(testcase, mollweide_projection);
}

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Bitmap functions");
    TCase * tc_core;

    tc_core = tcase_create("Projection access");
    add_projection_tests_to_testcase(tc_core);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Mollweide projection");
    add_mollweide_tests_to_testcase(tc_core);
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
