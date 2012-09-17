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

#include <hpxlib.h>
#include <math.h>
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

struct ___hpxlib_bmp_projection_t {
    unsigned int       width;
    unsigned int       height;
    hpxlib_coordinates_t coordsys;
};

hpxlib_bmp_projection_t * 
hpxlib_create_bmp_projection(unsigned int width, unsigned int height)
{
    hpxlib_bmp_projection_t * obj;
    obj = malloc(sizeof(hpxlib_bmp_projection_t));
    obj->width = width;
    obj->height = height;

    return obj;
}

void
hpxlib_free_bmp_projection(hpxlib_bmp_projection_t * proj)
{
    if(proj)
	free(proj);
}

unsigned int
hpxlib_bmp_projection_width(const hpxlib_bmp_projection_t * proj)
{
    assert(proj);
    return proj->width;
}

unsigned int
hpxlib_bmp_projection_height(const hpxlib_bmp_projection_t * proj)
{
    assert(proj);
    return proj->height;
}

void
hpxlib_set_bmp_projection_width(hpxlib_bmp_projection_t * proj,
				unsigned int width)
{
    assert(proj);
    proj->width = width;
}

void
hpxlib_set_bmp_projection_height(hpxlib_bmp_projection_t * proj,
				 unsigned int height)
{
    assert(proj);
    proj->height = height;
}

double *
hpxlib_bmp_trace_bitmap(const hpxlib_bmp_projection_t * proj,
			const hpxlib_map_t * map,
			double * min_value,
			double * max_value)
{
    unsigned int x;
    unsigned int y;
    double center_x = proj->width / 2.0;
    double center_y = proj->height / 2.0;
    double *bitmap;
    double *bitmap_ptr;
    hpxlib_angles_to_pixel_fn_t * angles_to_pixel_fn;
    hpxlib_nside_t nside;

    assert(proj);
    assert(map);

    nside = hpxlib_map_nside(map);

    angles_to_pixel_fn = (hpxlib_map_ordering(map) == HPXLIB_ORDER_NEST)
	? hpxlib_angles_to_nest_pixel
	: hpxlib_angles_to_ring_pixel;

    bitmap = hpxlib_malloc(sizeof(bitmap[0]), proj->width * proj->height);
    bitmap_ptr = bitmap;

    for (y = 0; y < proj->height; ++y)
    {
	for (x = 0; x < proj->width; ++x)
	{
	    double u = 2.0 * (x - center_x) / (center_x / 1.02);
	    double v = (y - center_y) / (center_y / 1.02);
	    double theta;
	    double phi;
	    hpxlib_pixel_num_t pixel_idx;

	    if(u*u/4 + v*v >= 1)
	    {
		*bitmap_ptr++ = INFINITY;
		continue;
	    }

	    theta = M_PI_2 - asin(2 / M_PI *
				  (asin(v) +
				   v * sqrt((1 - v) * (1 + v))));
	    phi = -M_PI_2 * u / MAX(sqrt((1 - v) * (1 + v)), 1e-6);
	    pixel_idx = angles_to_pixel_fn(nside, theta, phi);
	    *bitmap_ptr++ = hpxlib_map_pixels(map)[pixel_idx];
	}
    }

    return bitmap;
}
