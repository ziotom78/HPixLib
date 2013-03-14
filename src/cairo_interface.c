/* cairo_interface.c - Functions to plot map projections into Cairo surfaces
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
#include <assert.h>
#include <string.h>
#include <math.h>

/******************************************************************************/


/* This function creates a Cairo image surface that contains the
 * Mollweide projection of the map. The values `map_min` and `map_max`
 * are used to rescale every value in the map (i.e. to convert every
 * pixel value in the map into a number in [0.0, 1.0]). The value of
 * `bitmap` is the return value of `hpix_bmp_trace_bitmap`. Finally,
 * `width` and `height` give the resolution (in pixel) of the image
 * surface. The Mollweide plot fits into the largest ellipse that can
 * be enclosed in the rectangle (0,0) - (width, height). */
cairo_surface_t *
hpix_bmp_mollweide_proj_to_cairo_surface(const hpix_bmp_projection_t * proj,
					 const hpix_color_palette_t * palette,
					 const hpix_map_t * map,
					 double map_min, double map_max)
{
    const double dynamic_range = map_max - map_min;
    double * map_bitmap;
    double * cur_pixel;
    unsigned char * image_data;
    unsigned int cur_y;
    unsigned int stride;
    cairo_surface_t * surface;
    unsigned int width;
    unsigned int height;

    assert(proj);
    width = hpix_bmp_projection_width(proj);
    height = hpix_bmp_projection_height(proj);

    assert(map);

    map_bitmap = hpix_bmp_to_mollweide_proj(proj, map, NULL, NULL);
    assert(map_bitmap);

    cur_pixel = map_bitmap;

    /* Because of the way Cairo implements surface copies, it is not
     * possible to use CAIRO_FORMAT_ARGB32 here. It would have been
     * really useful, as having an "alpha" (transparency) channel
     * would have helped in implementing the surface copy
     * operation. */
    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
					 width, height);
    image_data = cairo_image_surface_get_data(surface);
    stride = cairo_image_surface_get_stride(surface);

    hpix_color_t color_for_unseen_pixels =
	hpix_color_for_unseen_pixels_in_palette(palette);
    hpix_color_t white_color = hpix_create_color(1.0, 1.0, 1.0);

    for(cur_y = 0; cur_y < height; ++cur_y)
    {
	unsigned int cur_x;
	unsigned char * row = image_data + (height - cur_y - 1) * stride;
	for(cur_x = 0; cur_x < width; ++cur_x)
	{
	    double value = *cur_pixel++;
	    hpix_color_t color;

	    /* Not sure if this works on big-endian machines... */
#define SET_PIXEL(opacity,color) {  \
		int base = cur_x * 4; \
		row[base]     = (int) (255 * color.blue);	\
		row[base + 1] = (int) (255 * color.green);	\
		row[base + 2] = (int) (255 * color.red);	\
		row[base + 3] = (int) (255 * opacity);		\
	    }

	    if(isinf(value))
	    {
		/* Transparent pixel */
		SET_PIXEL(0.0, white_color);
	    }
	    else if (isnan(value) || value < -1.6e+30)
	    {
		SET_PIXEL(1.0, color_for_unseen_pixels);
	    }
	    else
	    {
		double normalized_value = (value - map_min) / dynamic_range;
		color = hpix_get_palette_color(palette, normalized_value);
		SET_PIXEL(1.0, color);
	    }
#undef SET_PIXEL
	}
    }

    hpix_free(map_bitmap);
    return surface;
}

/******************************************************************************/


void
hpix_bmp_configure_linear_gradient(cairo_pattern_t * pattern, 
				   const hpix_color_palette_t * palette)
{
    size_t idx;
    assert(pattern);

    for(idx = 0; idx < hpix_num_of_steps_in_color_palette(palette); ++idx)
    {
	double level = hpix_level_for_step_in_palette(palette, idx);
	hpix_color_t color = hpix_color_for_step_in_palette(palette, idx);

	cairo_pattern_add_color_stop_rgb(pattern, level,
					 color.red, color.green, color.blue);
    }
}
