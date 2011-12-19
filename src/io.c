/* chpx.h -- header file for the CHealpix library
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

#include <chpx.h>

chpx_map_t *
chpx_load_fits_map(const char * file_name,
		   unsigned short hdu_number,
		   char ** error_status)
{
    return NULL;
}

void
chpx_save_fits_map(const char * file_name,
		   const chpx_map_t * map,
		   char ** error_status)
{
}

int
chpx_load_fits_pol_map(const char * file_name,
		       unsigned short hdu_number,
		       chpx_map_t ** map_i,
		       chpx_map_t ** map_q,
		       chpx_map_t ** map_u,
		       char ** error_status)
{
    return 0;
}

void
chpx_save_fits_pol_map(const char * file_name,
		       const chpx_map_t * map_i,
		       const chpx_map_t * map_q,
		       const chpx_map_t * map_u,
		       char ** error_status)
{
}
