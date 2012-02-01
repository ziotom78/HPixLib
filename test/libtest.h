/* libtest.c - Library of functions which helps to implement test cases
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

#ifndef LIBTEST_H
#define LIBTEST_H

#include <stdlib.h>
#include <math.h>

#define TEST_INIT  int test_result = EXIT_SUCCESS;
#define TEST_EXIT  exit(test_result);

#define TEST_TRUE(x)							\
    {									\
	if(! (x))							\
	{								\
	    fprintf(stderr, "%s:%d: assertion failed: "			\
		    #x " is not true\n"					\
		    __FILE__, __LINE__);				\
	    test_result = EXIT_FAILURE;					\
	}								\
    }

#define TEST_EQUAL(a,b)							\
    {									\
	if(! (a == b))							\
	{								\
	    fprintf(stderr, "%s:%d: assertion failed: "			\
		    #a " is not equal to "				\
		    #b "\n",						\
		    __FILE__, __LINE__);				\
	    test_result = EXIT_FAILURE;					\
	}								\
    }

#define TEST_CLOSE(a,b)							\
    {									\
	if(! fabs(a - b) < 1e-7)					\
	{								\
	    fprintf(stderr, "%s:%d: assertion failed: "			\
		    #a " is not close to "				\
		    #b "\n",						\
		    __FILE__, __LINE__);				\
	    test_result = EXIT_FAILURE;					\
	}								\
    }

#endif /* LIBTEST_H */
