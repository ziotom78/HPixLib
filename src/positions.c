/* positions.c -- functions to map positions in the sky sphere with pixels
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

#define NORMALIZE_ANGLE(x)					\
    {								\
	if(x >= 2.0 * M_PI) x = x - 2.0 * M_PI;			\
	if(x <  0.)         x = x + 2.0 * M_PI;			\
    }

void
hpix_angles_to_3dvec(double theta, double phi,
		     double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

hpix_pixel_num_t
hpix_angles_to_ring_pixel(hpix_nside_t nside,
			  double theta,
			  double phi)
{
    int nl2, nl4, ncap, npix;
    int jp, jm, ipix1;
    double z, z_abs, tt, tp, tmp;
    int ir, ip, kshift;

    nl2 = 2*nside;
    nl4 = 4*nside;
    ncap  = nl2*(nside-1);
    npix  = 12*nside*nside;

    z = cos(theta);
    z_abs = fabs(z);
    NORMALIZE_ANGLE(phi);
    tt = phi / (0.5 * M_PI);

    if(z_abs <= 2./3.)
    {
	jp = (int)floor(nside * (0.5 + tt - z*0.75));
	jm = (int)floor(nside * (0.5 + tt + z*0.75));

	ir = nside + 1 + jp - jm;
	kshift = 0;
	if(fmod(ir, 2) == 0.)
	    kshift = 1;

	ip = (int) floor((jp + jm - nside + kshift + 1) / 2) + 1;
	if(ip > nl4)
	    ip = ip - nl4;

	ipix1 = ncap + nl4 * (ir - 1) + ip;
    } else {
	tp = tt - floor(tt);
	tmp = sqrt(3. * (1. - z_abs));

	jp = (int) floor(nside * tp * tmp );
	jm = (int) floor(nside * (1. - tp) * tmp);

	ir = jp + jm + 1;
	ip = (int) floor(tt * ir) + 1;
	if(ip > 4 * ir)
	    ip = ip - 4 * ir;

	ipix1 = 2 * ir * (ir - 1) + ip;
	if (z <= 0.)
	{
	    ipix1 = npix - 2*ir*(ir+1) + ip;
	}
    }
    return ipix1 - 1;
}

hpix_pixel_num_t
hpix_angles_to_nest_pixel(hpix_nside_t nside,
			  double theta,
			  double phi)
{
    double z, z_abs, tt, tp, tmp;
    int    face_num,jp,jm;
    long   ifp, ifm;
    int    ix, iy, ix_low, ix_hi, iy_low, iy_hi, ipf, ntt;
    int    ns_max = 8192;

    z  = cos(theta);
    z_abs = fabs(z);
    NORMALIZE_ANGLE(phi);
    tt = phi / (0.5 * M_PI); /* in [0,4[ */

    if(z_abs <= 2./3.)
    {
	jp = (int) floor(ns_max*(0.5 + tt - z*0.75));
	jm = (int) floor(ns_max*(0.5 + tt + z*0.75));

	ifp = jp / ns_max; /* in {0,4} */
	ifm = jm / ns_max;

	if(ifp == ifm) face_num = (int) fmod(ifp, 4) + 4;
	else if(ifp < ifm) face_num = (int) fmod(ifp, 4);
	else face_num = (int) fmod(ifm, 4) + 8;

	ix = (int) fmod(jm, ns_max);
	iy = ns_max - (int) fmod(jp, ns_max) - 1;
    }
    else { /* polar region, z_abs > 2/3 */

	ntt = (int)floor(tt);
	if( ntt>=4 ) ntt = 3;
	tp = tt - ntt;
	tmp = sqrt( 3.*(1. - z_abs) ); /* in ]0,1] */

	jp = (int)floor( ns_max * tp * tmp );

	jm = (int)floor( ns_max * (1. - tp) * tmp );
	jp = (int)(jp < ns_max-1 ? jp : ns_max-1);
	jm = (int)(jm < ns_max-1 ? jm : ns_max-1);

	if( z>=0 ) {
	    face_num = ntt; /* in {0,3} */
	    ix = ns_max - jm - 1;
	    iy = ns_max - jp - 1;
	}
	else {
	    face_num = ntt + 8; /* in {8,11} */
	    ix =  jp;
	    iy =  jm;
	}
    }

    ix_low = (int)fmod(ix, 128);
    ix_hi  = ix / 128;
    iy_low = (int)fmod(iy, 128);
    iy_hi  = iy / 128;

    ipf = (x2pix[ix_hi]  + y2pix[iy_hi]) * (128 * 128)
	+ (x2pix[ix_low] + y2pix[iy_low]);
    ipf = (long) (ipf / pow(ns_max / nside, 2));
    return (hpix_pixel_num_t) (ipf + face_num * nside * nside);
}

void
hpix_3dvec_to_angles(double x, double y, double z,
		     double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

hpix_pixel_num_t
hpix_3dvec_to_ring_pixel(hpix_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

hpix_pixel_num_t
hpix_3dvec_to_nest_pixel(hpix_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

void
hpix_ring_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
			  double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

void
hpix_nest_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
			  double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

void
hpix_ring_pixel_to_3dvec(hpix_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

void
hpix_nest_pixel_to_3dvec(hpix_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}
