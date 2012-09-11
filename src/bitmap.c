#include <chpx.h>
#include <math.h>
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

unsigned int
chpx_projection_width(const chpx_bmp_projection_t * proj)
{
    assert(proj);
    return proj->width;
}

unsigned int
chpx_projection_height(const chpx_bmp_projection_t * proj)
{
    assert(proj);
    return proj->height;
}

double *
chpx_bmp_trace_bitmap(const chpx_bmp_projection_t * proj,
		      const chpx_map_t * map,
		      double * min_value,
		      double * max_value)
{
    unsigned int x;
    unsigned int y;
    double center_x = chpx_projection_width(proj) / 2.0;
    double center_y = chpx_projection_height(proj) / 2.0;
    double *bitmap;
    double *bitmap_ptr;
    chpx_angles_to_pixel_t * angles_to_pixel_fn;
    chpx_nside_t nside;

    assert(proj);
    assert(map);

    nside = chpx_map_nside(map);

    angles_to_pixel_fn = (chpx_map_ordering(map) == CHPX_ORDER_NEST)
	? chpx_angles_to_nest_pixel
	: chpx_angles_to_ring_pixel;

    bitmap = chpx_malloc(sizeof(bitmap[0]),
			 chpx_projection_width(proj) *
			 chpx_projection_height(proj));
    bitmap_ptr = bitmap;

    for (y = 0; y < chpx_projection_height(proj); ++y)
    {
	for (x = 0; x < chpx_projection_height(proj); ++x)
	{
	    double u = 2.0 * (x - center_x) / (center_x / 1.02);
	    double v = (y - center_y) / (center_y / 1.02);
	    double theta;
	    double phi;
	    chpx_pixel_num_t pixel_idx;

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
	    *bitmap_ptr++ = chpx_map_pixels(map)[pixel_idx];
	}
    }

    return bitmap;
}
