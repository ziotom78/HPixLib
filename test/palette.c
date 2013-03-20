/* test_ring_to_nest.c -- check the implementation of
 * hpix_ring_to_nest_idx
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
#include <math.h>
#include <stdlib.h>
#include <check.h>

double red = 1.0;
double green = 2.0;
double blue = 3.0;

#define TEST_FOR_CLOSENESS(float1, float2)					\
    {									\
	fail_unless(fabs(float1 - float2) < 1e-7,			\
		    "Numbers " #float1 " and " #float2 " are not close."); \
    }

#define TEST_COLORS_ARE_EQUAL(col1, col2)	\
    {						\
	TEST_FOR_CLOSENESS((col1).red, (col2).red);	\
	TEST_FOR_CLOSENESS((col1).green, (col2).green);	\
	TEST_FOR_CLOSENESS((col1).blue, (col2).blue);	\
    }

#define CHECK_COLOR_AND_LEVEL(palette, entry, color, level)		\
    {									\
	TEST_COLORS_ARE_EQUAL(hpix_color_for_step_in_palette(palette, entry), \
			      color);					\
	TEST_FOR_CLOSENESS(hpix_level_for_step_in_palette(palette, entry), \
			   level);					\
    }

/**********************************************************************/

START_TEST(access_to_colors)
{
    hpix_color_t dummy_color = hpix_create_color(red, green, blue);

    TEST_FOR_CLOSENESS(hpix_red_level_from_color(&dummy_color),
		       red);
    TEST_FOR_CLOSENESS(hpix_green_level_from_color(&dummy_color),
		       green);
    TEST_FOR_CLOSENESS(hpix_blue_level_from_color(&dummy_color),
		       blue);

}
END_TEST

/**********************************************************************/

START_TEST(empty_palette)
{
    hpix_color_palette_t * palette = hpix_create_black_color_palette();

    ck_assert(palette != NULL);
    ck_assert_int_eq(hpix_num_of_steps_in_color_palette(palette),
		     2);

    hpix_color_t black_color = (hpix_color_t) { 0.0, 0.0, 0.0 };

    CHECK_COLOR_AND_LEVEL(palette, 0, black_color, 0.0);
    CHECK_COLOR_AND_LEVEL(palette, 1, black_color, 1.0);

    hpix_free_color_palette(palette);
}
END_TEST

/**********************************************************************/

START_TEST(unseen_pixel_color)
{
    hpix_color_palette_t * palette = 
	hpix_create_healpix_color_palette();
    hpix_color_t dummy_color = hpix_create_color(red, green, blue);

    hpix_set_color_for_unseen_pixels_in_palette(palette, dummy_color);
    TEST_COLORS_ARE_EQUAL(hpix_color_for_unseen_pixels_in_palette(palette),
			  dummy_color);

    hpix_free_color_palette(palette);
}
END_TEST

/**********************************************************************/

START_TEST(access_to_palettes)
{
    hpix_color_t dummy_color = hpix_create_color(red, green, blue);

    hpix_color_t black_color = (hpix_color_t) { 0.0, 0.0, 0.0 };
    hpix_color_t red_color = (hpix_color_t) { 1.0, 0.0, 0.0 };
    hpix_color_t green_color = (hpix_color_t) { 0.0, 1.0, 0.0 };
    hpix_color_t blue_color = (hpix_color_t) { 0.0, 0.0, 1.0 };

    hpix_color_palette_t * palette = hpix_create_black_color_palette();

    hpix_add_step_to_color_palette(palette, 0.2, red_color);
    hpix_add_step_to_color_palette(palette, 0.8, green_color);
    hpix_add_step_to_color_palette(palette, 0.6, blue_color);

    ck_assert_int_eq(hpix_num_of_steps_in_color_palette(palette),
		     5);

    hpix_sort_levels_in_color_palette(palette);

    ck_assert_int_eq(hpix_num_of_steps_in_color_palette(palette),
		     5);

    CHECK_COLOR_AND_LEVEL(palette, 0, black_color, 0.0);
    CHECK_COLOR_AND_LEVEL(palette, 1, red_color, 0.2);
    CHECK_COLOR_AND_LEVEL(palette, 2, blue_color, 0.6);
    CHECK_COLOR_AND_LEVEL(palette, 3, green_color, 0.8);
    CHECK_COLOR_AND_LEVEL(palette, 4, black_color, 1.0);
}
END_TEST

/**********************************************************************/

START_TEST(modify_colors_in_palette)
{
    hpix_color_palette_t * palette = 
	hpix_create_healpix_color_palette();

    hpix_color_t dummy_color = hpix_create_color(red, green, blue);
    hpix_set_color_for_step_in_palette(palette, 1, dummy_color);
    hpix_set_level_for_step_in_palette(palette, 1, 0.4);

    TEST_COLORS_ARE_EQUAL(hpix_color_for_step_in_palette(palette, 1),
			  dummy_color);
    ck_assert_int_eq(hpix_level_for_step_in_palette(palette, 1),
	       0.4);
}
END_TEST

/**********************************************************************/

START_TEST(check_interpolation)
{
    hpix_color_palette_t * palette = 
	hpix_create_black_color_palette();

    hpix_color_t color0 = hpix_create_color(0.1, 0.3, 0.5);
    hpix_color_t color1 = hpix_create_color(0.9, 0.8, 0.2);

    hpix_set_color_for_step_in_palette(palette, 0, color0);
    hpix_set_color_for_step_in_palette(palette, 1, color1);

    hpix_color_t interpolated_color =
	hpix_get_palette_color(palette, 0.5);
    TEST_FOR_CLOSENESS(hpix_red_level_from_color(&interpolated_color),
	       (color0.red + color1.red) * 0.5);
    TEST_FOR_CLOSENESS(hpix_green_level_from_color(&interpolated_color),
	       (color0.green + color1.green) * 0.5);
    TEST_FOR_CLOSENESS(hpix_blue_level_from_color(&interpolated_color),
	       (color0.blue + color1.blue) * 0.5);

    hpix_free_color_palette(palette);
}
END_TEST

/**********************************************************************/

void
add_color_and_palette_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, access_to_colors);
    tcase_add_test(testcase, empty_palette);
    tcase_add_test(testcase, unseen_pixel_color);
    tcase_add_test(testcase, access_to_palettes);
    tcase_add_test(testcase, modify_colors_in_palette);
    tcase_add_test(testcase, check_interpolation);
}

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Colors and palettes");

    TCase * tc_core = tcase_create("Colors and palettes");
    add_color_and_palette_tests_to_testcase(tc_core);

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
