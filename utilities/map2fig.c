/* Copyright 2011-2012 Maurizio Tomasi. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MAURIZIO TOMASI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MAURIZIO TOMASI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and
 * documentation are those of the authors and should not be interpreted
 * as representing official policies, either expressed or implied, of
 * Maurizio Tomasi. */

#include <hpixlib/hpix.h>
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>

#include "gopt.h"

/* Used to print information/error/warning messages */
#define MSG_HEADER   "map2fig: "

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

/* Available file formats */
typedef enum { FMT_NULL, FMT_PNG, FMT_PS, FMT_EPS, FMT_PDF, FMT_SVG }
    output_format_code_t;

/* This structure holds information about one file format. It is only
 * used to parse the `--format` command-line flag and to implement the
 * `--list-formats` command. */
typedef struct {
    /* Short name, used to parse the keyword after `--format` */
    const char * name;
    /* Description, used by `--list-formats` */
    const char * description;
    /* Code, used to initialize the global variable `output_format` */
    output_format_code_t code;
} output_format_t;

/* Here is the list of the actual formats supported by the program.
 * The entries to be put in the list are decided at compilation time,
 * depending on the capabilities of the Cairo library found on the
 * system. */
output_format_t list_of_output_formats[] = {
    /* The Cairo documentation guarantees that PNG is always
     * available */
    { "png", "PNG 24-bit bitmap", FMT_PNG },
#if CAIRO_HAS_PS_SURFACE
    { "ps", "PostScript", FMT_PS },
    { "eps", "Encapsulated PostScript", FMT_EPS },
#endif
#if CAIRO_HAS_PDF_SURFACE
    { "pdf", "Adobe Portable Document Format", FMT_PDF },
#endif
#if CAIRO_HAS_SVG_SURFACE
    { "svg", "Scalable Vector Graphics", FMT_SVG },
#endif
    { NULL, NULL, FMT_NULL }
};

/* Output format to use, see above */
output_format_code_t output_format = FMT_PNG;

/* Should we skip painting the image background? Set by `--no-background` */
int no_background_flag = 0;

/* Should we draw a color bar? Set by `-b`, `--draw-color-bar` */
int draw_color_bar_flag = 0;

/* Should we produce a number of diagnostic messages? Set by `--verbose` */
int verbose_flag = 0;

/* Shall we remove the average from the map? Set by `--remove-monopole' */
int remove_monopole = 0;

/* String to append to the measure unit, set by `-m`, `--measure-unit` */
const char * measure_unit_str = "";

/* Scale to use with the maps, set by `-s`, `--scale` */
double scale_factor = 1.0;

/* Title to be drawn above the map, set by `-t`, `--title` */
const char * title_str = "";

/* Name of the output file name, set by `-o`, `--output` */
const char * output_file_name = NULL;

/* C-like format string for numbers, currently unused */
const char * number_format = "%g";

const char * input_file_name = NULL;

/* Number of the column to display, set by `-c`, `--column` */
unsigned short column_number = 1;

/* Relative height of the title and of the color bar. Together with
 * the height of the map, their sum is 1.0. */
float title_height_fraction = 0.1;
float colorbar_height_fraction = 0.1;

/* Extrema of the color bar, optinally set by `--min` and `--max`. If
 * the user does not specify them, they will be initialized to NAN. In
 * this case, the code will use the extrema of the map. */
double min_value;
double max_value;

/* Size of the image. Depending on the output format, these number can
 * be pixels (PNG) or points (1/72 inch, used by PS, PDF, SVG).
 * Therefore, they are set once the format has been decided. */
double image_width;
double image_height;

/* Number of pixels in the bitmapped representation of the map. Unlike
 * `image_width` and `image_height`, these numbers are always
 * expressed in pixel units. They provide the resolution of the bitmap
 * file containing the Mollview projection which is to be included in
 * e.g. the SVG file. */
double bitmap_columns = 600;
double bitmap_rows = 400;

/* This structure represents a rectangle on the Cairo surface. It is
 * used to lay out things on the image. */
typedef struct {
    double x, y;
    double width, height;
} rect_t;

typedef enum { PAL_NULL, PAL_HEALPIX, PAL_GRAYSCALE, PAL_PLANCK }
    palette_type_code_t;

/* This structure holds information about one palette. It is only used
 * to parse the `--palette` command-line flag and to implement the
 * `--list-palettes` command. */
