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
#include <stdlib.h>
#include <math.h>

unsigned int hpix_ilog2 (const unsigned int argument)
{
    unsigned int result = 0;
    unsigned int shifted_argument = argument;

    while(shifted_argument > 0x0000FFFF) 
    { 
	result += 16;
	shifted_argument >>= 16; 
    }

    if(shifted_argument > 0x000000FF) 
    { 
	result |= 8;
	shifted_argument >>= 8;
    }

    if(shifted_argument > 0x0000000F)
    {
	result|=4;
	shifted_argument>>=4;
    }

    if(shifted_argument > 0x00000003)
    {
	result |= 2;
	shifted_argument>>=2;
    }

    if(shifted_argument > 0x00000001)
	result |= 1;

    return result;
}

unsigned short hpix_isqrt(unsigned long argument)
{
    unsigned short result = sqrt(argument + 0.5);
    if (result * result > argument)
      result--;
    else if ((result + 1) * (result + 1) <= argument)
      result++;
    return result;
}
