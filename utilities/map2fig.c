#include <hpix.h>
#include <memory.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include "gopt.h"

#define VERSION "0.1"

const unsigned int bitmap_width = 800;
const unsigned int bitmap_height = 640;

/* Should we draw a color bar? Set by `-b`, `--draw-color-bar` */
int draw_color_bar_flag = 0;

/* String to append to the measure unit, set by `-m`, `--measure-unit` */
const char * measure_unit = NULL;

/* Title to be drawn above the map, set by `-t`, `--title` */
const char * title = NULL;

/* Name of the output file name, set by `-o`, `--output` */
const char * output_file_name = NULL;

/* C-like format string for numbers, set by `-f`, `--format` */
const char * number_format = "%g";

const char * input_file_name = NULL;

/* Number of the column to display, set by `-c`, `--column` */
unsigned short column_number;

/* Relative height of the title and of the color bar. Together with
 * the height of the map, their sum is 1.0 */
const float title_height_fraction = 0.1;
const float colorbar_height_fraction = 0.2;

/******************************************************************************/


void
print_usage(const char * program_name)
{
    printf("Usage: %s [OPTIONS] INPUT_MAP\n\n", program_name);
    puts("OPTIONS can be one or more of the following:");
    puts("  -b, --draw-color-bar      Draw a color bar");
    puts("  -c, --column=NUM          Number of the column to display");
    puts("  -f, --format=STRING       C-like formatting string for numbers");
    puts("  -m, --measure-unit=STRING Measure unit to use.");
    puts("  --min=VALUE, --max=VALUE  Minimum and maximum value to be used");
    puts("                            at the extrema of the color bar");
    puts("  -o, --output=FILE         Save the image to the specified file");
    puts("  -t, --title=TITLE         Title to be written");
    puts("  -v, --version             Print version number and exit");
    puts("  -h, --help                Print this help");
}

/******************************************************************************/


