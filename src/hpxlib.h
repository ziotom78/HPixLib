/* hpxlib.h -- header file for the HPixLib library
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

#ifndef HPXLIB_H
#define HPXLIB_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "fitsio.h"

#define CHEALPIX_VERSION "0.1"

typedef unsigned short hpxlib_nside_t;
typedef unsigned long hpxlib_pixel_num_t;

typedef enum {
    HPXLIB_ORDER_RING,
    HPXLIB_ORDER_NEST
} hpxlib_ordering_t;

typedef enum {
    HPXLIB_COORD_CUSTOM,
    HPXLIB_COORD_ECLIPTIC,
    HPXLIB_COORD_GALACTIC,
    HPXLIB_COORD_CELESTIAL
} hpxlib_coordinates_t;

typedef struct {
    hpxlib_ordering_t    order;
    hpxlib_coordinates_t coord;
    hpxlib_nside_t       nside;
    double           * pixels;
} hpxlib_map_t;

#define HPXLIB_MAP_PIXEL(map, index)				\
    (*((double *) (((char *) map->pixels)			\
		   + (index) * sizeof(map->pixels[0]))))

struct ___hpxlib_bmp_projection_t;
typedef struct ___hpxlib_bmp_projection_t hpxlib_bmp_projection_t;

/* Functions implemented in mem.c */

void * hpxlib_malloc(size_t size, size_t num);
void * hpxlib_calloc(size_t size, size_t num);
void * hpxlib_realloc(void * ptr, size_t size);
void hpxlib_free(void * ptr);

/* Functions implemented in misc.c */

hpxlib_pixel_num_t hpxlib_nside_to_npixel(hpxlib_nside_t);
hpxlib_nside_t hpxlib_npixel_to_nside(hpxlib_pixel_num_t);
double hpxlib_max_pixel_radius(hpxlib_nside_t);

/* Functions implemented in map.c */

hpxlib_map_t * hpxlib_create_map(hpxlib_nside_t nside, hpxlib_ordering_t order);

hpxlib_map_t * hpxlib_create_map_from_array(double * array,
					    size_t num_of_elements,
					    hpxlib_ordering_t order);

void hpxlib_free_map(hpxlib_map_t * map);

hpxlib_ordering_t hpxlib_map_ordering(const hpxlib_map_t * map);

hpxlib_coordinates_t hpxlib_map_coordinate_system(const hpxlib_map_t * map);

hpxlib_nside_t hpxlib_map_nside(const hpxlib_map_t * map);

double * hpxlib_map_pixels(const hpxlib_map_t * map);

size_t hpxlib_map_num_of_pixels(const hpxlib_map_t * map);

/* Functions implemented in io.c */

int
hpxlib_load_component_from_fitsptr(fitsfile * fptr,
				   unsigned short column_number,
				   hpxlib_map_t ** map,
				   int * status);

int hpxlib_load_component_from_file(const char * file_name,
				    unsigned short column_number,
				    hpxlib_map_t ** map,
				    int * status);

int
hpxlib_create_empty_fits_table_for_map(fitsfile * fptr,
				       const hpxlib_map_t * template_map,
				       unsigned short num_of_components,
				       const char * measure_unit,
				       int * status);

int
hpxlib_save_fits_component_to_fitsfile(fitsfile * fptr,
				       const hpxlib_map_t * map,
				       int data_type,
				       const char * measure_unit,
				       int * status);

int
hpxlib_save_fits_component_to_file(const char * file_name,
				   const hpxlib_map_t * map,
				   int data_type,
				   const char * measure_unit,
				   int * status);

int
hpxlib_load_fits_pol_from_fitsfile(fitsfile * fptr,
				   hpxlib_map_t ** map_i,
				   hpxlib_map_t ** map_q,
				   hpxlib_map_t ** map_u,
				   int * status);

int
hpxlib_load_fits_pol_from_file(const char * file_name,
			       hpxlib_map_t ** map_i,
			       hpxlib_map_t ** map_q,
			       hpxlib_map_t ** map_u,
			       int * status);

