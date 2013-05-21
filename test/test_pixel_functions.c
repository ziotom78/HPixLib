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
#include "check_helpers.h"

/**********************************************************************/

START_TEST(ilog2)
{
    ck_assert_int_eq(hpix_ilog2(4), 2);
    ck_assert_int_eq(hpix_ilog2(8), 3);
    ck_assert_int_eq(hpix_ilog2(256), 8);

    ck_assert_int_eq(hpix_ilog2(0), 0);
}
END_TEST

/**********************************************************************/

START_TEST(isqrt)
{
    ck_assert_int_eq(hpix_isqrt(0), 0);
    ck_assert_int_eq(hpix_isqrt(1), 1);
    ck_assert_int_eq(hpix_isqrt(2), 1);
    ck_assert_int_eq(hpix_isqrt(3), 1);
    ck_assert_int_eq(hpix_isqrt(4), 2);
    ck_assert_int_eq(hpix_isqrt(9), 3);

    ck_assert_int_eq(hpix_isqrt(143), 11);
    ck_assert_int_eq(hpix_isqrt(144), 12);
    ck_assert_int_eq(hpix_isqrt(145), 12);
}
END_TEST

/**********************************************************************/

void
add_integer_function_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, ilog2);
    tcase_add_test(testcase, isqrt);
}

/************************************************************************/

double red = 1.0;
double green = 2.0;
double blue = 3.0;

/**********************************************************************/

START_TEST(valid_nside)
{
    for(hpix_nside_t nside = 1; nside < 1024; nside *= 2)
	ck_assert(hpix_valid_nside(nside));
    
    ck_assert(! hpix_valid_nside(13));
    ck_assert(! hpix_valid_nside(28));
    ck_assert(! hpix_valid_nside(1025));
    ck_assert(! hpix_valid_nside(3166));
}
END_TEST

/**********************************************************************/

START_TEST(npixel_to_nside)
{
    int result = 1;
    unsigned short nside;

    for(nside = 1; nside < 1024; nside *= 2)
    {
	ck_assert_int_eq(hpix_npixel_to_nside(nside * nside * 12),
			 nside);
    }

    /* Check for failures */
    ck_assert_int_eq(hpix_npixel_to_nside(11),
		     0);
}
END_TEST

/**********************************************************************/

START_TEST(nside_to_npixel)
{
    ck_assert_int_eq(hpix_nside_to_npixel(64),   49152);
    ck_assert_int_eq(hpix_nside_to_npixel(2048), 50331648);
    ck_assert_int_eq(hpix_nside_to_npixel(0),    0);
}
END_TEST

/**********************************************************************/

START_TEST(angles_to_pixels)
{
    /* Check for the conversion to the RING scheme */
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.1, 0.1),  1861);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.2, 0.1),  7567);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.3, 0.1), 17117);

    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.1, 0.2),  1863);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.2, 0.2),  7571);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.3, 0.2), 17123);

    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.1, 0.3),  1865);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.2, 0.3),  7575);
    ck_assert_int_eq(hpix_angles_to_ring_pixel(256, 0.3, 0.3), 17129);

    /* Check for the conversion to the NEST scheme */
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.1, 0.1), 65196);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.2, 0.1), 64177);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.3, 0.1), 61128);

    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.1, 0.2), 65200);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.2, 0.2), 64193);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.3, 0.2), 61044);

    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.1, 0.3), 65180);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.2, 0.3), 64113);
    ck_assert_int_eq(hpix_angles_to_nest_pixel(256, 0.3, 0.3), 60856);
}
END_TEST

/**********************************************************************/

START_TEST(pixels_to_angles)
{
    double theta, phi;

#define CHECK_ANGLE(nside, schema, pixel, target_theta, target_phi)	\
    hpix_ ## schema ## _pixel_to_angles(nside, pixel, &theta, &phi);	\
    TEST_FOR_CLOSENESS(theta, target_theta);					\
    TEST_FOR_CLOSENESS(phi, target_phi);

    CHECK_ANGLE(256, ring,  1861, 0.09891295, 0.07600627);
    CHECK_ANGLE(256, ring,  7567, 0.19806888, 0.08867399);
    CHECK_ANGLE(256, ring, 17117, 0.29771618, 0.09289656);

    CHECK_ANGLE(256, ring,  1863, 0.09891295, 0.17734797);
    CHECK_ANGLE(256, ring,  7571, 0.19806888, 0.19001568);
    CHECK_ANGLE(256, ring, 17123, 0.29771618, 0.19423826);

    CHECK_ANGLE(256, ring,  1865, 0.09891295, 0.27868967);
    CHECK_ANGLE(256, ring,  7575, 0.19806888, 0.29135738);
    CHECK_ANGLE(256, ring, 17129, 0.29771618, 0.29557995);

    CHECK_ANGLE(256, nest, 65196, 0.09891295, 0.07600627);
    CHECK_ANGLE(256, nest, 64177, 0.19806888, 0.08867399);
    CHECK_ANGLE(256, nest, 61128, 0.29771618, 0.09289656);

    CHECK_ANGLE(256, nest, 65200, 0.09891295, 0.17734797);
    CHECK_ANGLE(256, nest, 64193, 0.19806888, 0.19001568);
    CHECK_ANGLE(256, nest, 61044, 0.29771618, 0.19423826);

    CHECK_ANGLE(256, nest, 65180, 0.09891295, 0.27868967);
    CHECK_ANGLE(256, nest, 64113, 0.19806888, 0.29135738);
    CHECK_ANGLE(256, nest, 60856, 0.29771618, 0.29557995);
}
END_TEST

