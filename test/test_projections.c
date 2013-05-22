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

#define PROJ_WIDTH 1000
#define PROJ_HEIGHT 500
hpix_bmp_projection_t * mollweide_proj = NULL;

/**********************************************************************/


void
setup_mollweide(void)
{
    mollweide_proj = hpix_create_bmp_projection(PROJ_WIDTH, PROJ_HEIGHT);
}

/**********************************************************************/


void
teardown_mollweide(void)
{
    hpix_free_bmp_projection(mollweide_proj);
}

/**********************************************************************/

START_TEST(mollweide_xy_to_angles)
{
    double theta, phi;

    /* These points are clearly outside Mollweide's ellipse */
    fail_unless(! hpix_mollweide_xy_to_angles(mollweide_proj,
					      0, 0,
					      &theta, &phi));
    fail_unless(! hpix_mollweide_xy_to_angles(mollweide_proj,
					      0, PROJ_HEIGHT - 1,
					      &theta, &phi));
    fail_unless(! hpix_mollweide_xy_to_angles(mollweide_proj,
					      PROJ_WIDTH - 1, 0,
					      &theta, &phi));
    fail_unless(! hpix_mollweide_xy_to_angles(mollweide_proj,
					      PROJ_WIDTH - 1, PROJ_HEIGHT - 1,
					      &theta, &phi));

    /* Middle point */
    hpix_mollweide_xy_to_angles(mollweide_proj,
				PROJ_WIDTH/2, PROJ_HEIGHT/2,
				&theta, &phi);
    TEST_FOR_CLOSENESS(theta, 1.5707963267948966);
    TEST_FOR_CLOSENESS(phi, 0.0);

    hpix_mollweide_xy_to_angles(mollweide_proj,
				PROJ_WIDTH/4, PROJ_HEIGHT/4,
				&theta, &phi);
    TEST_FOR_CLOSENESS(theta, 2.2255927456);
    TEST_FOR_CLOSENESS(phi, 1.8137993642);

    hpix_mollweide_xy_to_angles(mollweide_proj,
				PROJ_WIDTH/3 * 2, PROJ_HEIGHT/5 * 3,
				&theta, &phi);
    TEST_FOR_CLOSENESS(theta, 1.3150785376);
    TEST_FOR_CLOSENESS(phi, -1.0645163590);
}
END_TEST

/**********************************************************************/

START_TEST(mollweide_angles_to_xy)
{
}
END_TEST

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Map projection functions");
    TCase * tc_core;

    tc_core = tcase_create("Mollweide");
    tcase_add_checked_fixture(tc_core,
			      setup_mollweide,
			      teardown_mollweide);
    tcase_add_test(tc_core, mollweide_xy_to_angles);
    tcase_add_test(tc_core, mollweide_angles_to_xy);
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
