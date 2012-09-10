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

#include "chpx.h"
#include <assert.h>
#include <math.h>

/* Vectors `x2pix` and `y2pix` are used by `ang2pix_nest`. */

const int x2pix[] = {
       0,     1,     4,     5,    16,    17,    20,    21,    64,    65,
      68,    69,    80,    81,    84,    85,   256,   257,   260,   261,
     272,   273,   276,   277,   320,   321,   324,   325,   336,   337,
     340,   341,  1024,  1025,  1028,  1029,  1040,  1041,  1044,  1045,
    1088,  1089,  1092,  1093,  1104,  1105,  1108,  1109,  1280,  1281,
    1284,  1285,  1296,  1297,  1300,  1301,  1344,  1345,  1348,  1349,
    1360,  1361,  1364,  1365,  4096,  4097,  4100,  4101,  4112,  4113,
    4116,  4117,  4160,  4161,  4164,  4165,  4176,  4177,  4180,  4181,
    4352,  4353,  4356,  4357,  4368,  4369,  4372,  4373,  4416,  4417,
    4420,  4421,  4432,  4433,  4436,  4437,  5120,  5121,  5124,  5125,
    5136,  5137,  5140,  5141,  5184,  5185,  5188,  5189,  5200,  5201,
    5204,  5205,  5376,  5377,  5380,  5381,  5392,  5393,  5396,  5397,
    5440,  5441,  5444,  5445,  5456,  5457,  5460,  5461, };

const int y2pix[] = {
        0,     2,     8,    10,    32,    34,    40,    42,   128,   130,
      136,   138,   160,   162,   168,   170,   512,   514,   520,   522,
      544,   546,   552,   554,   640,   642,   648,   650,   672,   674,
      680,   682,  2048,  2050,  2056,  2058,  2080,  2082,  2088,  2090,
     2176,  2178,  2184,  2186,  2208,  2210,  2216,  2218,  2560,  2562,
     2568,  2570,  2592,  2594,  2600,  2602,  2688,  2690,  2696,  2698,
     2720,  2722,  2728,  2730,  8192,  8194,  8200,  8202,  8224,  8226,
     8232,  8234,  8320,  8322,  8328,  8330,  8352,  8354,  8360,  8362,
     8704,  8706,  8712,  8714,  8736,  8738,  8744,  8746,  8832,  8834,
     8840,  8842,  8864,  8866,  8872,  8874, 10240, 10242, 10248, 10250,
    10272, 10274, 10280, 10282, 10368, 10370, 10376, 10378, 10400, 10402,
    10408, 10410, 10752, 10754, 10760, 10762, 10784, 10786, 10792, 10794,
    10880, 10882, 10888, 10890, 10912, 10914, 10920, 10922, };

#define NORMALIZE_ANGLE(x)					\
    {								\
	if(x >= 2.0 * M_PI) x = x - 2.0 * M_PI;			\
	if(x <  0.)         x = x + 2.0 * M_PI;			\
    }

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void
chpx_angles_to_3dvec(double theta, double phi,
		     double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

chpx_pixel_num_t
chpx_angles_to_ring_pixel(chpx_nside_t nside,
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

chpx_pixel_num_t
chpx_angles_to_nest_pixel(chpx_nside_t nside,
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
    return (chpx_pixel_num_t) (ipf + face_num * nside * nside);
}

void
chpx_3dvec_to_angle(double x, double y, double z,
		    double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

chpx_pixel_num_t
chpx_3dvec_to_ring_pixel(chpx_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

chpx_pixel_num_t
chpx_3dvec_to_nest_pixel(chpx_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

void
chpx_ring_pixel_to_angle(chpx_nside_t nside, chpx_pixel_num_t pixel,
			 double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

void
chpx_nest_pixel_to_angle(chpx_nside_t nside, chpx_pixel_num_t pixel,
			 double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

void
chpx_ring_pixel_to_3dvec(chpx_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

void
chpx_nest_pixel_to_3dvec(chpx_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}