typedef struct {
    /* Short name, used to parse the keyword after `--palette` */
    const char * name;
    /* Description, used by `--list-palettes` */
    const char * description;
    /* Code, used to initialize the global variable `palette_type_code` */
    palette_type_code_t code;
} palette_type_t;

/* Here is the list of the palettes supported by the program. */
palette_type_t list_of_palette_types[] = {
    { "healpix", "Palette used in the original Healpix implementation", PAL_HEALPIX },
    { "parchment", "Palette used in the first Planck data release", PAL_PLANCK },
    { "grayscale", "Grayscale palette", PAL_GRAYSCALE },
    { NULL, NULL, PAL_NULL }
};

palette_type_code_t palette_type_code = PAL_HEALPIX;

/******************************************************************************/


void
print_usage(const char * program_name)
{
    printf("Usage: %s [OPTIONS] INPUT_MAP\n\n", program_name);
    puts("OPTIONS can be one or more of the following:");
    puts("  -b, --draw-color-bar      Draw a color bar");
    puts("  --no-background           Do not paint the background");
    puts("  -c, --column=NUM          Number of the column to display");
    puts("  -f, --format=STRING       Format of the output image file");
    puts("  --list-formats            Print a list of the file formats that");
    puts("                            can be specified with --format");
    puts("  -m, --measure-unit=STRING Measure unit to use.");
    puts("  --min=VALUE, --max=VALUE  Minimum and maximum value to be used");
    puts("                            at the extrema of the color bar");
    puts("  -o, --output=FILE         Save the image to the specified file");
    puts("  --palette=NAME            Color palette to use");
    puts("  --list-palettes           Print a list of the palettes that can");
    puts("                            be specified with --palette");
    puts("  -s, --scale=NUM           Multiply the pixel values by NUM");
    puts("                            (useful e.g. to convert K into mK)");
    puts("  -t, --title=TITLE         Title to be written");
    puts("  --tick-font-size=NUM      Height of the tick labels in the");
    puts("                            color bar (in pixels if exporting to");
    puts("                            PNG, in dots otherwise)");
    puts("  --title-font-size=NUM     Height of the title (in pixels if");
    puts("                            exporting to PNG, in dots otherwise)");
    puts("  --remove-monopole         Remove the monopole from the map");
    puts("  --xy-aspect-ratio=NUM     Width/height ratio of the image size");
    puts("  -v, --version             Print version number and exit");
    puts("  -h, --help                Print this help");
}

/******************************************************************************/


void
print_list_of_available_formats(void)
{
    int idx;
    for(idx = 0; list_of_output_formats[idx].name != NULL; ++idx)
    {
	output_format_t * format = &list_of_output_formats[idx];
	assert(format != NULL);
	printf("%s\t%s\n", format->name, format->description);
    }
}

/******************************************************************************/


void
print_list_of_palette_types(void)
{
    int idx;
    for(idx = 0; list_of_palette_types[idx].name != NULL; ++idx)
    {
	palette_type_t * type = &list_of_palette_types[idx];
	assert(type != NULL);
	printf("%s\t%s\n", type->name, type->description);
    }
}

/******************************************************************************/


void
parse_format_specification(const char * format_str)
{
    int idx;
    for(idx = 0; list_of_output_formats[idx].name != NULL; ++idx)
    {
	output_format_t * format = &list_of_output_formats[idx];
	assert(format != NULL);
	if(strcmp(format->name, format_str) == 0)
	{
	    output_format = format->code;
	    return;
	}
    }

    fprintf(stderr,
	    MSG_HEADER "unknown format `%s', get a list of the available\n"
	    MSG_HEADER "formats using `--list-formats'\n",
	    format_str);
}

/******************************************************************************/


void
parse_palette_specification(const char * type_str)
{
    int idx;
    for(idx = 0; list_of_palette_types[idx].name != NULL; ++idx)
    {
	palette_type_t * type = &list_of_palette_types[idx];
	assert(type != NULL);
	if(strcmp(type->name, type_str) == 0)
	{
	    palette_type_code = type->code;
	    return;
	}
    }

    fprintf(stderr,
	    MSG_HEADER "unknown palette `%s', get a list of the available\n"
	    MSG_HEADER "palettes using `--list-palettes'\n",
	    type_str);
}

/******************************************************************************/


hpix_color_palette_t *
create_palette(void)
{
    switch(palette_type_code)
    {
    case PAL_HEALPIX:   return hpix_create_healpix_color_palette();
    case PAL_GRAYSCALE: return hpix_create_grayscale_color_palette();
    case PAL_PLANCK:    return hpix_create_planck_color_palette();
    case PAL_NULL:
    default:
	abort();
    }
}

