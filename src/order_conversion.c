/* order_conversion.c -- functions to change the order of pixels for a
 * map from RING to NESTED and vice versa.
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
#include <math.h>

#include "xy2pix.c"
#include "pix2xy.c"

static const uint16_t utab[] = {
#define Z(a) 0x##a##0, 0x##a##1, 0x##a##4, 0x##a##5
#define Y(a) Z(a##0), Z(a##1), Z(a##4), Z(a##5)
#define X(a) Y(a##0), Y(a##1), Y(a##4), Y(a##5)
X(0),X(1),X(4),X(5)
#undef X
#undef Y
#undef Z
};

static const uint16_t ctab[] = {
#define Z(a) a,a+1,a+256,a+257
#define Y(a) Z(a),Z(a+2),Z(a+512),Z(a+514)
#define X(a) Y(a),Y(a+4),Y(a+1024),Y(a+1028)
X(0),X(8),X(2048),X(2056)
#undef X
#undef Y
#undef Z
};

static const int jrll[12] = { 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
static const int jpll[12] = { 1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7 };
static const int swap_clen[] = {
    0,  /* NSIDE=1 */
    7,  /* NSIDE=2 */
    5,  /* NSIDE=4 */
    4,  /* NSIDE=8 */
    12, /* NSIDE=16 */
    10, /* NSIDE=32 */
    13, /* NSIDE=64 */
    18, /* NSIDE=128 */
    14, /* NSIDE=256 */
    19, /* NSIDE=512 */
    18, /* NSIDE=1024 */
    17, /* NSIDE=2048 */
    27, /* NSIDE=4096 */
    21  /* NSIDE=8192 */
};

static const int swap_cycle[] = {
    0, 1, 8, 12, 16, 21, 40,
    0, 1, 2, 40, 114,
    0, 4, 160, 263,
    0, 4, 30, 49, 51, 87, 526, 1027, 1105, 1387, 1807, 2637,
    0, 8, 10, 18, 39, 74, 146, 307, 452, 4737,
    0, 1, 2, 7, 9, 17, 80, 410, 1526, 1921, 32859, 33566, 38931,
    0, 5, 6, 10, 12, 24, 27, 95, 372, 494, 924, 1409, 3492, 4248, 9137, 66043, 103369, 156899,
    0, 1, 2, 3, 4, 45, 125, 351, 697, 24337, 102940, 266194, 341855, 419857,
    0, 1, 2, 3, 9, 16, 1705, 2082, 2126, 8177, 12753, 15410, 52642, 80493, 83235, 88387, 99444,
      1675361, 2495125,
    0, 2, 6, 8, 9, 11, 20, 50, 93, 152, 183, 2137, 13671, 44794, 486954, 741908, 4803258,
      5692573,
    0, 1, 5, 6, 44, 53, 470, 2847, 3433, 4906, 13654, 14710, 400447, 1797382, 2744492,
      18775974, 23541521,
    0, 4, 9, 10, 16, 33, 83, 117, 318, 451, 5759, 10015, 128975, 171834, 211256, 347608,
      1278690, 2154097, 2590798, 3427694, 5581717, 21012301, 27023976, 72522811,
      95032729, 139166747, 171822389,
    0, 5, 10, 267, 344, 363, 2968, 3159, 9083, 18437, 76602, 147614, 1246902, 1593138,
      2035574, 6529391, 9511830, 11340287, 29565945, 281666026, 677946848
};

/**********************************************************************/


static uint64_t
spread_bits(unsigned int v)
{
    return ( (uint64_t) utab[ v        & 0xff])
	|  (((uint64_t) utab[(v >>  8) & 0xff]) << 16)
	|  (((uint64_t) utab[(v >> 16) & 0xff]) << 32)
	|  (((uint64_t) utab[(v >> 24) & 0xff]) << 48);
}

/**********************************************************************/


static unsigned int
compress_bits(uint64_t v)
{
  uint64_t raw = v & 0x5555555555555555ull;
  raw |= raw >> 15;
  return ctab[ raw        & 0xff]
      | (ctab[(raw >>  8) & 0xff] <<  4)
      | (ctab[(raw >> 32) & 0xff] << 16)
      | (ctab[(raw >> 40) & 0xff] << 20);
}

