#include <chpx.h>
#include <math.h>

chpx_pixel_num_t
chpx_nside_to_npixel(chpx_nside_t nside)
{
    if (nside > 0)
	return 12 * nside * nside;
    else
	return 0;
}

chpx_nside_t
chpx_npixel_to_nside(chpx_pixel_num_t npixels)
{
	unsigned short  nside_estimate = sqrt(npixels / 12.0);
	if (chpx_nside_to_npixel(nside_estimate) != npixels)
		return 0;
	else
		return nside_estimate;
}
