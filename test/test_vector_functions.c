/* test_vector_functions.c -- check the implementation of
 * a number of vector functions
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

START_TEST(vector_length)
{
    hpix_vector_t vector;

    vector = (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.3 };
    TEST_FOR_CLOSENESS(hpix_vector_length(&vector),
		       0.37416573867739413856);

    vector = (hpix_vector_t) { .x = -0.6, .y = -0.9, .z = 0.4 };
    TEST_FOR_CLOSENESS(hpix_vector_length(&vector),
		       1.15325625946707958894);

    vector = (hpix_vector_t) { .x = 0.0, .y = 0.0, .z = 0.0 };
    TEST_FOR_CLOSENESS(hpix_vector_length(&vector), 0.0);
}
END_TEST

/**********************************************************************/

START_TEST(dot_product)
{
    hpix_vector_t vec1, vec2;

    vec1 = (hpix_vector_t) { .x = 1.0, .y = 0.0, .z = 0.0 };
    vec2 = (hpix_vector_t) { .x = 0.0, .y = 1.0, .z = 0.0 };

    fail_unless(hpix_dot_product(&vec1, &vec2) == 0.0);

    vec1 = (hpix_vector_t) { .x = 0.0, .y = 1.0, .z = 0.0 };
    vec2 = (hpix_vector_t) { .x = 0.0, .y = 0.0, .z = 1.0 };

    fail_unless(hpix_dot_product(&vec1, &vec2) == 0.0);

    vec1 = (hpix_vector_t) { .x = 1.0, .y = 0.0, .z = 0.0 };
    vec2 = (hpix_vector_t) { .x = 0.0, .y = 0.0, .z = 1.0 };

    fail_unless(hpix_dot_product(&vec1, &vec2) == 0.0);

    vec1 = (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.3 };
    vec2 = (hpix_vector_t) { .x = -0.4, .y = 0.5, .z = -0.6 };

    TEST_FOR_CLOSENESS(hpix_dot_product(&vec1, &vec2), -0.12);

    vec1 = (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = -0.3 };
    vec2 = (hpix_vector_t) { .x = 0.4, .y = -0.5, .z = 0.6 };

    TEST_FOR_CLOSENESS(hpix_dot_product(&vec1, &vec2), -0.24);
}
END_TEST

/**********************************************************************/

START_TEST(vector_to_versor)
{
    hpix_vector_t test_vector;
    hpix_vector_t ref_vector;

    test_vector = (hpix_vector_t) { .x = 0.3, .y = 0.0, .z = 0.0 };
    hpix_normalize_vector(&test_vector);
    ref_vector = (hpix_vector_t) { .x = 1.0, .y = 0.0, .z = 0.0 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);

    test_vector = (hpix_vector_t) { .x = 0.0, .y = 0.3, .z = 0.0 };
    hpix_normalize_vector(&test_vector);
    ref_vector = (hpix_vector_t) { .x = 0.0, .y = 1.0, .z = 0.0 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);

    test_vector = (hpix_vector_t) { .x = 0.0, .y = 0.0, .z = 0.3 };
    hpix_normalize_vector(&test_vector);
    ref_vector = (hpix_vector_t) { .x = 0.0, .y = 0.0, .z = 1.0 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);

    test_vector = (hpix_vector_t) { .x = 0.1, .y = 0.2, .z = 0.3 };
    hpix_normalize_vector(&test_vector);
    ref_vector = (hpix_vector_t) { .x = 0.26726124191242438468, 
				      .y = 0.53452248382484876937, 
				      .z = 0.80178372573727315405 };
    ARE_VECTORS_EQUAL(test_vector, ref_vector);
}
END_TEST

/**********************************************************************/

START_TEST(matrix_initialization)
{
    hpix_matrix_t test_matrix;

    const hpix_matrix_t unity_matrix = 
	(hpix_matrix_t) { .m = { { 1, 0, 0 },
				 { 0, 1, 0 },
				 { 0, 0, 1 } } };
    hpix_set_matrix_to_unity(&test_matrix);
    ARE_MATRICES_EQUAL(test_matrix, unity_matrix);

    hpix_set_matrix_to_zero(&test_matrix);
    fail_unless(hpix_is_matrix_zero(&test_matrix));

    hpix_set_matrix_to_scale_transform(&test_matrix,
				       2.0, 3.0, 4.0);
    const hpix_matrix_t scale_matrix = 
	(hpix_matrix_t) { .m = { { 2, 0, 0 },
				 { 0, 3, 0 },
				 { 0, 0, 4 } } };
    ARE_MATRICES_EQUAL(test_matrix, scale_matrix);
}
END_TEST

