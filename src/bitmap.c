#include <chpx.h>
#include <math.h>
#include <assert.h>

/* Given a map projection \a proj, this function determine which
 * pixels of \a map fall within the point located at (\a x, \a y). At
 * the end of the execution, \a hits will be initialized with a list
 * of the indexes for the hit pixels in \a map, and \a num_of_hits
 * will contain the number of hits (i.e. the number of elements in the
 * array \a hits). The function returns nonzero if at least one hit
 * has been found, zero otherwise.
 *
 * The array \a hits is created on the heap by this function and needs
 * therefore to be freed using #chpx_free. Since however the typical
 * usage of this function is within a loop with many elements (see
 * e.g. #chpx_trace_bitmap), the function will not allocate space for
 * \a hits if (1) it is not null, and (2) \a num_of_allocated_hits is
 * greater than zero. In such case, the function will assume that \a
 * hits is a heap variable allocated using #chpx_alloc that is already
 * able to contain at most \a num_of_allocated_hits elements. If this
 * size is not enough, the function will call #chpx_realloc to change
 * the size of the buffer (and will change the value in \a
 * num_of_allocated_hits accordingly). In this way, heap
 * allocations/deallocations are minimized.
 */
static int
get_hits(const chpx_bmp_projection_t * proj,
	 const chpx_map_t * map, 
	 unsigned int x, 
	 unsigned int y, 
	 size_t ** hits,
	 size_t * num_of_allocated_hits,
	 size_t * num_of_hits)
{
    assert(hits);
    assert(num_of_allocated_hits);
    assert(num_of_hits);

    return *num_of_hits > 0;
}

/** Paint a bitmapped representation of the map
 *
 * This function creates a bitmap (rectangular array of numbers)
 * representing \a map. The details of the projection are specified by
 * the \a proj parameter (size of the bitmap, set of coordinates to be
 * used and so on). The bitmap is an array of floating-point values,
 * each using the same scale as in the original map (i.e. if the map
 * represents a set of temperatures in Kelvin, then each pixel in the
 * bitmap will be measured in Kelvin as well).
 *
 * When the bitmap returned by this function is no longer useful, you
 * must free it using #chpx_free.
 *
 * The typical usage is to produce a bitmap, then use \a min_value and
 * \a max_value to scale it from the map measure unit into a color
 * space. In the following example we imagine to use a graphics
 * library which implements * two functions: \c paint_pixel, which
 * draw a pixel at a specified coordinate with a given color, and \c
 * RGB, which returns a color given its red, green and blue
 * components. The code here converts each value in the map into a
 * gray level using a linear transformation, and then outputs a pixel
 * with red, green and blue components all equal. (Note that this is
 * only an approximation of gray: you should instead weight
 * differently the red, green and blue components according to some
 * perceptual model -- here we simply ignore this complication.) Here
 * is the code:

 * \code
 * chpx_bmp_projection_t * proj;
 * double * bitmap;
 * double min, max;
 * size_t i, x, y; 
 *
 * proj = chpx_new_projection(640, 480, COORD_GALACTIC);
 * bitmap = chpx_bmp_trace_bitmap(proj, map, &min, &max);
 *
 * i = 0;
 * for(y = 0; y < proj->height; ++y)
 * {
 *     for(x = 0; x < proj->width; ++x)
 *     {
 *         int gray = 255 * (bitmap[i++] - min) / (max - min));
 *         paint_pixel(x, y, RGB(gray, gray, gray));
 *     }
 * }
 *
 * chpx_free(bitmap);
 * chpx_free_projection(proj);
 * \endcode
 */
double *
chpx_bmp_trace_bitmap(const chpx_bmp_projection_t * proj,
		      const chpx_map_t * map,
		      double * min_value,
		      double * max_value)
{
    unsigned int x;
    unsigned int y;
    double       *bitmap;
    double       *bitmap_ptr;
    size_t       *hits = NULL;
    size_t       num_of_allocated_hits = 0;

    assert(proj);
    assert(map);

    bitmap = chpx_malloc(sizeof(bitmap[0]), proj->width * proj->height);
    bitmap_ptr = bitmap;

    for (y = 0; y < proj->height; ++y) {
	for (x = 0; x < proj->width; ++x) {
	    size_t          num_of_hits = 0;
	    int             hit_result;

	    hit_result = get_hits(proj, map, x, y, &hits,
				  &num_of_allocated_hits,
				  &num_of_hits);

	    if (hit_result) {
		/* Average the hits and update `min_value' and
		 * `max_value' */
	    } else
		*bitmap_ptr = NAN;
	}
    }

    return bitmap;
}