/******************************************************************************/


double
parse_double_from_options(void * list_of_options,
			  char option_code,
			  double default_value,
			  const char * command_line_switch)
{
    const char * value_str;
    double value = default_value;

    if(gopt_arg(list_of_options, option_code, &value_str))
    {
	char * tail_ptr = NULL;
	value = strtod(value_str, &tail_ptr);
	if(! tail_ptr ||
	   *tail_ptr != '\x0')
	{
	    fprintf(stderr,
		    MSG_HEADER "invalid minimum '%s' specified with %s\n",
		    value_str,
		    command_line_switch);
	    exit(EXIT_FAILURE);
	}
    }

    return value;
}

/******************************************************************************/


/* This code uses the `gopt` library to parse the command-line
 * options. It initializes a number of global variables declared at
 * the beginning of this file. */
void
parse_command_line(int argc, const char ** argv)
{
    const char * value_str;
    char * tail_ptr;
    double tick_font_size = NAN;
    double title_font_size = NAN;

    void * options =
	gopt_sort(&argc, argv,
		  gopt_start(
		      gopt_option('h', 0, gopt_shorts('h', '?'), gopt_longs("help")),
		      gopt_option('v', 0, gopt_shorts('v'), gopt_longs("version")),
		      gopt_option('V', 0, gopt_shorts(0), gopt_longs("verbose")),
		      gopt_option('b', 0, gopt_shorts('b'), gopt_longs("draw-color-bar")),
		      gopt_option('M', 0, gopt_shorts(0), gopt_longs("remove-monopole")),
		      gopt_option('B', 0, gopt_shorts(0), gopt_longs("no-background")),
		      gopt_option('c', 0, gopt_shorts('c'), gopt_longs("column")),	
		      gopt_option('F', 0, gopt_shorts(0), gopt_longs("list-formats")),
		      gopt_option('P', 0, gopt_shorts(0), gopt_longs("list-palettes")),
		      gopt_option('m', GOPT_ARG, gopt_shorts('m'), gopt_longs("measure-unit")),
		      gopt_option('o', GOPT_ARG, gopt_shorts('o'), gopt_longs("output")),
		      gopt_option('a', GOPT_ARG, gopt_shorts(0), gopt_longs("xy-aspect-ratio")),
		      gopt_option('_', GOPT_ARG, gopt_shorts(0), gopt_longs("min")),
		      gopt_option('^', GOPT_ARG, gopt_shorts(0), gopt_longs("max")),
		      gopt_option('s', GOPT_ARG, gopt_shorts('s'), gopt_longs("scale")),
		      gopt_option('t', GOPT_ARG, gopt_shorts('t'), gopt_longs("title")),
		      gopt_option('1', GOPT_ARG, gopt_shorts(0), gopt_longs("tick-font-size")),
		      gopt_option('2', GOPT_ARG, gopt_shorts(0), gopt_longs("title-font-size")),
		      gopt_option('f', GOPT_ARG, gopt_shorts('f'), gopt_longs("format")),
		      gopt_option('p', GOPT_ARG, gopt_shorts('p'), gopt_longs("palette"))));

    /* --help */
    if(gopt(options, 'h'))
    {
	print_usage("map2fig");
	exit(EXIT_SUCCESS);
    }

    /* --version */
    if(gopt(options, 'v'))
    {
	puts("map2fig version " VERSION " - Copyright(c) 2011-2012 Maurizio Tomasi");
	exit(EXIT_SUCCESS);
    }

    /* --no-background */
    if(gopt(options, 'B'))
	no_background_flag = 1;

    /* --list-formats */
    if(gopt(options, 'F'))
    {
	print_list_of_available_formats();
	exit(EXIT_SUCCESS);
    }

    /* --list-palettes */
    if(gopt(options, 'P'))
    {
	print_list_of_palette_types();
	exit(EXIT_SUCCESS);
    }

    /* --verbose */
    if(gopt(options, 'V'))
	verbose_flag = 1;

    /* --remove-monopole */
    if(gopt(options, 'M'))
	remove_monopole = 1;

    if(gopt(options, 'b'))
	draw_color_bar_flag = 1;

    /* --column NUM */
    if(gopt_arg(options, 'c', &value_str))
    {
	tail_ptr = NULL;
	column_number = strtoul(value_str, &tail_ptr, 10);
	if(! tail_ptr ||
	   *tail_ptr != '\x0' ||
	   column_number == 0 ||
	   column_number > USHRT_MAX)
	{
	    fprintf(stderr, MSG_HEADER "invalid column number '%s'\n",
		    value_str);
	    exit(EXIT_FAILURE);
	}
    }

    /* --format STRING */
    if(gopt_arg(options, 'f', &value_str))
	parse_format_specification(value_str);

    /* --format STRING */
    if(gopt_arg(options, 'p', &value_str))
	parse_palette_specification(value_str);

    /* Here we parse all those options of the form --name VALUE, with
     * VALUE being a floating-point number. */
    min_value = parse_double_from_options(options, '_', 
					  /* default_value = */ NAN,
					  "--min");
    max_value = parse_double_from_options(options, '^',
					  /* default_value = */ NAN,
					  "--max");
    scale_factor = 
	parse_double_from_options(options, 's', 
				  /* default_value = */ 1.0,
				  "--scale");
    double xy_aspect_ratio = 
	parse_double_from_options(options, 'a', 
				  /* default_value = */ 1.5,
				  "--xy-aspect-ratio");
    tick_font_size = 
	parse_double_from_options(options, '1',
				  /* default = */ tick_font_size,
				  "--tick-font-size");
    title_font_size = 
	parse_double_from_options(options, '2',
				  /* default = */ title_font_size,
				  "--title-font-size");

    /* --measure-unit STRING */
    gopt_arg(options, 'm', &measure_unit_str);
    /* --title STRING */
    gopt_arg(options, 't', &title_str);
    /* --output FILE */
    gopt_arg(options, 'o', &output_file_name);

    gopt_free(options);

    /* NOTE: in this version, there must be only input parameter! This
     * is the name of the FITS file containing the map to draw. */

    if(argc > 2)
    {
	fputs(MSG_HEADER "too many command-line arguments (hint: use --help)\n",
	      stderr);
	exit(EXIT_FAILURE);
    }

    if(argc < 2)
    {
	fprintf(stderr,
		MSG_HEADER "reading maps from stdin is not supported yet\n"
		MSG_HEADER "(hint: specify the name of a FITS file to be read)\n");
	exit(EXIT_FAILURE);
    }

    switch(output_format)
    {
    case FMT_PNG:
	/* Pixels */
	image_width = 750;
	break;

    case FMT_PS:
    case FMT_EPS:
    case FMT_PDF:
    case FMT_SVG:
	/* Points, that is, 1/72 inches */
	image_width = 7.5 * 72;
	break;

    default:
	assert(0);
    }

    image_height = image_width / xy_aspect_ratio;

    if(! isnan(title_font_size))
    {
	if(title_font_size <= 0.0)
	{
	    fputs(MSG_HEADER "the size of the title font must be positive\n",
		  stderr);
	    exit(EXIT_FAILURE);
	}
	title_height_fraction = title_font_size / image_height * 1.05;
    }

    if(! isnan(tick_font_size))
    {
	if(title_font_size <= 0.0)
	{
	    fputs(MSG_HEADER "the size of the font of the colorbar labels\n"
		  MSG_HEADER "must be positive\n",
		  stderr);
	    exit(EXIT_FAILURE);
	}
	colorbar_height_fraction = tick_font_size / image_height * 2.10;
    }

    if(title_height_fraction + colorbar_height_fraction >= 0.9)
    {
	fprintf(stderr,
		MSG_HEADER "too large title/tick font sizes\n"
		MSG_HEADER "try to enlarge the image using --geometry");
	exit(EXIT_FAILURE);
    }

    input_file_name = argv[1];
}