void
parse_command_line(int argc, const char ** argv)
{
    const char * column_str;
    char * tail_ptr;
    void * options =
	gopt_sort(&argc, argv,
		  gopt_start(
		      gopt_option('h', 0, gopt_shorts('h', '?'), gopt_longs("help")),
		      gopt_option('v', 0, gopt_shorts('v'), gopt_longs("version")),
		      gopt_option('b', 0, gopt_shorts('b'), gopt_longs("draw-color-bar")),
		      gopt_option('c', 0, gopt_shorts('c'), gopt_longs("column")),	
		      gopt_option('m', GOPT_ARG, gopt_shorts('m'), gopt_longs("measure-unit")),
		      gopt_option('_', GOPT_ARG, gopt_shorts(0), gopt_longs("min")),
		      gopt_option('^', GOPT_ARG, gopt_shorts(0), gopt_longs("max")),
		      gopt_option('t', GOPT_ARG, gopt_shorts('t'), gopt_longs("title")),
		      gopt_option('f', GOPT_ARG, gopt_shorts('f'), gopt_longs("format"))));

    if(gopt(options, 'h'))
    {
	print_usage("map2fig");
	exit(EXIT_SUCCESS);
    }

    if(gopt(options, 'v'))
    {
	puts("map2fig version " VERSION " - Copyright(c) 2011-2012 Maurizio Tomasi");
	exit(EXIT_SUCCESS);
    }

    if(gopt(options, 'b'))
	draw_color_bar_flag = 1;

    if(gopt_arg(options, 'c', &column_str))
    {
	column_number = strtoul(column_str, &tail_ptr, 10);
	if(! tail_ptr ||
	   *tail_ptr == '\x0' ||
	   column_number == 0 ||
	   column_number > USHRT_MAX)
	{
	    fprintf(stderr, "map2fig: invalid column number '%s'\n",
		    column_str);
	    exit(EXIT_FAILURE);
	}
    }

    /* Save the option passed to `--measure-unit` into variable MEASURE_UNIT. */
    gopt_arg(options, 'm', &measure_unit);
    gopt_arg(options, 't', &title);
    gopt_arg(options, 'f', &number_format);

    gopt_free(options);

    if(argc > 2)
    {
	fputs("map2fig: too many command-line arguments (hint: use --help)\n",
	      stderr);
	exit(EXIT_FAILURE);
    }

    if(argc < 2)
    {
	fprintf(stderr, "map2fig: reading maps from stdin is not supported yet\n"
		"map2fig: (hint: specify the name of a FITS file to be read)\n");
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

    if(! hpix_load_fits_component_from_file(input_file_name,
					    column_number,
					    &result, &status))
    {
	fprintf(stderr, "map2fig: unable to load file '%s'\n",
		input_file_name);
	exit(EXIT_FAILURE);
    }

    return result;
}

/******************************************************************************/


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
	    *min = *max = *cur_pixel;
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


typedef struct {
    double red;
    double green;
    double blue;
} color_t;

static const double levels[] = { 0.0, 0.15, 0.40, 0.70, 0.90, 1.00 };
static const color_t colors[] = {
    { 0.0, 0.0, 0.5 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 1.0, 1.0 },
    { 1.0, 1.0, 0.0 },
    { 1.0, 0.33, 0.0 },
    { 0.5, 0.0, 0.0 }};
static const size_t num_of_levels = sizeof(levels) / sizeof(levels[0]);

void
get_palette_color(double level, color_t * color_ptr)
{
    size_t idx;
    size_t index0, index1;

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

    idx = 0;
    while(level > levels[idx])
	++idx;

    index1 = idx;
    index0 = index1 - 1;

#define INTERPOLATE_COMPONENT(level, comp_name) \
    (  colors[index0].comp_name * (levels[index1] - level) / (levels[index1] - levels[index0]) \
     + colors[index1].comp_name * (level - levels[index0]) / (levels[index1] - levels[index0]))

    color_ptr->red   = INTERPOLATE_COMPONENT(level, red);
    color_ptr->green = INTERPOLATE_COMPONENT(level, green);
    color_ptr->blue  = INTERPOLATE_COMPONENT(level, blue);

#undef INTERPOLATE_COMPONENT
}

/******************************************************************************/


void
plot_bitmap_to_cairo_surface(cairo_t * cairo_context,
			     double origin_x, double origin_y,
			     double size_x, double size_y,
			     double map_min, double map_max,
			     const float * bitmap,
			     unsigned int bitmap_width,
			     unsigned int bitmap_height)
{
    const double pixel_width  = size_x / bitmap_width;
    const double pixel_height = size_y / bitmap_height;
    const double dynamic_range = map_max - map_min;
    const float * cur_pixel = bitmap;

    unsigned int cur_y;

    for(cur_y = bitmap_height; cur_y > 0; --cur_y)
    {
	unsigned int cur_x;
	for(cur_x = 0; cur_x < bitmap_width; ++cur_x)
	{
	    double value = *bitmap++;
	    color_t color;

	    if(isinf(value))
		continue;
	    else if (isnan(value) || value < -1.6e+30)
		color.red = color.green = color.blue = 0.5;
	    else
	    {
		double normalized_value = (value - map_min) / dynamic_range;
		get_palette_color(normalized_value, &color);
	    }

	    cairo_rectangle(cairo_context,
			    origin_x + (cur_x * size_x) / bitmap_width,
			    origin_y + (cur_y * size_y) / bitmap_height,
			    pixel_width,
			    pixel_height);
	    cairo_set_source_rgb(cairo_context,
				 color.red,
				 color.green,
				 color.blue);
	    cairo_fill(cairo_context);
	}
    }
}

/******************************************************************************/


void
paint_map(const hpix_map_t * map)
{
    cairo_surface_t * surface;
    cairo_context_t * context;
    hpix_bmp_projection_t * projection;
    double * bitmap;
    double min, max;

    projection = hpix_create_bmp_projection(640, 480);
    bitmap = hpix_bmp_trace_bitmap(projection, map, NULL, NULL);
    hpix_free_bmp_projection(projection);

    find_map_extrema(map, &min, &max);

    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
					 bitmap_width,
					 bitmap_height);
    context = cairo_create(surface);

    cairo_rectangle(context, 0.0, 0.0, width, height);
    cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
    cairo_fill(context);

    plot_bitmap_to_cairo_surface(context, 0.0, 0.0,
				 bitmap_width, bitmap_height,
				 min, max,
				 bitmap,
				 bitmap_width, bitmap_height);

    if(cairo_surface_write_to_png(surface, output_file_name)
       != CAIRO_STATUS_SUCCESS)
    {
	fprintf(stderr, "map2fig: unable to write to file '%s'\n",
		output_file_name);
	exit(EXIT_FAILURE);
    }
}

/******************************************************************************/


int
main(int argc, const char ** argv)
{
    hpix_map_t * map;

    parse_command_line(argc, argv);
    map = load_map();
    paint_map(map);
    hpix_free_map(map);

    return 0;
}
