/* mollweide_projection.c -- Functions to convert a map into a
 * Mollweide projection and vice-versa
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

#include "constants.h"

/**********************************************************************/


static inline void
mollweide_xy_to_uv(const hpix_bmp_projection_t * proj,
		   unsigned int x,
		   unsigned int y,
		   double * u,
		   double * v)
{
    assert(proj != NULL);
    assert(u != NULL);
    assert(v != NULL);

    const double center_x = hpix_bmp_projection_width(proj) / 2.0;
    const double center_y = hpix_bmp_projection_height(proj) / 2.0;

    assert(center_x > 0.0);
    assert(center_y > 0.0);

    *u = 2.0 * (x - center_x) / center_x;
    *v = (y - center_y) / center_y;
}

/**********************************************************************/


inline _Bool
mollweide_is_uv_inside(double u, double v)
{
    return u*u/4 + v*v < 1;
}

/**********************************************************************/


_Bool
hpix_mollweide_is_xy_inside(const hpix_bmp_projection_t * proj,
			    unsigned int x,
			    unsigned int y)
{
    double u, v;
    mollweide_xy_to_uv(proj, x, y, &u, &v);

    return mollweide_is_uv_inside(u, v);
}

/**********************************************************************/


_Bool
hpix_mollweide_xy_to_angles(const hpix_bmp_projection_t * proj,
			    unsigned int x,
			    unsigned int y,
			    double * theta,
			    double * phi)
{
    double u, v;
    mollweide_xy_to_uv(proj, x, y, &u, &v);
    if (! mollweide_is_uv_inside(u, v))
	return FALSE;

    const double asin_v = asin(v);
    const double cos_asin_v = cos(asin_v);
    *theta = M_PI_2 - asin(2.0 / M_PI * (asin_v + v * cos_asin_v));
    *phi = -M_PI_2 * u / fmax(cos_asin_v, 1e-6);
    return TRUE;
}
