/* palette.c - Data types and function for color palettes
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
#include <assert.h>
#include <stdlib.h>

/* The following code is used to define the color gradient used to
 * draw the Mollview projection and the color bar. The purpose is to
 * convert a floating-point number into a color. Like Healpix's
 * map2gif and map2tga, we use a combination of linear gradients, i.e.
 * a function that maps a floating-point value between 0.0 and 1.0 and
 * a `color_t` structure. The mapping is implemented by means of a set
 * of linear functions over subsets of the domain [0.0, 1.0] that
 * completely cover the domain and are continuously connected. */

/* Association between a color and a level between 0.0 and 1.0. It is
 * used as a base type for the "hpix_color_palette_t" structure. */ 
typedef struct {
    double level;
    hpix_color_t color;
} color_step_in_palette_t;

/* Linear palette. The elements in "array_of_steps" must be sorted in
 * ascending order on their field "level", otherwise bad things might
 * occur! */
struct hpix_color_palette_t {
    size_t num_of_steps;
    size_t num_of_allocated_entries;
    color_step_in_palette_t * array_of_steps;

    hpix_color_t color_for_unseen_pixels;
};

/**********************************************************************/

hpix_color_t
hpix_create_color(double red, double green, double blue)
{
    return (hpix_color_t) { .red = red, .green = green, .blue = blue };
}

/**********************************************************************/

double
hpix_red_level_from_color(const hpix_color_t * color)
{
    return color->red;
}

/**********************************************************************/

double
hpix_green_level_from_color(const hpix_color_t * color)
{
    return color->green;
}

/**********************************************************************/

double
hpix_blue_level_from_color(const hpix_color_t * color)
{
    return color->blue;
}

/**********************************************************************/

/* Create a black palette. This
 * function is typically used to fill the levels with something less
 * boring that black :-). */

hpix_color_palette_t *
hpix_create_black_color_palette(void)
{
    hpix_color_palette_t * result = hpix_malloc(1, sizeof(hpix_color_palette_t));

    result->num_of_steps = 2;
    result->num_of_allocated_entries = 2;
    result->array_of_steps = hpix_calloc(2, sizeof(color_step_in_palette_t));
    result->color_for_unseen_pixels = hpix_create_color(0.5, 0.5, 0.5);

    hpix_color_t black_color = hpix_create_color(0.0, 0.0, 0.0);
    result->array_of_steps[0] = 
	(color_step_in_palette_t) { .level = 0.0, .color = black_color };
    result->array_of_steps[1] = 
	(color_step_in_palette_t) { .level = 1.0, .color = black_color };

    return result;
}

/**********************************************************************/

/* Create a grayscale palette. */

hpix_color_palette_t *
hpix_create_grayscale_color_palette(void)
{
    hpix_color_palette_t * palette = hpix_create_black_color_palette();

    /* First fix the extrema of the range */
    hpix_set_color_for_step_in_palette(palette, 0,
				       hpix_create_color(0.0, 0.0, 0.0));
    hpix_set_color_for_step_in_palette(palette, 1,
				       hpix_create_color(1.0, 1.0, 1.0));
    
    /* The color for unseen pixels must be distinguishable from the
     * others, so we give it a faint tint of red... */
    hpix_set_color_for_unseen_pixels_in_palette(palette,
						hpix_create_color(0.7, 0.5, 0.5));

    return palette;
}

/**********************************************************************/

/* Create a palette similar to the one used by Healpix' tools like
 * map2gif. */

hpix_color_palette_t *
hpix_create_healpix_color_palette(void)
{
    hpix_color_palette_t * palette = hpix_create_black_color_palette();

    /* First fix the extrema of the range */
    hpix_set_color_for_step_in_palette(palette, 0,
				       hpix_create_color(0.0, 0.0, 0.5));
    hpix_set_color_for_step_in_palette(palette, 1,
				       hpix_create_color(0.5, 0.0, 0.0));
    
    /* Now fill the intermediate levels */
    hpix_add_step_to_color_palette(palette, 0.15,
				   hpix_create_color(0.0, 0.0, 1.0));
    hpix_add_step_to_color_palette(palette, 0.40,
				   hpix_create_color(0.0, 1.0, 1.0));
    hpix_add_step_to_color_palette(palette, 0.70,
				   hpix_create_color(1.0, 1.0, 0.0));
    hpix_add_step_to_color_palette(palette, 0.90,
				   hpix_create_color(1.0, 0.33, 0.0));

    /* Finally, sort everything */
    hpix_sort_levels_in_color_palette(palette);

    return palette;
}

/**********************************************************************/

/* Create a palette similar to the one used in the first Planck data
 * release (2013). */

hpix_color_palette_t *
hpix_create_planck_color_palette(void)
{
    hpix_color_palette_t * palette = hpix_create_black_color_palette();

    /* First fix the extrema of the range */
    hpix_set_color_for_step_in_palette(palette, 0,
				       hpix_create_color(0.0, 0.0, 1.0));
    hpix_set_color_for_step_in_palette(palette, 1,
				       hpix_create_color(0.4, 0.0, 0.0));
    
    /* Now fill the intermediate levels */
    hpix_add_step_to_color_palette(palette, 0.33,
				   hpix_create_color(0.0, 0.87, 1.0));
    hpix_add_step_to_color_palette(palette, 0.50,
				   hpix_create_color(1.0, 0.93, 0.85));
    hpix_add_step_to_color_palette(palette, 0.67,
				   hpix_create_color(1.0, 0.7, 0.0));
    hpix_add_step_to_color_palette(palette, 0.83,
				   hpix_create_color(1.0, 0.3, 0.0));

    /* Finally, sort everything */
    hpix_sort_levels_in_color_palette(palette);

    return palette;
}

