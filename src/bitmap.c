/* bitmap.c -- Project a map into a 2D bitmap
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

#include "config.h"

#include <hpixlib/hpix.h>
#include <float.h>
#include <math.h>
#include <assert.h>

typedef int inside_test_t (const hpix_bmp_projection_t * proj,
			   unsigned int x,
			   unsigned int y);
typedef int xy_to_angles_t (const hpix_bmp_projection_t * proj,
			    unsigned int x,
			    unsigned int y,
			    double * theta,
			    double * phi);

struct ___hpix_bmp_projection_t {
    unsigned int           width;
    unsigned int           height;
    hpix_coordinates_t     coordsys;

    hpix_projection_type_t type;
    xy_to_angles_t         * xy_to_angles_fn;
    void                   * angle_to_xy_fn;
    inside_test_t          * inside_test_fn;
};

/**********************************************************************/


hpix_bmp_projection_t * 
hpix_create_bmp_projection(unsigned int width, unsigned int height)
{
    hpix_bmp_projection_t * obj;
    obj = malloc(sizeof(hpix_bmp_projection_t));
    obj->width = width;
    obj->height = height;
    obj->xy_to_angles_fn = NULL;
    obj->angle_to_xy_fn = NULL;
    obj->inside_test_fn = NULL;
    obj->type = HPIX_PROJ_NULL;

    return obj;
}

/**********************************************************************/


void
hpix_free_bmp_projection(hpix_bmp_projection_t * proj)
{
    if(proj)
	free(proj);
}

/**********************************************************************/


unsigned int
hpix_bmp_projection_width(const hpix_bmp_projection_t * proj)
{
    assert(proj);
    return proj->width;
}

/**********************************************************************/


unsigned int
hpix_bmp_projection_height(const hpix_bmp_projection_t * proj)
{
    assert(proj);
    return proj->height;
}

/**********************************************************************/


hpix_projection_type_t
hpix_bmp_projection_type(const hpix_bmp_projection_t * proj)
{
    assert(proj);
    return proj->type;
}

/**********************************************************************/


void
hpix_set_equirectangular_projection(hpix_bmp_projection_t * proj)
{
    assert(proj);

    proj->xy_to_angles_fn = hpix_equirectangular_xy_to_angles;
    proj->angle_to_xy_fn = NULL;
    proj->inside_test_fn = hpix_equirectangular_is_xy_inside;
    proj->type = HPIX_PROJ_EQUIRECTANGULAR;
}

/**********************************************************************/


void
hpix_set_mollweide_projection(hpix_bmp_projection_t * proj)
{
    assert(proj);

    proj->xy_to_angles_fn = hpix_mollweide_xy_to_angles;
    proj->angle_to_xy_fn = NULL;
    proj->inside_test_fn = hpix_mollweide_is_xy_inside;
    proj->type = HPIX_PROJ_MOLLWEIDE;
}

/**********************************************************************/

int
hpix_bmp_projection_is_xy_inside(const hpix_bmp_projection_t * proj,
				 unsigned int x,
				 unsigned int y)
{
    assert(proj);
    assert(proj->inside_test_fn);

    return proj->inside_test_fn(proj, x, y);
}

/**********************************************************************/


int
hpix_bmp_projection_xy_to_angles(const hpix_bmp_projection_t * proj,
				 unsigned int x,
				 unsigned int y,
				 double * theta,
				 double * phi)
{
    assert(proj);
    assert(proj->inside_test_fn);

    return proj->xy_to_angles_fn(proj, x, y, theta, phi);
}

/**********************************************************************/


double *
hpix_bmp_projection_trace(const hpix_bmp_projection_t * proj,
			  const hpix_map_t * map,
			  double * min_value,
			  double * max_value)
{
    assert(proj);
    assert(map);

    hpix_nside_t nside = hpix_map_nside(map);

    hpix_angles_to_pixel_fn_t * angles_to_pixel_fn =
	(hpix_map_ordering_scheme(map) == HPIX_ORDER_SCHEME_NEST)
	? hpix_angles_to_nest_pixel
	: hpix_angles_to_ring_pixel;

    size_t num_of_pixels = proj->width * proj->height;
    double *restrict bitmap =
	hpix_malloc(sizeof(bitmap[0]), num_of_pixels);

    double * pixels = hpix_map_pixels(map);

    /* First step: render the bitmap */
#pragma omp parallel for default(shared)
    for (unsigned int y = 0; y < hpix_bmp_projection_height(proj); ++y)
    {
	double * line_ptr = bitmap + y * hpix_bmp_projection_width(proj);

	for (unsigned int x = 0; 
	     x < hpix_bmp_projection_width(proj); 
	     ++x, ++line_ptr)
	{
	    double theta, phi;

	    if(! hpix_bmp_projection_xy_to_angles(proj, x, y, &theta, &phi))
	    {
		*line_ptr = INFINITY; /* Skip the pixel */
		continue;
	    }

	    hpix_pixel_num_t pixel_idx =
		angles_to_pixel_fn(hpix_map_resolution(map), theta, phi);
	    if(pixels[pixel_idx] > -1.6e+30)
		*line_ptr = pixels[pixel_idx];
	    else
		*line_ptr = NAN;
	}
    }

    /* Second step: if the user asked for them, compute the maximum
     * and/or minimum values in the bitmap */
    if(min_value || max_value) 
    {
	if(min_value)
	    *min_value = DBL_MAX;

	if(max_value)
	    *max_value = -DBL_MAX;

	double * bitmap_ptr = bitmap;
	for(size_t idx = 0; idx < num_of_pixels; ++idx, ++bitmap_ptr) 
	{
	    if(isnan(*bitmap_ptr) || isinf(*bitmap_ptr))
		continue;

	    if(min_value && *min_value > *bitmap_ptr)
		*min_value = *bitmap_ptr;
	    if(max_value && *max_value < *bitmap_ptr)
		*max_value = *bitmap_ptr;
	}
    }

    return bitmap;
}
