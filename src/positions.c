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

#include "constants.h"

#include "xy2pix.c"
#include "pix2xy.c"

#define NORMALIZE_ANGLE(x)					\
    {								\
	while((x) >= 2.0 * M_PI) (x) = (x) - 2.0 * M_PI;	\
	while((x) <  0.)         (x) = (x) + 2.0 * M_PI;	\
    }

/**********************************************************************/


void
hpix_angles_to_vector(double theta, double phi,
		     hpix_vector_t * vector)
{
    assert(vector);

    const double sin_theta = sin(theta);
    vector->x = sin_theta * cos(phi);
    vector->y = sin_theta * sin(phi);
    vector->z = cos(theta);
}

/**********************************************************************/


hpix_pixel_num_t
hpix_angles_to_ring_pixel(hpix_nside_t nside,
			  double theta,
			  double phi)
{
    int jp, jm, ipix1;
    int ir, ip;

    int nl2 = 2*nside;
    int nl4 = 4*nside;
    int ncap  = nl2*(nside-1);

    double z = cos(theta);
    double z_abs = fabs(z);
    NORMALIZE_ANGLE(phi);
    double tt = phi / (0.5 * M_PI);

    if(z_abs <= 2./3.)
    {
	jp = (int) floor(nside * (0.5 + tt - z*0.75));
	jm = (int) floor(nside * (0.5 + tt + z*0.75));

	ir = nside + 1 + jp - jm;
	int kshift = 0;
	if(fmod(ir, 2) == 0.)
	    kshift = 1;

	ip = (int) floor((jp + jm - nside + kshift + 1) / 2) + 1;
	if(ip > nl4)
	    ip = ip - nl4;

	ipix1 = ncap + nl4 * (ir - 1) + ip;
    } else {
	double tp = tt - floor(tt);
	double tmp = sqrt(3. * (1. - z_abs));

	jp = (int) floor(nside * tp * tmp );
	jm = (int) floor(nside * (1. - tp) * tmp);

	ir = jp + jm + 1;
	ip = (int) floor(tt * ir) + 1;
	if(ip > 4 * ir)
	    ip -= 4 * ir;

	ipix1 = 2 * ir * (ir - 1) + ip;
	if (z <= 0.)
	{
	    ipix1 = 12 * nside * nside - 2 * ir * (ir + 1) + ip;
	}
    }
    return ipix1 - 1;
}

/**********************************************************************/


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

/**********************************************************************/


void
hpix_vector_to_angles(const hpix_vector_t * vector,
		     double * theta, double * phi)
{
    assert(vector);
    assert(theta && phi);

    double vector_len = hpix_vector_length(vector);
    *theta = acos(vector->z / vector_len);
    *phi = atan2(vector->y, vector->x);
    NORMALIZE_ANGLE(*phi);
}

/**********************************************************************/


hpix_pixel_num_t
hpix_vector_to_ring_pixel(hpix_nside_t nside,
			 const hpix_vector_t * vector)
{
    double theta, phi;
    hpix_vector_to_angles(vector, &theta, &phi);
    return hpix_angles_to_ring_pixel(nside, theta, phi);
}

/**********************************************************************/


hpix_pixel_num_t
hpix_vector_to_nest_pixel(hpix_nside_t nside,
			 const hpix_vector_t * vector)
{
    double theta, phi;
    hpix_vector_to_angles(vector, &theta, &phi);
    return hpix_angles_to_nest_pixel(nside, theta, phi);
}

/**********************************************************************/