/**********************************************************************/

START_TEST(matrix_determinant)
{
    const hpix_matrix_t matrix = 
	(hpix_matrix_t) { .m = { { 5, 4, -1 },
				 { 3, -7, 2 },
				 { -6, 4, 2 } } };
    fail_unless(hpix_matrix_determinant(&matrix), -152);
}
END_TEST

/**********************************************************************/

START_TEST(matrix_vector_multiplication)
{
    const hpix_matrix_t matrix = 
	(hpix_matrix_t) { .m = { { 1, -2, 3 },
				 { -4, 5, -6 },
				 { 7, -8, 9 } } };
    const hpix_vector_t vector = { 10, -11, 12 };
    const hpix_vector_t reference = { 68, -167, 266 };
    hpix_vector_t result;

    hpix_matrix_vector_mul(&result, &matrix, &vector);
    ARE_VECTORS_EQUAL(result, reference);
}
END_TEST

/**********************************************************************/

START_TEST(matrix_matrix_multiplication)
{
    const hpix_matrix_t matrix1 = 
	(hpix_matrix_t) { .m = { { 1, 2, 3 },
				 { 4, 5, 6 },
				 { 7, 8, 9 } } };
    const hpix_matrix_t matrix2 = 
	(hpix_matrix_t) { .m = { { 5, 4, -1 },
				 { 3, -7, 2 },
				 { -6, 4, 2 } } };
    const hpix_matrix_t reference = 
	(hpix_matrix_t) { .m = { { -7, 2, 9 },
				 { -1, 5, 18 },
				 { 5, 8, 27 } } };
    hpix_matrix_t result;

    hpix_matrix_mul(&result, &matrix1, &matrix2);
    ARE_MATRICES_EQUAL(reference, result);
}
END_TEST

/**********************************************************************/


START_TEST(matrix_inverse)
{
    const hpix_matrix_t matrix1 = 
	(hpix_matrix_t) { .m = { { 1, 0, 0 },
				 { 0, 1, 1 },
				 { 0, 0, 1 } } };
    const hpix_matrix_t reference1 =
	(hpix_matrix_t) { .m = { { 1, 0, 0 },
				 { 0, 1, -1 },
				 { 0, 0, 1 } } };

    hpix_matrix_t inverse;
    fail_unless(hpix_matrix_inverse(&inverse, &matrix1));
    ARE_MATRICES_EQUAL(reference1, inverse);

    const hpix_matrix_t matrix2 = 
	(hpix_matrix_t) { .m = { { 0, 1, 3 }, 
				 { -6, 2, 2 }, 
				 { 1, 2, 5 }} };
    const hpix_matrix_t reference2 = 
	(hpix_matrix_t) { .m = { { -0.6, -0.1, 0.4 },
				 { -3.2, 0.3, 1.8 },
				 { 1.4, -0.1, -0.6 } } };

    fail_unless(hpix_matrix_inverse(&inverse, &matrix2));
    ARE_MATRICES_EQUAL(reference2, inverse);
}
END_TEST

/**********************************************************************/

START_TEST(matrix_euler_rotations)
{
}
END_TEST

/**********************************************************************/

Suite *
create_hpix_test_suite(void)
{
    Suite * suite = suite_create("Vector functions");
    TCase * tc_core;

    tc_core = tcase_create("Vector length");
    tcase_add_test(tc_core, vector_length);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Dot product");
    tcase_add_test(tc_core, dot_product);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Vector to versor");
    tcase_add_test(tc_core, vector_to_versor);
    suite_add_tcase(suite, tc_core);

    tc_core = tcase_create("Operations with matrices");
    tcase_add_test(tc_core, matrix_initialization);
    tcase_add_test(tc_core, matrix_determinant);
    tcase_add_test(tc_core, matrix_inverse);
    tcase_add_test(tc_core, matrix_vector_multiplication);
    tcase_add_test(tc_core, matrix_matrix_multiplication);
    tcase_add_test(tc_core, matrix_euler_rotations);
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