/**********************************************************************/

START_TEST(angles_to_vectors)
{
    hpix_vector_t test_vector;
    hpix_vector_t ref_vector;

    hpix_angles_to_vector(0.1, 0.2, &test_vector);
    ref_vector = (hpix_vector_t) { .x = 0.0978434,
				      .y = 0.01983384,
				      .z = 0.99500417 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);

    hpix_angles_to_vector(0.4, 0.3, &test_vector);
    ref_vector = (hpix_vector_t) { .x = 0.37202555,
				      .y = 0.11508099,
				      .z = 0.92106099 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);
}
END_TEST

/**********************************************************************/

START_TEST(vectors_to_angles)
{
    hpix_vector_t vector;
    double theta, phi;

    vector = (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.3 };
    hpix_vector_to_angles(&vector, &theta, &phi);
    TEST_FOR_CLOSENESS(theta, 0.64052231);
    TEST_FOR_CLOSENESS(phi,   1.10714872);

    vector = (hpix_vector_t) { .x = 0.4, .y = 0.5, .z = 0.6 };
    hpix_vector_to_angles(&vector, &theta, &phi);
    TEST_FOR_CLOSENESS(theta, 0.81788856);
    TEST_FOR_CLOSENESS(phi,   0.89605538);
}
END_TEST

/**********************************************************************/

START_TEST(vectors_to_pixels)
{
#define CHECK_INDEX(nside, scheme, input_x, input_y, input_z, ref_index)	\
    {									\
	hpix_vector_t input_vector =					\
	    (hpix_vector_t) { .x = input_x,				\
				 .y = input_y,				\
				 .z = input_z };			\
	ck_assert_int_eq(hpix_vector_to_ ## scheme ## _pixel(nside, &input_vector), \
			 ref_index);					\
    }

    CHECK_INDEX( 256, ring, 0.1, 0.2, 0.3,   78151);
    CHECK_INDEX( 512, ring, 0.1, 0.2, 0.3,  311538);
    CHECK_INDEX(1024, ring, 0.1, 0.2, 0.3, 1247176);

    CHECK_INDEX( 256, nest, 0.1, 0.2, 0.3,  31281);
    CHECK_INDEX( 512, nest, 0.1, 0.2, 0.3, 125127);
    CHECK_INDEX(1024, nest, 0.1, 0.2, 0.3, 500510);

#undef CHECK_INDEX
}
END_TEST

/**********************************************************************/

START_TEST(pixels_to_vectors)
{
#define CHECK_PIXEL(nside, schema, index, ref_x, ref_y, ref_z)		\
    {									\
	hpix_vector_t result_vec;					\
	hpix_ ## schema ## _pixel_to_vector(nside, index, &result_vec);	\
	    TEST_FOR_CLOSENESS(result_vec.x, ref_x);			\
	    TEST_FOR_CLOSENESS(result_vec.y, ref_y);			\
	    TEST_FOR_CLOSENESS(result_vec.z, ref_z);			\
    }

    CHECK_PIXEL(256, ring,      0, 
		0.0022552716212903439, 0.0022552716212903435, 0.99999491373697913);
    CHECK_PIXEL(256, ring,   1653, 
		-0.0025019940270464042, 0.092360906323387754, 0.99572245279947913);
    CHECK_PIXEL(256, ring,  37644, 
		-0.14609170150719039, -0.40058952463088032, 0.90453592936197913);
    CHECK_PIXEL(256, ring, 588412, 
	        0.62830151534459699, 0.59819277060018217, -0.49739583333333331);

    CHECK_PIXEL(256, nest,      0, 
		0.70710438349510052, 0.70710438349510041, 0.0026041666666666665);
    CHECK_PIXEL(256, nest,   1653, 
		0.6359637575364262, 0.75112688152914975, 0.17708333333333331);
    CHECK_PIXEL(256, nest,  37644, 
		0.66743848329372024, 0.44596813666212065, 0.59635416666666663);
    CHECK_PIXEL(256, nest, 588412, 
		0.77898558859132916, 0.60792632009670888, -0.15364583333333331);

#undef CHECK_PIXEL
}
END_TEST

/**********************************************************************/

hpix_map_t * map64 = NULL;
hpix_map_t * map256 = NULL;
hpix_map_t * map512 = NULL;

const hpix_resolution_t * resol64 = NULL;
const hpix_resolution_t * resol256 = NULL;
const hpix_resolution_t * resol512 = NULL;