/******************************************************************************/


hpix_map_t *
load_map_and_rescale_if_needed(void)
{
    hpix_map_t * result;
    int status = 0;

    if(! hpix_load_fits_component_from_file(input_file_name,
					    column_number,
					    &result, &status))
    {
	fprintf(stderr, MSG_HEADER "unable to load file '%s'\n",
		input_file_name);
	exit(EXIT_FAILURE);
    }

    /* Remove the monopole */
    if(remove_monopole)
	hpix_remove_monopole_from_map_inplace(result);

    hpix_scale_pixels_by_constant_inplace(result, scale_factor);

    return result;
}

/******************************************************************************/


/* Find the minimum and maximum value in a map, properly skipping
 * UNSEEN pixels. This is different from the extrema returned by
 * hpix_bmp_trace_bitmap, as the latter returns the extrema of the
 * pixels used to draw the map (which are in general a subset of all
 * the pixels in the map). */
void
find_map_extrema(const hpix_map_t * map, double * min, double * max)
{
    const double * cur_pixel = hpix_map_pixels(map);
    size_t num_of_pixels = hpix_map_num_of_pixels(map);
    size_t counter;
    int are_minmax_initialized = 0;

    assert(map != NULL);
    assert(min != NULL);
    assert(max != NULL);

    for(counter = 0; counter < num_of_pixels; ++counter, ++cur_pixel)
    {
	if(HPIX_IS_MASKED(*cur_pixel))
	    continue;

	if (! are_minmax_initialized)
	{
	    *min = *max = *cur_pixel;
	    are_minmax_initialized = 1;
	}
	else
	{
	    if(*min > *cur_pixel)
		*min = *cur_pixel;
	    else if (*max < *cur_pixel)
		*max = *cur_pixel;
	}
    }
}