/**********************************************************************/


typedef struct {
    uint64_t ix;
    uint64_t iy;
    unsigned face_num;
} xyf_pixel_t;

/**********************************************************************/


static xyf_pixel_t
nest2xyf(const hpix_resolution_t * resolution,
	 hpix_pixel_num_t index)
{
    assert(resolution != NULL);

    unsigned face_num = index >> (2 * resolution->order);

    hpix_pixel_num_t masked_index = index;
    masked_index &= (resolution->npface - 1);

    return (xyf_pixel_t) {
	.face_num = face_num,
	.ix = compress_bits(masked_index),
	.iy = compress_bits(masked_index / 2)
    };
}

/**********************************************************************/


static hpix_pixel_num_t
xyf2nest(const hpix_resolution_t * resolution,
	 xyf_pixel_t xyf)
{
    return (xyf.face_num << (2 * resolution->order))
	+ spread_bits(xyf.ix)
	+ 2 * spread_bits(xyf.iy);
}

/**********************************************************************/


static xyf_pixel_t
ring2xyf(const hpix_resolution_t * resolution,
	 hpix_pixel_num_t index)
{
    assert(resolution != NULL);
    hpix_nside_t nside_times_two = resolution->nside * 2;
    unsigned face_num;
    long iring, iphi, kshift, nr;

    if(index < resolution->ncap)
    {
	/* North polar cap */
	iring = (1 + hpix_isqrt(1 + 2 * index)) >> 1; /* Counted from North pole */
	iphi  = (index + 1) - 2 * iring * (iring - 1);
	kshift = 0;
	nr = iring;
	face_num = (iphi - 1) / nr;
    } else if (index < (resolution->num_of_pixels - resolution->ncap))
    {
	/* Equatorial region */
	long ip = index - resolution->ncap;
	long tmp = (resolution->order >= 0)
	    ? ip >> (resolution->order + 2)
	    : ip / (4 * resolution->order);
	iring = tmp + resolution->nside;
	iphi = ip - tmp * 4 * resolution->nside + 1;
	kshift = (iring + resolution->nside) & 1;
	nr = resolution->nside;
	long ire = iring - resolution->nside + 1;
	long irm = resolution->nside_times_two + 2 - ire;
	long ifm = iphi - ire/2 + resolution->nside -1;
	long ifp = iphi - irm/2 + resolution->nside -1;
	if (resolution->order>=0)
	{
	    ifm >>= resolution->order;
	    ifp >>= resolution->order;
	}
	else
	{
	    ifm /= resolution->nside;
	    ifp /= resolution->nside;
	}
	face_num = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));
    } else
    {
	/* South polar cap */
	hpix_pixel_num_t ip = resolution->num_of_pixels - index;
	iring = (1 + hpix_isqrt(2 * ip - 1)) >> 1; /* Counted from South pole */
	iphi  = 4 * iring + 1 - (ip - 2 * iring * (iring - 1));
	kshift = 0;
	nr = iring;
	iring = 2 * resolution->nside_times_two - iring;
	face_num = 8 + (iphi - 1) / nr;
    }
    hpix_pixel_num_t masked_index = index;
    masked_index &= (resolution->npface - 1);

  long irt = iring - (jrll[face_num] * resolution->nside) + 1;
  long ipt = 2*iphi - jpll[face_num] * nr - kshift -1;
  if (ipt >= resolution->nside_times_two)
      ipt -= 8*resolution->nside;

    return (xyf_pixel_t) {
	.face_num = face_num,
	.ix = (ipt-irt) >>1,
	.iy = (-ipt-irt) >>1
    };
}

/**********************************************************************/