void
setup_order_conversions(void)
{
    map64  = hpix_create_map( 64, HPIX_ORDER_SCHEME_RING);
    map256 = hpix_create_map(256, HPIX_ORDER_SCHEME_RING);
    map512 = hpix_create_map(512, HPIX_ORDER_SCHEME_RING);

    resol64  = hpix_map_resolution(map64);
    resol256 = hpix_map_resolution(map256);
    resol512 = hpix_map_resolution(map512);
}

/**********************************************************************/

void
teardown_order_conversions(void)
{
    hpix_free_map(map64);
    hpix_free_map(map256);
    hpix_free_map(map512);
}

/**********************************************************************/

START_TEST(nest_to_ring)
{
    ck_assert_int_eq(hpix_nest_to_ring_idx( resol64,    9632),    9010);
    ck_assert_int_eq(hpix_nest_to_ring_idx(resol256,    1652),  324237);
    ck_assert_int_eq(hpix_nest_to_ring_idx(resol512, 2966186), 2800416);
}
END_TEST

/**********************************************************************/

START_TEST(ring_to_nest)
{
    ck_assert_int_eq(hpix_ring_to_nest_idx( resol64,    9010),    9632);
    ck_assert_int_eq(hpix_ring_to_nest_idx(resol256,  324237),    1652);
    ck_assert_int_eq(hpix_ring_to_nest_idx(resol512, 2800416), 2966186);
}
END_TEST

/**********************************************************************/

START_TEST(switch_order)
{
    /* A sample map with NSIDE = 2, assumed to be in RING ordering.
     * Remember that integer numbers smaller than 2^53 are represented
     * exactly as floating-point. */
    double ring_idx[] = {  0,  1,  2,  3,  4,  5,  6,  7,
			   8,  9, 10, 11, 12, 13, 14, 15, 
			  16, 17, 18, 19, 20, 21, 22, 23, 
			  24, 25, 26, 27, 28, 29, 30, 31, 
			  32, 33, 34, 35, 36, 37, 38, 39, 
			  40, 41, 42, 43, 44, 45, 46, 47 };

    /* The same map, but with NEST ordering */
    double nest_idx[] = {  3,  7, 11, 15,  2,  1,  6,  5,
			  10,  9, 14, 13, 19,  0, 23,  4,
			  27,  8, 31, 12, 17, 22, 21, 26, 
			  25, 30, 29, 18, 16, 35, 20, 39, 
			  24, 43, 28, 47, 34, 33, 38, 37, 
			  42, 41, 46, 45, 32, 36, 40, 44 };

    const size_t num_of_pixels = 48;
    hpix_map_t * map =
	hpix_create_map_from_array(ring_idx, num_of_pixels,
				   HPIX_ORDER_SCHEME_RING);
    hpix_switch_order(map);

    double * map_pixels = hpix_map_pixels(map);

    for(size_t i = 0; i < num_of_pixels; ++i)
	ck_assert_int_eq(map_pixels[i], nest_idx[i]);

    hpix_switch_order(map);
    map_pixels = hpix_map_pixels(map);

    for(size_t i = 0; i < num_of_pixels; ++i)
	ck_assert_int_eq(map_pixels[i], ring_idx[i]);
    
    hpix_free_map(map);
}
END_TEST

/**********************************************************************/

START_TEST(query_disc)
{
    ck_assert_int_eq(1, 0);
}
END_TEST

/**********************************************************************/

void
add_pixel_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, valid_nside);
    tcase_add_test(testcase, npixel_to_nside);
    tcase_add_test(testcase, nside_to_npixel);

    tcase_add_test(testcase, angles_to_pixels);
    tcase_add_test(testcase, pixels_to_angles);

    tcase_add_test(testcase, angles_to_vectors);
    tcase_add_test(testcase, vectors_to_angles);

    tcase_add_test(testcase, vectors_to_pixels);
    tcase_add_test(testcase, pixels_to_vectors);
}

/**********************************************************************/

void
add_pixel_order_tests_to_testcase(TCase * testcase)
{
    tcase_add_checked_fixture(testcase,
			      setup_order_conversions,
			      teardown_order_conversions);

    tcase_add_test(testcase, nest_to_ring);
    tcase_add_test(testcase, ring_to_nest);
}

/**********************************************************************/

void
add_map_order_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, switch_order);
}

/**********************************************************************/

void
add_query_disk_tests_to_testcase(TCase * testcase)
{
    tcase_add_test(testcase, query_disc);
}

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Pixel functions");
    TCase * tc_core;

    tc_core = tcase_create("Mathematical functions on integers");
    add_integer_function_tests_to_testcase(tc_core);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Pixel functions");
    add_pixel_tests_to_testcase(tc_core);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Pixel order swapping");
    add_pixel_order_tests_to_testcase(tc_core);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Map order swapping");
    add_map_order_tests_to_testcase(tc_core);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Querying disks");
    add_query_disk_tests_to_testcase(tc_core);
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