/******************************************************************************/


typedef enum {
    HALIGN_RIGHT,
    HALIGN_LEFT,
    HALIGN_CENTER
} halign_t;

typedef enum {
    VALIGN_TOP,
    VALIGN_BOTTOM,
    VALIGN_CENTER
} valign_t;

void
draw_aligned_text(cairo_t * context, const char * label,
		  double x, double y,
		  halign_t h_align, valign_t v_align)
{
    cairo_text_extents_t te;
    double text_pos_x = x, text_pos_y = y;

    cairo_text_extents (context, label, &te);

    switch(h_align)
    {
    case HALIGN_LEFT:
	text_pos_x -= te.width;
	break;
    case HALIGN_CENTER:
	text_pos_x -= te.width * 0.5;
	break;
    }

    switch(v_align)
    {
    case VALIGN_BOTTOM:
	text_pos_y += te.height;
	break;
    case VALIGN_CENTER:
	text_pos_y += te.height * 0.5;
	break;
    case VALIGN_TOP:
	break;
    }

    cairo_move_to(context, text_pos_x, text_pos_y);
    cairo_show_text(context, label);
}

/******************************************************************************/


void
paint_title(cairo_t * context, const rect_t * region)
{
    const double title_font_size = region->height * 0.9;

    cairo_set_font_size(context, title_font_size);
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    draw_aligned_text(context, title_str,
		      region->x + 0.5 * region->width,
		      region->y + 0.5 * region->height,
		      HALIGN_CENTER, VALIGN_CENTER);
}

/******************************************************************************/



double
pow_of_ten(int n)
{
    int index;
    double result = 1.0;

    if(n > 0)
    {
	for(index = 0; index < n; ++index)
	    result *= 10.0;
    } else if(n < 0)
    {
	for(index = 0; index < -n; ++index)
	    result /= 10.0;
    }

    return result;
}

/******************************************************************************/


/* Given some floating-point number NUM, return a "nice" number which
 * is roughly equal to NUM. This is used to paint the ticks in the
 * color bar. */
double
nice_number(double num, int round_flag)
{
    int sign = num >= 0.0 ? 1.0 : -1.0;
    int exponent = (int) floor(log10(sign * num));
    double fraction = sign * num / pow_of_ten(exponent); /* Always between 1 and 10 */
    double nice_fraction;
    double result;

    if(round_flag)
    {
	if(fraction < 1.5)
	    nice_fraction = 1.0;
	else if(fraction < 3.0)
	    nice_fraction = 2.0;
	else if(fraction < 7.0)
	    nice_fraction = 5.0;
	else
	    nice_fraction = 10.0;
    } else {
	if(fraction <= 1.0)
	    nice_fraction = 1.0;
	else if(fraction <= 2.0)
	    nice_fraction = 2.0;
	else if(fraction <= 5.0)
	    nice_fraction = 5.0;
	else
	    nice_fraction = 10.0;
    }

    result = sign * nice_fraction * pow_of_ten(exponent);

    /* Prevent slight roundoff errors */
    if(fabs(result / pow_of_ten(exponent)) < 1e-6)
	return 0.0;
    else
	return result;
}

/******************************************************************************/


