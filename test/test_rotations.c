/* test_projections.c -- check the functions that implement map projections
 *
 * Copyright 2011-2013 Maurizio Tomasi.
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
#include "check_helpers.h"

/**********************************************************************/

START_TEST(angular_distance)
{
    /* Pass a zero vector and check that the function returns NaN */
    {
	const hpix_vector_t null_vector =
	    (hpix_vector_t) { .x = 0, .y = 0, .z = 0 };
	const hpix_vector_t vector =
	    (hpix_vector_t) { .x = 1.0, .y = 1.0, .z = 1.0 };

	double distance =
	    hpix_calc_angular_distance_from_vectors(&null_vector,
						    &vector);
	fail_unless(isnan(distance));
    }
    
    /* First "true" test */
    {
	const hpix_vector_t vec1 =
	    (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.3 };
	const hpix_vector_t vec2 =
	    (hpix_vector_t) { .x = 0.2, .y = -0.1, .z = 0.4 };

	double distance =
	    hpix_calc_angular_distance_from_vectors(&vec1, &vec2);
	TEST_FOR_CLOSENESS(distance, 0.79560295);
    }

    /* Another test */
    {
	const hpix_vector_t vec1 =
	    (hpix_vector_t) { .x = -0.1, .y = 0.1, .z = 0.0 };
	const hpix_vector_t vec2 =
	    (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.4 };

	double distance =
	    hpix_calc_angular_distance_from_vectors(&vec1, &vec2);
	TEST_FOR_CLOSENESS(distance, 1.41587401);
    }
}
END_TEST

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Rotation functions");
    TCase * tc_core;

    tc_core = tcase_create("Vector operations");
    tcase_add_test(tc_core, angular_distance);
    suite_add_tcase(suite, tc_core);

    return suite;
}

/**********************************************************************/

int
main(void)
{
    int number_failed;
    Suite * suite = create_hpix_test_suite();
    SRunner * runner = srunner_create(suite);
    srunner_run_all(runner, CK_VERBOSE);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
