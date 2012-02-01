#include <chpx.h>
#include <stdlib.h>
#include <assert.h>

void
chpx_query_disc(double theta, double phi, double radius,
		chpx_pixel_num_t ** pixels,
		size_t * num_of_matches)
{
    assert(pixels != NULL);
    assert(num_of_matches != NULL);

    *pixels = NULL;
    *num_of_matches = 0;
}

void
chpx_query_disc_inclusive(double theta, double phi, double radius,
			  chpx_pixel_num_t ** pixels,
			  size_t * num_of_matches)
{
    assert(pixels != NULL);
    assert(num_of_matches != NULL);

    *pixels = NULL;
    *num_of_matches = 0;
}
