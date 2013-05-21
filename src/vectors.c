/* vectors.c -- 3D vector functions
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
#include <assert.h>

/**********************************************************************/


void
hpix_print_vector(FILE * output_file, 
		  const hpix_3d_vector_t * vector)
{
    fprintf(output_file,
	    "%g\t%g\t%g", 
	    vector->x, vector->y, vector->z);
}

/**********************************************************************/


double 
hpix_vector_length(const hpix_3d_vector_t * vector)
{
    assert(vector);

    if(vector->x == 0.0 && vector->y == 0.0 && vector->z == 0.0)
	return 0.0;

    return sqrt(vector->x * vector->x + 
		vector->y * vector->y + 
		vector->z * vector->z);
}

/**********************************************************************/


double
hpix_dot_product(const hpix_3d_vector_t * vector1,
		 const hpix_3d_vector_t * vector2)
{
    assert(vector1);
    assert(vector2);

    return 
	vector1->x * vector2->x +
	vector1->y * vector2->y +
	vector1->z * vector2->z;
}

/**********************************************************************/


void
hpix_normalize_vector(hpix_3d_vector_t * vector)
{
    assert(vector);

    const double len = hpix_vector_length(vector);
    if(len > 0.0)
    {
	/* Avoid declaring
	 *
	 * double inv_len = 1.0 / len 
	 *
	 * and then multiplying by inv_len just to optimize the
	 * calculations: any decent C compiler will already do this
	 * for you. */
	vector->x /= len;
	vector->y /= len;
	vector->z /= len;
    }
}