void
hpix_ring_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
			  double * theta, double * phi)
{
    assert(theta && phi);

    int nl2, nl4, ncap, iring, iphi, ip, ipix1;
    double  fact1, fact2, fodd, hip, fihip;

    ipix1 = pixel + 1; // in {1, npix}
    nl2 = 2*nside;
    nl4 = 4*nside;
    ncap = 2*nside*(nside-1);
    fact1 = 1.5*nside;
    fact2 = 3.0*nside*nside;

    if(ipix1 <= ncap)
    {
        //! North Polar cap -------------
	hip   = ipix1 / 2.;
	fihip = floor(hip);
	iring = (int)floor(sqrt(hip - sqrt(fihip))) + 1; // counted from North pole
	iphi  = ipix1 - 2*iring*(iring - 1);

	*theta = acos( 1. - iring*iring / fact2 );
	*phi   = (1.*iphi - 0.5) * M_PI/(2.*iring);
    }
    else if(ipix1 <= nl2 * (5 * nside + 1))
    {//then ! Equatorial region ------

	ip = ipix1 - ncap - 1;
	iring = (int) floor(ip / nl4) + nside; // counted from North pole
	iphi = (int) fmod(ip, nl4) + 1;

        // 1 if iring + nside is odd, 1/2 otherwise
	fodd  = 0.5 * (1 + fmod((double) (iring + nside), 2)); 
	*theta = acos( (nl2 - iring) / fact1 );
	*phi = (1.*iphi - fodd) * M_PI /(2.*nside);
    }
    else {//! South Polar cap -----------------------------------

	ip = 12 * nside * nside - ipix1 + 1;
	hip = ip / 2.;
/* bug corrige floor instead of 1.* */
	fihip = floor(hip);
	iring = (int)floor( sqrt( hip - sqrt(fihip) ) ) + 1;//     ! counted from South pole
	iphi = (int)(4. * iring + 1 - (ip - 2. * iring * (iring - 1)));

	*theta = acos(-1. + iring * iring / fact2);
	*phi = (1. * iphi - 0.5) * M_PI / (2. * iring);
    }
}

/**********************************************************************/


void
hpix_nest_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
			  double * theta, double * phi)
{
    assert(theta && phi);

    int ix, iy, jrt, jr, nr, jpt, jp, kshift;
    double z;
    double piover2=0.5*M_PI;

    int jrll[12] = { 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
    int jpll[12] = { 1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7 };

    double fn = 1.*nside;
    double fact1 = 1./(3.*fn*fn);
    double fact2 = 2./(3.*fn);
    int nl4 = 4*nside;

    int npface = nside*nside;

    int face_num = pixel/npface; // face number in {0,11}
    int ipf = (int)fmod(pixel,npface); // pixel number in the face {0,npface-1}

    int ip_low = (int) fmod(ipf,1024); // content of the last 10 bits
    int ip_trunc = ipf/1024; // truncation of the last 10 bits
    int ip_med = (int) fmod(ip_trunc,1024); // content of the next 10 bits
    int ip_hi  = ip_trunc/1024; // content of the high weight 10 bits

    ix = 1024 * pix2x[ip_hi] + 32 * pix2x[ip_med] + pix2x[ip_low];
    iy = 1024 * pix2y[ip_hi] + 32 * pix2y[ip_med] + pix2y[ip_low];

    // Transforms this in (horizontal, vertical) coordinates
    jrt = ix + iy; // 'vertical' in {0,2*(nside-1)}
    jpt = ix - iy; // 'horizontal' in {-nside+1,nside-1}

    jr =  jrll[face_num]*nside - jrt - 1;
    nr = nside; // Equatorial region (the most frequent)
    z  = (2*nside-jr)*fact2;
    kshift = (int)fmod(jr - nside, 2);
    if(jr < nside)
    {
	nr = jr;
	z = 1. - nr*nr*fact1;
	kshift = 0;
    } else {
	if(jr > 3 * nside)
	{
	    nr = nl4 - jr;
	    z = - 1. + nr * nr * fact1;
	    kshift = 0;
	}
    }
    *theta = acos(z);

    jp = (jpll[face_num] * nr + jpt + 1 + kshift) / 2;
    if(jp > nl4) jp = jp - nl4;
    if(jp < 1)   jp = jp + nl4;

    *phi = (jp - (kshift+1)*0.5) * (piover2 / nr);
}

/**********************************************************************/


void
hpix_ring_pixel_to_vector(hpix_nside_t nside,
			 hpix_pixel_num_t pixel_index,
			 hpix_vector_t * vector)
{
    assert(vector);

    double theta, phi;
    hpix_ring_pixel_to_angles(nside, pixel_index, &theta, &phi);
    hpix_angles_to_vector(theta, phi, vector);
}

/**********************************************************************/


void
hpix_nest_pixel_to_vector(hpix_nside_t nside,
			 hpix_pixel_num_t pixel_index,
			 hpix_vector_t * vector)
{
    assert(vector);

    double theta, phi;
    hpix_nest_pixel_to_angles(nside, pixel_index, &theta, &phi);
    hpix_angles_to_vector(theta, phi, vector);
}
