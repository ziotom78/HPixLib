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

#ifndef CHEALPIX_H
#define CHEALPIX_H

#ifdef __cplusplus
extern"C"{
#endif /* __cplusplus */

#include "fitsio.h"

#define CHEALPIX_VERSION "0.1"

typedef unsigned short chpx_nside_t;
typedef unsigned long chpx_pixel_num_t;

typedef enum {
    CHPX_ORDER_UNKNOWN,
    CHPX_ORDER_RING,
    CHPX_ODER_NEST
} chpx_ordering_t;

typedef enum {
    CHPX_COORD_UNKNOWN,
    CHPX_COORD_ECLIPTIC,
    CHPX_COORD_GALACTIC
} chpx_coordinates_t;

typedef struct {
    chpx_ordering_t    order;
    chpx_coordinates_t coord;
    chpx_nside_t       nside;
    size_t             pixel_size;
    void               * pixels;
} chpx_map_t;

#define CHPX_MAP_PIXEL(map, index, type)	\
    (*((type *) (((char *) map->pixels)		\
		 + (index) * map->pixel_size)))

typedef struct {
    unsigned int       width;
    unsigned int       height;
    chpx_coordinates_t coordsys;
} chpx_bmp_projection_t;

/* Functions implemented in mem.c */

void * chpx_malloc(size_t size, size_t num);
void * chpx_calloc(size_t size, size_t num);
void * chpx_realloc(void * ptr, size_t size);
void chpx_free(void * ptr);

/* Functions implemented in misc.c */

chpx_pixel_num_t chpx_nside_to_npixel(chpx_nside_t);
chpx_nside_t chpx_npixel_to_nside(chpx_pixel_num_t);
double chpx_max_pixel_radius(chpx_nside_t);

/* Functions implemented in map.c */

chpx_map_t * chpx_create_map(chpx_nside_t,
			     size_t pixel_size);

chpx_map_t * chpx_create_map_from_array(void * array,
					size_t num_of_elements,
					size_t pixel_size);

void chpx_free_map(chpx_map_t * map);

chpx_ordering_t chpx_map_ordering(const chpx_map_t * map);

chpx_coordinates_t chpx_map_coordinate_system(const chpx_map_t * map);

chpx_nside_t chpx_map_nside(const chpx_map_t * map);

size_t chpx_map_pixel_size(const chpx_map_t * map);

void * chpx_map_pixels(const chpx_map_t * map);

size_t chpx_num_of_pixels(const chpx_map_t * map);

/* Functions implemented in io.c */

chpx_map_t * chpx_load_fits_map(const char * file_name,
				unsigned short hdu_number,
				char ** error_status);

int chpx_save_fits_map(const char * file_name,
		       const chpx_map_t * map,
		       int data_type,
		       char ** error_status);

int chpx_load_fits_pol_map(const char * file_name,
			   unsigned short hdu_number,
			   chpx_map_t ** map_i,
			   chpx_map_t ** map_q,
			   chpx_map_t ** map_u,
			   char ** error_status);

int chpx_save_fits_pol_map(const char * file_name,
			   const chpx_map_t * map_i,
			   const chpx_map_t * map_q,
			   const chpx_map_t * map_u,
			   int data_type,
			   char ** error_status);

/* Functions implemented in positions.c */

void chpx_angles_to_3dvec(double theta, double phi,
			  double * x, double * y, double * z);

chpx_pixel_num_t chpx_angles_to_ring_pixel(chpx_nside_t nside,
					   double theta,
					   double phi);

chpx_pixel_num_t chpx_angles_to_nest_pixel(chpx_nside_t nside,
					   double theta,
					   double phi);

void chpx_3dvec_to_angle(double x, double y, double z,
			 double * theta, double * phi);

chpx_pixel_num_t chpx_3dvec_to_ring_pixel(chpx_nside_t nside,
					  double x, double y, double z);

chpx_pixel_num_t chpx_3dvec_to_nest_pixel(chpx_nside_t nside,
					  double x, double y, double z);

void chpx_ring_pixel_to_angle(chpx_nside_t nside, chpx_pixel_num_t pixel,
			      double * theta, double * phi);

void chpx_nest_pixel_to_angle(chpx_nside_t nside, chpx_pixel_num_t pixel,
			      double * theta, double * phi);

void chpx_ring_pixel_to_3dvec(chpx_nside_t nside,
			      double * x, double * y, double * z);

void chpx_nest_pixel_to_3dvec(chpx_nside_t nside,
			      double * x, double * y, double * z);

/* Functions implemented in bitmap.c */

double *
chpx_bmp_trace_bitmap(const chpx_bmp_projection_t * proj,
		      const chpx_map_t * map,
		      double * min_value,
		      double * max_value);

/* Functions implemented in order_conversion.c */

chpx_pixel_num_t
chpx_nest_to_ring_idx(chpx_nside_t nside, chpx_pixel_num_t nest_index);

chpx_pixel_num_t
chpx_ring_to_nest_idx(chpx_nside_t nside, chpx_pixel_num_t ring_index);

void
chpx_switch_order(chpx_map_t * map);

/* Functions implemented in query_disc.c */

void chpx_query_disc(double theta, double phi, double radius,
		     chpx_pixel_num_t ** pixels,
		     size_t * num_of_matches);

void chpx_query_disc_inclusive(double theta, double phi, double radius,
			       chpx_pixel_num_t ** pixels,
			       size_t * num_of_matches);


#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
