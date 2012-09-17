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

#include <hpxlib.h>
#include <assert.h>
#include <math.h>

#include "xy2pix.c"
#include "pix2xy.c"

static const int jrll[12] = { 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
static const int jpll[12] = { 1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7 };


hpxlib_pixel_num_t
hpxlib_nest_to_ring_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t nest_index)
{
    int npix, npface, face_num, ncap, n_before;
    int ipf, ip_low, ip_trunc, ip_med, ip_hi;
    int ix, iy, jrt, jr, nr, jpt, jp, kshift, nl4;

    npix = hpxlib_nside_to_npixel(nside);
    ncap = 2 * nside * (nside - 1);
    nl4 = 4 * nside;
    npface = nside * nside;
    face_num = nest_index / npface;
    ipf = (int) fmod(nest_index, npface);
    ip_low = (int) fmod(ipf, 1024);
    ip_trunc = ipf/1024;
    ip_med = (int) fmod(ip_trunc, 1024);
    ip_hi = ip_trunc/1024;
    ix = 1024 * pix2x[ip_hi] + 32 * pix2x[ip_med] + pix2x[ip_low];
    iy = 1024 * pix2y[ip_hi] + 32 * pix2y[ip_med] + pix2y[ip_low];
    jrt = ix + iy;
    jpt = ix - iy;
    jr =  jrll[face_num] * nside - jrt - 1;
    nr = nside;
    n_before = ncap + nl4 * (jr - nside);
    kshift = (int)fmod(jr - nside, 2);

    if(jr < nside)
    {
	nr = jr;
	n_before = 2 * nr * (nr - 1);
	kshift = 0;
    } else if(jr > 3 * nside)
    {
	nr = nl4 - jr;
	n_before = npix - 2 * (nr + 1) * nr;
	kshift = 0;
    }

    jp = (jpll[face_num] * nr + jpt + 1 + kshift) / 2;

    if(jp > nl4) jp = jp - nl4;
    if(jp < 1)   jp = jp + nl4;

    return n_before + jp - 1;
}

hpxlib_pixel_num_t
hpxlib_ring_to_nest_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t ring_index)
{
    double fihip, hip;
    int npix, nl2, nl4, ncap, ip, iphi, ipt, ring_index1;
    int kshift, face_num, nr;
    int irn, ire, irm, irs, irt, ifm , ifp;
    int ix, iy, ix_low, ix_hi, iy_low, iy_hi, ipf;

    nl2 = 2 * nside;
    nl4 = 4 * nside;
    npix = hpxlib_nside_to_npixel(nside);
    ncap = 2 * nside * (nside - 1);
    ring_index1 = ring_index + 1;

    if(ring_index1 <= ncap)
    {
	hip = ring_index1 / 2.;
	fihip = (int) floor(hip);
	irn = (int) floor(sqrt(hip - sqrt(fihip))) + 1;
	iphi = ring_index1 - 2 * irn * (irn - 1);
    	kshift = 0;
	nr = irn;
	face_num = (iphi - 1) / irn;
    }
    else if(ring_index1 <= nl2 * (5 * nside + 1))
    {
	ip = ring_index1 - ncap - 1;
	irn = (int) floor(ip / nl4) + nside;
	iphi = (int) fmod(ip, nl4) + 1;
    	kshift = (int) fmod(irn + nside, 2);
	nr = nside;
	ire = irn - nside + 1;
	irm = nl2 + 2 - ire;
	ifm = (iphi - ire / 2 + nside -1) / nside;
	ifp = (iphi - irm / 2 + nside -1) / nside;
	if(ifp == ifm)
	    face_num = (int) fmod(ifp, 4) + 4;
	else if(ifp + 1 == ifm)
	    face_num = ifp;
	else if(ifp - 1 == ifm)
	    face_num = ifp + 7;
    }
    else {
	ip = npix - ring_index1 + 1;
	hip = ip / 2.;
	fihip = floor(hip);
	irs = (int) floor(sqrt(hip - sqrt(fihip))) + 1;
	iphi = 4 * irs + 1 - (ip - 2 * irs * (irs - 1));

	kshift = 0;
	nr = irs;
	irn = nl4 - irs;
	face_num = (iphi - 1) / irs + 8;
    }

    irt =   irn  - jrll[face_num]*nside + 1;
    ipt = 2*iphi - jpll[face_num]*nr - kshift - 1;

    if(ipt >= nl2)
	ipt = ipt - 8 * nside;

    ix =  (ipt - irt) / 2;
    iy = -(ipt + irt) / 2;

    ix_low = (int) fmod(ix, 128);
    ix_hi  = ix / 128;
    iy_low = (int) fmod(iy, 128);
    iy_hi  = iy / 128;
    ipf = (x2pix[ix_hi] + y2pix[iy_hi]) * (128 * 128)
	+ (x2pix[ix_low] + y2pix[iy_low]);

    return ipf + face_num * nside * nside;
}

typedef hpxlib_pixel_num_t conversion_fn_t(hpxlib_nside_t nside,
					   hpxlib_pixel_num_t ring_index);

void
hpxlib_switch_order(hpxlib_map_t * map)
{
    conversion_fn_t * conversion_fn;
    assert(map);

    assert(1 == 2); /* Continue from here */
}