static void
get_ring_info_small(const hpix_resolution_t * resolution,
		    unsigned ring,
		    hpix_pixel_num_t * startpix,
		    hpix_pixel_num_t * ringpix,
		    _Bool * shifted)
{
    assert(resolution != NULL);
    assert(startpix != NULL);
    assert(ringpix != NULL);
    assert(shifted != NULL);

    if (ring < resolution->nside)
    {
	*ringpix = 4 * ring;
	*startpix = 2 * ring * (ring - 1);
	*shifted = TRUE;
    }
    else if (ring < 3 * resolution->nside)
    {
	*shifted = ((ring - resolution->nside) & 1) == 0;
	*ringpix = 4 * resolution->nside;
	*startpix = resolution->ncap + (ring - resolution->nside) * (*ringpix);
    }
    else
    {
	unsigned nr = 4 * resolution->nside - ring;
	*ringpix = 4 * nr;
	*startpix = resolution->num_of_pixels - 2 * nr * (nr + 1);
	*shifted = TRUE;
    }
}

/**********************************************************************/


static hpix_pixel_num_t
xyf2ring(const hpix_resolution_t * resolution,
	 xyf_pixel_t xyf)
{
    unsigned nside_times_four = 4 * resolution->nside;
    unsigned jr = (jrll[xyf.face_num]*resolution->nside) - xyf.ix - xyf.iy  - 1;

    hpix_pixel_num_t nr, kshift, n_before;

    _Bool shifted;
    get_ring_info_small(resolution, jr, &n_before, &nr, &shifted);
    nr >>= 2;
    kshift = 1 - shifted;
    unsigned jp = (jpll[xyf.face_num] * nr + xyf.ix - xyf.iy + 1 + kshift) / 2;
    assert(jp <= 4 * nr);
    if (jp < 1)
    {
	/* Assumption: if this triggers, then resolution->nsidetimes_four==4*nr */
	jp += resolution->nside_times_four;
    }

    return n_before + jp - 1;
}

/**********************************************************************/


hpix_pixel_num_t
hpix_nest_to_ring_idx(const hpix_resolution_t * resolution,
		      hpix_pixel_num_t nest_index)
{
    xyf_pixel_t xyf = nest2xyf(resolution, nest_index);
    return xyf2ring(resolution, xyf);
}

/**********************************************************************/


hpix_pixel_num_t
hpix_ring_to_nest_idx(const hpix_resolution_t * resolution,
		      hpix_pixel_num_t ring_index)
{
    xyf_pixel_t xyf = ring2xyf(resolution, ring_index);
    return xyf2nest(resolution, xyf);
}

/**********************************************************************/


static const int *
cycles_for_swapping(const hpix_resolution_t * resolution,
		    size_t * num_of_elements)
{
    assert(resolution != NULL);
    assert(num_of_elements != NULL);

    *num_of_elements = swap_clen[resolution->order];

    size_t offset = 0;
    for (size_t m = 0; m < resolution->order; ++m) 
	offset += swap_clen[m];

    return &swap_cycle[0] + offset;
}

/**********************************************************************/


typedef hpix_pixel_num_t conversion_fn_t(const hpix_resolution_t * resolution,
					 hpix_pixel_num_t ring_index);

void
hpix_switch_order(hpix_map_t * map)
{
    conversion_fn_t * conversion_fn;
    assert(map);

    if(map->scheme == HPIX_ORDER_SCHEME_RING)
	conversion_fn = hpix_ring_to_nest_idx;
    else
	conversion_fn = hpix_nest_to_ring_idx;

    size_t num_of_cycles;
    const int *restrict array_of_cycles = 
	cycles_for_swapping(&map->resolution, &num_of_cycles);
    for(size_t m = 0; m < num_of_cycles; ++m)
    {
        hpix_pixel_num_t istart = array_of_cycles[m];
	printf("New cycle: istart = %d\n", istart);
        double pixbuf = map->pixels[istart];
        hpix_pixel_num_t iold = istart;
	hpix_pixel_num_t inew = conversion_fn(&map->resolution, istart);
        while (inew != istart)
	{
	    printf("  iold = %d, inew = %d\n", iold, inew);
	    map->pixels[iold] = map->pixels[inew];
	    iold = inew;
	    inew = conversion_fn(&map->resolution, inew);
	}
        map->pixels[iold] = pixbuf;
    }

    if(map->scheme == HPIX_ORDER_SCHEME_RING)
	map->scheme = HPIX_ORDER_SCHEME_NEST;
    else
	map->scheme = HPIX_ORDER_SCHEME_RING;
}
