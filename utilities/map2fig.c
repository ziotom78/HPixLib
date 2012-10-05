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

#define VERSION "0.1"

/* Used to print information/error/warning messages */
#define MSG_HEADER   "map2fig: "

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
    { NULL, FMT_NULL }
};

/* Output format to use, see above */
output_format_code_t output_format = FMT_PNG;

/* Should we skip painting the image background? Set by `--no-background` */
int no_background_flag = 0;

/* Should we draw a color bar? Set by `-b`, `--draw-color-bar` */
int draw_color_bar_flag = 0;

/* Should we produce a number of diagnostic messages? Set by `--verbose` */
int verbose_flag = 0;

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
    puts("  -s, --scale=NUM           Multiply the pixel values by NUM");
    puts("                            (useful e.g. to convert K into mK)");
    puts("  -t, --title=TITLE         Title to be written");
    puts("  --tick-font-size=NUM      Height of the tick labels in the");
    puts("                            color bar (in pixels if exporting to");
    puts("                            PNG, in dots otherwise)");
    puts("  --title-font-size=NUM     Height of the title (in pixels if");
    puts("                            exporting to PNG, in dots otherwise)");
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
		      gopt_option('B', 0, gopt_shorts(0), gopt_longs("no-background")),
		      gopt_option('c', 0, gopt_shorts('c'), gopt_longs("column")),	
		      gopt_option('F', 0, gopt_shorts(0), gopt_longs("list-formats")),
		      gopt_option('m', GOPT_ARG, gopt_shorts('m'), gopt_longs("measure-unit")),
		      gopt_option('o', GOPT_ARG, gopt_shorts('o'), gopt_longs("output")),
		      gopt_option('_', GOPT_ARG, gopt_shorts(0), gopt_longs("min")),
		      gopt_option('^', GOPT_ARG, gopt_shorts(0), gopt_longs("max")),
		      gopt_option('s', GOPT_ARG, gopt_shorts('s'), gopt_longs("scale")),
		      gopt_option('t', GOPT_ARG, gopt_shorts('t'), gopt_longs("title")),
		      gopt_option('1', GOPT_ARG, gopt_shorts(0), gopt_longs("tick-font-size")),
		      gopt_option('2', GOPT_ARG, gopt_shorts(0), gopt_longs("title-font-size")),
		      gopt_option('f', GOPT_ARG, gopt_shorts('f'), gopt_longs("format"))));

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

    /* --verbose */
    if(gopt(options, 'V'))
	verbose_flag = 1;

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

    /* Here we parse all those options of the form --name VALUE, with
     * VALUE being a floating-point number. */
    min_value = parse_double_from_options(options, '_', NAN, "--min");
    max_value = parse_double_from_options(options, '^', NAN, "--max");
    scale_factor = parse_double_from_options(options, 's', 1.0, "--scale");
    tick_font_size = parse_double_from_options(options, '1',
					       tick_font_size,
					       "--tick-font-size");
    title_font_size = parse_double_from_options(options, '2',
						title_font_size,
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
	image_height = 500;
	break;

    case FMT_PS:
    case FMT_EPS:
    case FMT_PDF:
    case FMT_SVG:
	/* Points, that is, 1/72 inches */
	image_width = 7.5 * 72;
	image_height = 5.0 * 72;
	break;

    default:
	assert(0);
    }

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
load_map(void)
{
    hpix_map_t * result;
    int status = 0;
    double * pixels;
    size_t num_of_pixels;
    size_t idx;

    if(! hpix_load_fits_component_from_file(input_file_name,
					    column_number,
					    &result, &status))
    {
	fprintf(stderr, MSG_HEADER "unable to load file '%s'\n",
		input_file_name);
	exit(EXIT_FAILURE);
    }

    /* Multiply the pixels in the map by `scale_factor` */
    pixels = hpix_map_pixels(result);
    num_of_pixels = hpix_map_num_of_pixels(result);
    for(idx = 0; idx < num_of_pixels; ++idx)
	pixels[idx] *= scale_factor;

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
	if(isnan(*cur_pixel) || *cur_pixel < -1.6e+30)
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
 * Mollview projection of the map. The values `map_min` and `map_max`
 * are used to rescale every value in the map (i.e. to convert every
 * pixel value in the map into a number in [0.0, 1.0]). The value of
 * `bitmap` is the return value of `hpix_bmp_trace_bitmap`. Finally,
 * `width` and `height` give the resolution (in pixel) of the image
 * surface. The Mollview plot fits into the largest ellipse that can
 * be enclosed in the rectangle (0,0) - (width, height). */
cairo_surface_t *
plot_bitmap_to_cairo_surface(double map_min, double map_max,
			     const double * bitmap,
			     unsigned int width,
			     unsigned int height)
{
    const double dynamic_range = map_max - map_min;
    const double * cur_pixel = bitmap;
    unsigned char * image_data;
    unsigned int cur_y;
    unsigned int stride;
    cairo_surface_t * surface;

    assert(bitmap);
    fprintf(stderr, MSG_HEADER "plotting the map on a %ux%u bitmap\n",
	    width, height);
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

    return surface;
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
paint_title(cairo_t * context, double start_x, double start_y,
	    double width, double height)
{
    const double title_font_size = height * 0.9;

    cairo_set_font_size(context, title_font_size);
    cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
    draw_aligned_text(context, title_str,
		      start_x + 0.5 * width, start_y + 0.5 * height,
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


void
format_number(char * buf, size_t size, double number)
{
    if(measure_unit_str != NULL
       && measure_unit_str[0] != '\0')
    {
	snprintf(buf, size, "%.4g %s", number, measure_unit_str);
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
	cairo_text_extents_t te;

	if(x < min_level || x > max_level)
	    continue;

	pos = start_x + width * (x - min_level) / (max_level - min_level);
	cairo_move_to(context, pos, start_y);
	cairo_line_to(context, pos, start_y + tick_height);
	cairo_stroke(context);

	format_number(label, sizeof(label) + 1, x);

	/* We need to decide which is the baseline of the font. The
	 * function `draw_aligned_text` does not include the space
	 * needed below the baseline (e.g. the tail of the letter
	 * 'y'). But in this case we must compensate for it, as these
	 * letters are going to be painted right along the bottom
	 * border of the image. */
	cairo_text_extents(context, label, &te);
	draw_aligned_text(context, label,
			  pos, start_y + height - (te.height + te.y_bearing),
			  HALIGN_CENTER, VALIGN_TOP);
    }
}


/******************************************************************************/


void
paint_colorbar(cairo_t * context,
	       double start_x, double start_y,
	       double width, double height,
	       double min_level, double max_level)
{
    cairo_pattern_t * linear;
    size_t idx;
    char label_min[20], label_max[20];
    cairo_text_extents_t min_te, max_te;
    double bar_start_x, bar_start_y;
    double bar_width, bar_height;
    const double text_margin_factor = 1.1;
    double tick_band_height;

    if(output_format == FMT_PNG)
	tick_band_height = 6.0;
    else
	tick_band_height = 0.1;

    cairo_set_font_size(context, height * 0.4);

    format_number(label_min, sizeof(label_min) + 1, min_level);
    format_number(label_max, sizeof(label_max) + 1, max_level);

    cairo_text_extents(context, label_min, &min_te);
    cairo_text_extents(context, label_max, &max_te);

    bar_start_x = start_x;
    bar_start_y = start_y + height * 0.05;
    bar_width = width;
    bar_height = height * 0.45; /* = 0.5 - 0.05 (the number above) */

    bar_start_x += min_te.width * text_margin_factor;
    bar_width -= (min_te.width + max_te.width) * text_margin_factor;

    draw_ticks(context,
	       bar_start_x, bar_start_y + bar_height - tick_band_height,
	       bar_width, height - bar_height + tick_band_height,
	       height - (bar_height + min_te.height) * 1.05,
	       min_level, max_level);

    linear = cairo_pattern_create_linear (bar_start_x, 0.0,
					  bar_start_x + bar_width, 0.0);

    for(idx = 0; idx < num_of_levels; ++idx)
    {
	cairo_pattern_add_color_stop_rgb(linear, levels[idx],
					 colors[idx].red,
					 colors[idx].green,
					 colors[idx].blue);
    }

    cairo_rectangle(context,
		    bar_start_x, bar_start_y,
		    bar_width, bar_height - tick_band_height);

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
	    start_y
	    + bar_height * 0.5
	    - min_te.y_bearing 
	    - min_te.height * 0.5;
	cairo_move_to(context, start_x, baseline);
	cairo_show_text(context, label_min);

	cairo_move_to(context, start_x + width - max_te.width,
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


void
paint_map(const hpix_map_t * map)
{
    cairo_surface_t * surface;
    cairo_t * context;
    hpix_bmp_projection_t * projection;
    double min, max;

    const double title_start_y = 0.0;
    const double map_start_y = title_height_fraction * image_height;
    const double colorbar_start_y = image_height
	* (1 - colorbar_height_fraction);

    const double title_height = map_start_y;
    const double map_height = image_height
	* (1 - title_height_fraction - colorbar_height_fraction);
    const double colorbar_height = image_height * colorbar_height_fraction;

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
    surface = create_surface(image_width, image_height);
    context = cairo_create(surface);

    if(output_format == FMT_PNG)
	bitmap_rows = map_height;

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

    /* Draw the title */
    paint_title(context,
		0.0, title_start_y,
		image_width, title_height);

    /* Plot the map */
    {
	cairo_surface_t * map_surface;
	double * map_bitmap;
	const double reduce_factor = 1.02;

	/* First produce a cairo image surface with the map in it */
	projection = hpix_create_bmp_projection((int) (bitmap_columns + .5),
						(int) (bitmap_rows + .5));
	map_bitmap = hpix_bmp_trace_bitmap(projection, map, NULL, NULL);
	map_surface =
	    plot_bitmap_to_cairo_surface(min, max,
					 map_bitmap,
					 hpix_bmp_projection_width(projection),
					 hpix_bmp_projection_height(projection));
	hpix_free(map_bitmap);

	/* Now copy the cairo surface into the surface we're currently
	 * using to draw the figure */
	cairo_save(context);

	/* These transformations are useful for the map containing the
	 * bitmap (i.e. the source in the copy operation). */
	cairo_translate(context, 0.0, map_start_y);
	cairo_scale(context,
		    image_width / cairo_image_surface_get_width(map_surface),
		    map_height  / cairo_image_surface_get_height(map_surface));
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
	cairo_fill(context);
	cairo_restore(context);

	/* Cleanup */
	cairo_surface_destroy(map_surface);
	hpix_free_bmp_projection(projection);
    }

    paint_colorbar(context,
		   0.01 * image_width, colorbar_start_y,
		   image_width * 0.98, colorbar_height,
		   min, max);

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
    map = load_map();
    if(verbose_flag)
	fprintf(stderr, MSG_HEADER "map loaded\n");

    if(verbose_flag)
	fprintf(stderr, MSG_HEADER "painting map\n");
    paint_map(map);

    hpix_free_map(map);

    return 0;
}
