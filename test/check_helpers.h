/* check_helpers.h -- macros and functions based on the "check" library
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

#ifndef CHECK_HELPERS_H
#define CHECK_HELPERS_H

#define TEST_FOR_CLOSENESS(float1, float2)					\
    {									\
	fail_unless(fabs(float1 - float2) < 1e-7,			\
		    "Numbers " #float1 " and " #float2 " are not close."); \
    }

#define ARE_VECTORS_EQUAL(vec1, vec2) \
    { \
	TEST_FOR_CLOSENESS((vec1).x, (vec2).x);	\
	TEST_FOR_CLOSENESS((vec1).y, (vec2).y);	\
	TEST_FOR_CLOSENESS((vec1).z, (vec2).z);	\
    }

#define ARE_MATRICES_EQUAL(test, reference)		   \
    {							   \
	size_t __i, __j;				   \
	for(__i = 0; __i < 3; ++__i)			   \
	{						   \
	    for(__j = 0; __j < 3; ++__j)		   \
	    {						   \
		TEST_FOR_CLOSENESS(test.m[__i][__j],	   \
				   reference.m[__i][__j]); \
	    }						   \
	}						   \
    }

#endif