int
hpxlib_save_fits_pol_fitsfile(const char * file_name,
			      const hpxlib_map_t * map_i,
			      const hpxlib_map_t * map_q,
			      const hpxlib_map_t * map_u,
			      int data_type,
			      const char * measure_unit,
			      int * status);

int
hpxlib_save_fits_pol_map(const char * file_name,
			 const hpxlib_map_t * map_i,
			 const hpxlib_map_t * map_q,
			 const hpxlib_map_t * map_u,
			 int data_type,
			 const char * measure_unit,
			 int * status);

/* Functions implemented in positions.c */

void hpxlib_angles_to_3dvec(double theta, double phi,
			    double * x, double * y, double * z);

typedef hpxlib_pixel_num_t hpxlib_angles_to_pixel_fn_t(hpxlib_nside_t,
						       double, double);

hpxlib_pixel_num_t hpxlib_angles_to_ring_pixel(hpxlib_nside_t nside,
					       double theta,
					       double phi);

hpxlib_pixel_num_t hpxlib_angles_to_nest_pixel(hpxlib_nside_t nside,
					       double theta,
					       double phi);

void hpxlib_3dvec_to_angles(double x, double y, double z,
			    double * theta, double * phi);

typedef hpxlib_pixel_num_t hpxlib_3dvec_to_pixel_fn_t(hpxlib_nside_t,
						      double, double, double);

hpxlib_pixel_num_t hpxlib_3dvec_to_ring_pixel(hpxlib_nside_t nside,
					      double x, double y, double z);

hpxlib_pixel_num_t hpxlib_3dvec_to_nest_pixel(hpxlib_nside_t nside,
					      double x, double y, double z);

typedef void hpxlib_pixel_to_angles(hpxlib_nside_t, hpxlib_pixel_num_t,
				    double *, double *);

void hpxlib_ring_pixel_to_angles(hpxlib_nside_t nside, hpxlib_pixel_num_t pixel,
				 double * theta, double * phi);

void hpxlib_nest_pixel_to_angles(hpxlib_nside_t nside, hpxlib_pixel_num_t pixel,
				 double * theta, double * phi);

typedef void hpxlib_pixel_to_3dvec(hpxlib_nside_t, hpxlib_pixel_num_t,
				   double *, double *, double *);

void hpxlib_ring_pixel_to_3dvec(hpxlib_nside_t nside,
				double * x, double * y, double * z);

void hpxlib_nest_pixel_to_3dvec(hpxlib_nside_t nside,
				double * x, double * y, double * z);

/* Functions implemented in bitmap.c */

hpxlib_bmp_projection_t * 
hpxlib_create_bmp_projection(unsigned int width, unsigned int height);

void
hpxlib_free_bmp_projection(hpxlib_bmp_projection_t * proj);

unsigned int
hpxlib_bmp_projection_width(const hpxlib_bmp_projection_t * proj);

unsigned int
hpxlib_bmp_projection_height(const hpxlib_bmp_projection_t * proj);

void
hpxlib_set_bmp_projection_width(hpxlib_bmp_projection_t * proj,
				unsigned int width);

void
hpxlib_set_bmp_projection_height(hpxlib_bmp_projection_t * proj,
				 unsigned int height);

double *
hpxlib_bmp_trace_bitmap(const hpxlib_bmp_projection_t * proj,
			const hpxlib_map_t * map,
			double * min_value,
			double * max_value);

/* Functions implemented in order_conversion.c */

hpxlib_pixel_num_t
hpxlib_nest_to_ring_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t nest_index);

hpxlib_pixel_num_t
hpxlib_ring_to_nest_idx(hpxlib_nside_t nside, hpxlib_pixel_num_t ring_index);

void
hpxlib_switch_order(hpxlib_map_t * map);

/* Functions implemented in query_disc.c */

void hpxlib_query_disc(double theta, double phi, double radius,
		       hpxlib_pixel_num_t ** pixels,
		       size_t * num_of_matches);

void hpxlib_query_disc_inclusive(double theta, double phi, double radius,
				 hpxlib_pixel_num_t ** pixels,
				 size_t * num_of_matches);


#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
