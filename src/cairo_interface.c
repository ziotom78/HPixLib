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

/* The following code is used to define the color gradient used to
 * draw the Mollview projection and the color bar. The purpose is to
 * convert a floating-point number into a color. Like Healpix's
 * map2gif and map2tga, we use a combination of linear gradients, i.e.
 * a function that maps a floating-point value between 0.0 and 1.0 and
 * a `color_t` structure. The mapping is implemented by means of a set
 * of linear functions over subsets of the domain [0.0, 1.0] that
 * completely cover the domain and are continuously connected. */

/* The most basic structure: a RGB color. Following Cairo's
 * conventions, each component is a floating-point number between 0.0
 * and 1.0. */
typedef struct {
    double red;
    double green;
    double blue;
} color_t;

/* These numbers split the [0.0, 1.0] domain in a number of subsets. */
static const double levels[] = { 0.0, 0.15, 0.40, 0.70, 0.90, 1.00 };
/* These are the colors of the mapping at the boundaries specified by
 * the `levels` variable above. */
static const color_t colors[] = {
    { 0.0, 0.0, 0.5 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 1.0, 1.0 },
    { 1.0, 1.0, 0.0 },
    { 1.0, 0.33, 0.0 },
    { 0.5, 0.0, 0.0 }};
/* Number of steps, i.e. of subsets of [0.0, 1.0] */
static const size_t num_of_levels = sizeof(levels) / sizeof(levels[0]);

/* This function provides the mapping between [0.0, 1.0] and a
 * `color_t` structure. It is a faithful copy of a function found in
 * Heapix's map2tga program. */
void
get_palette_color(double level, color_t * color_ptr)
{
    size_t idx;
    size_t index0, index1;

    /* Clip values outside the boundaries */

    if(level <= 0.0)
    {
	memcpy(color_ptr, (const void *) &colors[0],
	       sizeof(color_t));
	return;
    }

    if(level >= 1.0)
    {
	memcpy(color_ptr, (const void *) &colors[num_of_levels - 1],
	       sizeof(color_t));
	return;
    }

    /* Look for the applicable subset of [0.0, 1.0] */
    idx = 0;
    while(level > levels[idx])
	++idx;

    index1 = idx;
    index0 = index1 - 1;

    /* Perform a linear interpolation for each of the three color
     * components */
#define INTERPOLATE_COMPONENT(level, comp_name) \
    (  colors[index0].comp_name * (levels[index1] - level) / (levels[index1] - levels[index0]) \
     + colors[index1].comp_name * (level - levels[index0]) / (levels[index1] - levels[index0]))

    color_ptr->red   = INTERPOLATE_COMPONENT(level, red);
    color_ptr->green = INTERPOLATE_COMPONENT(level, green);
    color_ptr->blue  = INTERPOLATE_COMPONENT(level, blue);

#undef INTERPOLATE_COMPONENT
}

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

    for(cur_y = 0; cur_y < height; ++cur_y)
    {
	unsigned int cur_x;
	unsigned char * row = image_data + (height - cur_y - 1) * stride;
	for(cur_x = 0; cur_x < width; ++cur_x)
	{
	    double value = *cur_pixel++;
	    color_t color;

	    /* Not sure if this works on big-endian machines... */
#define SET_PIXEL(a,r,g,b) {	      \
		int base = cur_x * 4; \
		row[base]     = b;    \
		row[base + 1] = g;    \
		row[base + 2] = r;    \
		row[base + 3] = a;    \
	    }

	    if(isinf(value))
	    {
		/* Transparent pixel */
		SET_PIXEL(0, 255, 255, 255);
	    }
	    else if (isnan(value) || value < -1.6e+30)
	    {
		/* Opaque pixel with a gray shade */
		SET_PIXEL(255, 128, 128, 128);
	    }
	    else
	    {
		double normalized_value = (value - map_min) / dynamic_range;
		get_palette_color(normalized_value, &color);
		SET_PIXEL(255, /* This makes the pixel fully opaque */
			  (int) (255 * color.red),
			  (int) (255 * color.green),
			  (int) (255 * color.blue));
	    }
#undef SET_PIXEL
	}
    }

    hpix_free(map_bitmap);
    return surface;
}

/******************************************************************************/


void
hpix_bmp_configure_linear_gradient(cairo_pattern_t * pattern)
{
    size_t idx;
    assert(pattern);

    for(idx = 0; idx < num_of_levels; ++idx)
    {
	cairo_pattern_add_color_stop_rgb(pattern,
					 levels[idx],
					 colors[idx].red,
					 colors[idx].green,
					 colors[idx].blue);
    }
}
