/*6:*/
#line 89 "./chealpix.w"

#include <chealpix.h> 

pixel_num_t
chealpix_nside_to_npixel(const unsigned short nside)
{
return 12*nside*nside;
}

/*:6*/