/* Draw the Mollweide projection of the map on the rectangle specified
 * by `map_rect' on the Cairo surface specified by `context'. The
 * values of `min' and `max' specify the minimum and maximum values to
 * be used in plotting the map, and are used to set the color scale.
 * This function is a nice wrapper around
 * `plot_bitmap_to_cairo_surface', which is more low-level because (1)
 * it fills the whole Cairo surface, and (2) it fills the whole
 * rectangular surface, instead of just an ellipse. (The latter seems
 * to be due to a bug in Cairo.) */
void
paint_map(cairo_t * context, const rect_t * map_rect,
	  const hpix_color_palette_t * palette,
	  const hpix_map_t * map, double min, double max)
{
    hpix_bmp_projection_t * projection;
    cairo_surface_t * map_surface;
    const double reduce_factor = 1.02;

    /* First produce a cairo image surface with the map in it */
    projection = hpix_create_bmp_projection((int) (bitmap_columns + .5),
					    (int) (bitmap_rows + .5));
    map_surface =
	hpix_bmp_mollweide_proj_to_cairo_surface(projection, palette,
						 map, min, max);

    /* Now copy the cairo surface into the surface we're currently
     * using to draw the figure */
    cairo_save(context);
    
    /* These transformations are useful for the map containing the
     * bitmap (i.e. the source in the copy operation). */
    cairo_translate(context, 0.0, map_rect->y);
    cairo_scale(context,
		image_width / cairo_image_surface_get_width(map_surface),
		map_rect->height  / cairo_image_surface_get_height(map_surface));
    cairo_set_source_surface(context, map_surface,
			     0.0, 0.0);
    
    /* Now we need two more transformations in order to draw an
     * ellipse out of `cairo_arc'. */
    cairo_translate(context,
		    cairo_image_surface_get_width(map_surface) / 2.0,
		    cairo_image_surface_get_height(map_surface) / 2.0);
    cairo_scale(context,
		cairo_image_surface_get_width(map_surface) / 2.0,
		cairo_image_surface_get_height(map_surface) / 2.0);
    
    /* Fill an ellipse with the content of `map_surface'. */
    cairo_arc(context, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
    cairo_fill_preserve(context);

    cairo_set_line_width(context, 0.001);
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    cairo_stroke(context);

    cairo_restore(context);
    
    /* Cleanup */
    cairo_surface_destroy(map_surface);
    hpix_free_bmp_projection(projection);
}

/******************************************************************************/


void
format_number(char * buf, size_t size, double number,
	      const char * measure_unit)
{
    if(measure_unit != NULL
       && measure_unit[0] != '\0')
    {
	snprintf(buf, size, "%.4g %s", number, measure_unit);
    } else {
	snprintf(buf, size, "%.4g", number);
    }
    buf[size - 1] = 0;
}

/******************************************************************************/


void
draw_ticks(cairo_t * context,
	   double start_x, double start_y,
	   double width, double height,
	   double tick_height,
	   double min_level, double max_level)
{
    const int num_of_ticks = 5;
    double range = nice_number(max_level - min_level, 0);
    double delta = nice_number(range / (num_of_ticks - 1), 1);
    double graph_min = floor(min_level / delta) * delta;
    double graph_max = ceil(max_level / delta) * delta;
    double num_of_frac_digits = -floor(log10(delta));
    double x;

    if(num_of_frac_digits < 0)
	num_of_frac_digits = 0;
	
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    for(x = graph_min; x <= graph_max + 0.5 * delta; x += delta)
    {
	double pos;
	char label[40];

	if(x < min_level || x > max_level)
	    continue;

	pos = start_x + width * (x - min_level) / (max_level - min_level);
	cairo_move_to(context, pos, start_y);
	cairo_line_to(context, pos, start_y + tick_height);
	cairo_stroke(context);

	format_number(label, sizeof(label) + 1, x, NULL);

	draw_aligned_text(context, label,
			  pos, start_y + height,
			  HALIGN_CENTER, VALIGN_TOP);
    }
}


/******************************************************************************/


void
paint_colorbar(cairo_t * context,
	       const rect_t * colorbar_rect,
	       const hpix_color_palette_t * palette,
	       double min_level, double max_level)
{
    /* Here is the inner layout of the colorbar:
     *
     * +---+--------------------------------------------------+---+
     * |   |                                                  |   |
     * | 1 |                        2                         | 3 |
     * |   |                                                  |   |
     * +---+--------------------------------------------------+---+
     * |                            4                             |
     * +----------------------------------------------------------+
     * |                                                          |
     * |                            5                             |
     * |                                                          |
     * +----------------------------------------------------------+
     *
     * 1. Minimum value of the color bar
     * 2. The gradient-filled rectangle
     * 3. Maximum value of the color bar
     * 4. Space for the vertical tick marks
     * 5. Values associated with each tick mark
     */

    cairo_pattern_t * linear;
    char label_min[20], label_max[20];
    cairo_text_extents_t min_te, max_te;
    const double text_margin_factor = 1.1;
    /* This is space 2. in the diagram above */
    rect_t bar_only_rect;
    /* This is the height of space 4. in the diagram above */
    double tick_band_height;

    cairo_set_font_size(context, colorbar_rect->height * 0.4);
    cairo_set_line_width(context, 1.0);

    format_number(label_min, sizeof(label_min) + 1, min_level, measure_unit_str);
    format_number(label_max, sizeof(label_max) + 1, max_level, measure_unit_str);

    cairo_text_extents(context, label_min, &min_te);
    cairo_text_extents(context, label_max, &max_te);

    bar_only_rect.x = colorbar_rect->x;
    bar_only_rect.y = colorbar_rect->y;
    bar_only_rect.width = colorbar_rect->width;
    bar_only_rect.height = colorbar_rect->height * 0.4;

    bar_only_rect.x += min_te.width * text_margin_factor;
    bar_only_rect.width -= (min_te.width + max_te.width) * text_margin_factor;

    if(output_format == FMT_PNG)
	tick_band_height = 6.0;
    else
	tick_band_height = 0.1;

    tick_band_height = colorbar_rect->height * 0.1;

    draw_ticks(context,
	       bar_only_rect.x,
	       bar_only_rect.y + bar_only_rect.height,
	       bar_only_rect.width,
	       colorbar_rect->height - bar_only_rect.height,
	       tick_band_height,
	       min_level, max_level);

    linear =
	cairo_pattern_create_linear(bar_only_rect.x, 0.0,
				    bar_only_rect.x + bar_only_rect.width, 0.0);

    hpix_bmp_configure_linear_gradient(linear, palette);

    cairo_rectangle(context,
		    bar_only_rect.x, bar_only_rect.y,
		    bar_only_rect.width, bar_only_rect.height);

    /* Draw the gradient */
    cairo_set_source(context, linear);
    cairo_fill_preserve(context);

    cairo_pattern_destroy(linear);

    /* Draw the border */
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    cairo_stroke(context);

    /* Draw the labels */
    {
	double baseline;
	baseline =
	    colorbar_rect->y
	    + bar_only_rect.height * 0.5
	    - min_te.y_bearing 
	    - min_te.height * 0.5;
	cairo_move_to(context, colorbar_rect->x, baseline);
	cairo_show_text(context, label_min);

	cairo_move_to(context,
		      colorbar_rect->x + colorbar_rect->width - max_te.width,
		      baseline);
	cairo_show_text(context, label_max);
    }
}

/******************************************************************************/


/* Wrapper to one of the cairo_*_surface_create function, depending on
 * the output format chosen by the user (--format). */
cairo_surface_t *
create_surface(double width, double height)
{
    cairo_surface_t * surface;

    switch(output_format)
    {
    case FMT_PNG:
	surface = cairo_image_surface_create(no_background_flag
					     ? CAIRO_FORMAT_ARGB32
					     : CAIRO_FORMAT_RGB24,
					     width, height);
	bitmap_columns = width;
	break;

#if CAIRO_HAS_PS_SURFACE
    case FMT_PS:
    case FMT_EPS:
	surface = cairo_ps_surface_create(output_file_name,
					     width, height);
	if(output_format == FMT_EPS)
	    cairo_ps_surface_set_eps(surface, TRUE);
	break;
#endif

#if CAIRO_HAS_PDF_SURFACE
    case FMT_PDF:
	surface = cairo_pdf_surface_create(output_file_name,
					     width, height);
	break;
#endif

#if CAIRO_HAS_SVG_SURFACE
    case FMT_SVG:
	surface = cairo_svg_surface_create(output_file_name,
					     width, height);
	break;
#endif
    default:
	assert(0);
    }

    return surface;
}

/******************************************************************************/


/* Determine the position and extents of the three elements of the
 * image, that is, the title, the Mollweide projection, and the color
 * bar. */
void
lay_out_page(rect_t * title_rect, rect_t * map_rect, rect_t * colorbar_rect)
{
    /* This is a small dimensionless number that quantifies how much
     * empty vertical space there should be between consecutive
     * elements in the page. */
    const double relative_margin = 0.05;

    title_rect->x = title_rect->y = 0.0;
    title_rect->width = image_width;
    if(title_str != NULL && title_str[0] != 0)
	title_rect->height = title_height_fraction * image_height;
    else
	title_rect->height = 0.0;

    colorbar_rect->x = 0.0;
    /* Leave colorbar_rect->y to the end */
    colorbar_rect->width = image_width;
    if(draw_color_bar_flag)
	colorbar_rect->height = image_height * colorbar_height_fraction;
    else
	colorbar_rect->height = 0.0;

    map_rect->x = 0.0;
    map_rect->y = title_rect->y + title_rect->height;
    map_rect->width = image_width;
    map_rect->height = image_height - title_rect->height - colorbar_rect->height;

    if(verbose_flag)
	fprintf(stderr,
		MSG_HEADER "Mollweide projection has a size of %gx%g "
		"(aspect ratio: %.3f)\n",
		map_rect->width, map_rect->height,
		map_rect->width / map_rect->height);

    /* Now calculate where the color bar should be placed vertically */
    colorbar_rect->y = map_rect->y + map_rect->height;

    /* Finally, shrink a bit the title and the color bar so that there
     * is some space between them and the map. */
    title_rect->height *= (1.0 - relative_margin);
    colorbar_rect->y += colorbar_rect->height * relative_margin;
    colorbar_rect->height *= (1.0 - relative_margin);
}

/******************************************************************************/


void
paint_and_save_figure(const hpix_map_t * map)
{
    double min, max;

    find_map_extrema(map, &min, &max);
    if(! isnan(min_value))
	min = min_value;
    if(! isnan(max_value))
	max = max_value;

    if(verbose_flag)
	fprintf(stderr,
		MSG_HEADER "map extrema are %g and %g, " 
		"with a range of %g\n",
		min, max, max - min);

    /* Steps to create the map:
     * 1. Create a surface of the appropriate type (e.g. PS, PDF...)
     * 2. Fill the background (unless --no-background was used)
     * 3. Draw the title
     * 4. Use `plot_bitmap_to_cairo_surface` to create another
     *    (bitmapped) surface containing the Mollview projection of
     *    the map
     * 5. Copy the surface with the Mollview projection into the "big"
     *    surface created in 1.
     * 6. Draw the color bar 
     * 7. Save the result
     */
    cairo_surface_t * surface = create_surface(image_width, image_height);
    cairo_t * context = cairo_create(surface);

    rect_t title_rect;
    rect_t map_rect;
    rect_t colorbar_rect;

    lay_out_page(&title_rect, &map_rect, &colorbar_rect);

    if(output_format == FMT_PNG)
	bitmap_rows = map_rect.height;

    /* Draw the background */
    if(no_background_flag)
    {
	cairo_save(context);
	cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
	cairo_paint(context);
	cairo_restore(context);
    } else {
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_paint(context);
    }

    if(title_rect.height > 0.0)
	paint_title(context, &title_rect);

    hpix_color_palette_t * palette = create_palette();
    paint_map(context, &map_rect, palette, map, min, max);

    if(colorbar_rect.height > 0.0)
	paint_colorbar(context, &colorbar_rect, palette, min, max);
    hpix_free_color_palette(palette);

    if(output_format == FMT_PNG)
    {
	fprintf(stderr, MSG_HEADER "writing the file to `%s'\n",
		output_file_name);
	if(cairo_surface_write_to_png(surface, output_file_name)
	   != CAIRO_STATUS_SUCCESS)
	{
	    fprintf(stderr, MSG_HEADER "unable to write to file '%s'\n",
		    output_file_name);
	    exit(EXIT_FAILURE);
	}
	fputs(MSG_HEADER "file has been written successfully\n", stderr);
    } else {
	cairo_show_page(context);
    }

    cairo_destroy(context);
    cairo_surface_destroy(surface);
}

/******************************************************************************/


int
main(int argc, const char ** argv)
{
    hpix_map_t * map;

    parse_command_line(argc, argv);

    if(verbose_flag)
	fprintf(stderr, MSG_HEADER "loading map `%s'\n", input_file_name);
    map = load_map_and_rescale_if_needed();
    if(verbose_flag)
	fprintf(stderr, MSG_HEADER "map loaded\n");

    if(verbose_flag)
	fprintf(stderr, MSG_HEADER "painting map\n");
    paint_and_save_figure(map);

    hpix_free_map(map);

    return 0;
}