/**********************************************************************/


void
hpix_free_color_palette(hpix_color_palette_t * palette)
{
    assert(palette != NULL);
    assert(palette->array_of_steps != NULL);

    hpix_free(palette->array_of_steps);
    hpix_free(palette);
}

/**********************************************************************/

void
hpix_set_color_for_unseen_pixels_in_palette(hpix_color_palette_t * palette,
					    hpix_color_t new_color)
{
    assert(palette != NULL);

    palette->color_for_unseen_pixels = new_color;
}

/**********************************************************************/

hpix_color_t
hpix_color_for_unseen_pixels_in_palette(const hpix_color_palette_t * palette)
{
    assert(palette != NULL);

    return palette->color_for_unseen_pixels;
}

/**********************************************************************/

/* Add a new color step to a palette */
void
hpix_add_step_to_color_palette(hpix_color_palette_t * palette,
			       double level, hpix_color_t color)
{
    assert(palette != NULL);
    assert(level >= 0.0 && level <= 1.0);

    if(palette->num_of_allocated_entries <= palette->num_of_steps)
    {
	palette->num_of_allocated_entries = palette->num_of_steps + 1;
	palette->array_of_steps = hpix_realloc(palette->array_of_steps,
					       palette->num_of_allocated_entries *
					       sizeof(color_step_in_palette_t));
    }

    palette->array_of_steps[palette->num_of_steps] =
	(color_step_in_palette_t) { .level = level, .color = color };

    palette->num_of_steps++;
}

/**********************************************************************/

size_t
hpix_num_of_steps_in_color_palette(const hpix_color_palette_t * palette)
{
    assert(palette != NULL);
    return palette->num_of_steps;
}

/**********************************************************************/

hpix_color_t
hpix_color_for_step_in_palette(const hpix_color_palette_t * palette,
			       size_t zero_based_index)
{
    assert(palette != NULL);
    assert(zero_based_index >= 0);
    assert(zero_based_index < palette->num_of_steps);

    return palette->array_of_steps[zero_based_index].color;
}

/**********************************************************************/

double
hpix_level_for_step_in_palette(const hpix_color_palette_t * palette,
			       size_t zero_based_index)
{
    assert(palette != NULL);
    assert(zero_based_index >= 0);
    assert(zero_based_index < palette->num_of_steps);

    return palette->array_of_steps[zero_based_index].level;
}

/**********************************************************************/

void
hpix_set_color_for_step_in_palette(hpix_color_palette_t * palette,
				   size_t zero_based_index,
				   hpix_color_t new_color)
{
    assert(palette != NULL);
    assert(zero_based_index >= 0);
    assert(zero_based_index < palette->num_of_steps);

    palette->array_of_steps[zero_based_index].color = new_color;
}

/**********************************************************************/

void
hpix_set_level_for_step_in_palette(hpix_color_palette_t * palette,
				   size_t zero_based_index,
				   double new_level)
{
    assert(palette != NULL);
    assert(zero_based_index >= 0);
    assert(zero_based_index < palette->num_of_steps);

    palette->array_of_steps[zero_based_index].level = new_level;
}

/**********************************************************************/

static int
compare_levels(const void * a, const void * b)
{
    const color_step_in_palette_t * step_a = (const color_step_in_palette_t *) a;
    const color_step_in_palette_t * step_b = (const color_step_in_palette_t *) b;

    return (step_a->level > step_b->level) - (step_a->level < step_b->level);
}

void
hpix_sort_levels_in_color_palette(hpix_color_palette_t * palette)
{
    assert(palette != NULL);
    assert(palette->num_of_steps > 0);
    assert(palette->array_of_steps != NULL);

    qsort(palette->array_of_steps, palette->num_of_steps,
	  sizeof(color_step_in_palette_t), compare_levels);
}

/**********************************************************************/

/* This function provides the mapping between [0.0, 1.0] and a
 * `color_t` structure. It is a faithful copy of a function found in
 * Heapix's map2tga program. */

hpix_color_t
hpix_get_palette_color(const hpix_color_palette_t * palette,
		       double level)
{
    assert(palette != NULL);

    size_t num_of_levels = hpix_num_of_steps_in_color_palette(palette);
    assert(num_of_levels > 0);

    /* Clip values outside the boundaries */

    if(level <= 0.0)
	return hpix_color_for_step_in_palette(palette, 0);

    if(level >= 1.0)
	return hpix_color_for_step_in_palette(palette, num_of_levels - 1);

    /* Look for the applicable subset of [0.0, 1.0] */
    size_t idx = 0;
    while(level > hpix_level_for_step_in_palette(palette, idx))
	++idx;

    hpix_color_t color0 = hpix_color_for_step_in_palette(palette, idx - 1);
    hpix_color_t color1 = hpix_color_for_step_in_palette(palette, idx);

    double level0 = hpix_level_for_step_in_palette(palette, idx - 1);
    double level1 = hpix_level_for_step_in_palette(palette, idx);

    /* Perform a linear interpolation for each of the three color
     * components */
#define INTERPOLATE_COMPONENT(level, comp_function) \
    (  comp_function(&color0) * (level1 - level) / (level1 - level0) \
     + comp_function(&color1) * (level - level0) / (level1 - level0))

    return hpix_create_color(INTERPOLATE_COMPONENT(level, hpix_red_level_from_color),
			     INTERPOLATE_COMPONENT(level, hpix_green_level_from_color),
			     INTERPOLATE_COMPONENT(level, hpix_blue_level_from_color));

#undef INTERPOLATE_COMPONENT
}
