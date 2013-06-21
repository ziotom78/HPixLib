/* equirectangular_projection.c -- Functions to convert a map into a
 * equirectangular projection and vice-versa
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

#include "config.h"

#include <hpixlib/hpix.h>
#include <math.h>
#include <assert.h>

#include "constants.h"

/**********************************************************************/


_Bool
hpix_equirectangular_is_xy_inside(const hpix_bmp_projection_t * proj,
				  unsigned int x,
				  unsigned int y)
{
    return (x < hpix_bmp_projection_width(proj))
	&& (y < hpix_bmp_projection_height(proj));
}

/**********************************************************************/


_Bool
hpix_equirectangular_xy_to_angles(const hpix_bmp_projection_t * proj,
				  unsigned int x,
				  unsigned int y,
				  double * theta,
				  double * phi)
{
    assert(proj);
    assert(theta);
    assert(phi);

    if (! hpix_equirectangular_is_xy_inside(proj, x, y))
	return FALSE;

    *theta = ((double) y) / hpix_bmp_projection_height(proj) * M_PI;
    *phi = ((double) x) / hpix_bmp_projection_width(proj) * 2.0 * M_PI;
    return TRUE;
}
