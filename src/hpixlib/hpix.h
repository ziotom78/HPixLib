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

#ifndef HPIX_H
#define HPIX_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <inttypes.h>
#include <stdio.h>
#include "config.h"
#include "fitsio.h"

#define CHEALPIX_VERSION "0.1"

#define HPIX_IS_MASKED(x) (isnan(x) || (x) < -1.6e+30)

typedef unsigned short hpix_nside_t;
typedef uint64_t hpix_pixel_num_t;

typedef enum {
    HPIX_ORDER_SCHEME_RING,
    HPIX_ORDER_SCHEME_NEST
} hpix_ordering_scheme_t;

typedef enum {
    HPIX_COORD_CUSTOM,
    HPIX_COORD_ECLIPTIC,
    HPIX_COORD_GALACTIC,
    HPIX_COORD_CELESTIAL
} hpix_coordinates_t;

typedef struct {
    hpix_nside_t           nside;
    hpix_nside_t           nside_times_two;
    hpix_nside_t           nside_times_four;
    size_t                 num_of_pixels;

    /* The following fields are used to quickly convert between pixel
     * numbers and other representations. */
    unsigned int           order;
    unsigned int           pixels_per_face;
    unsigned int           ncap;
    double                 fact2;
    double                 fact1;
} hpix_resolution_t;

typedef struct {
    hpix_ordering_scheme_t scheme;
    hpix_coordinates_t     coord;
    double               * pixels;
    _Bool                  free_pixels_flag;

    hpix_resolution_t      resolution;
} hpix_map_t;

typedef struct {
    double x;
    double y;
    double z;
} hpix_vector_t;

typedef struct {
    double m[3][3];
} hpix_matrix_t;

/* The most basic structure: a RGB color. Following Cairo's
 * conventions, each component is a floating-point number between 0.0
 * and 1.0. */

typedef struct {
    double red;
    double green;
    double blue;
} hpix_color_t;

typedef struct hpix_color_palette_t hpix_color_palette_t;

#define HPIX_MAP_PIXEL(map, index)				\
    (*((double *) (((char *) map->pixels)			\
		   + (index) * sizeof(map->pixels[0]))))

typedef enum { HPIX_PROJ_NULL, 
	       HPIX_PROJ_MOLLWEIDE, 
	       HPIX_PROJ_EQUIRECTANGULAR }
    hpix_projection_type_t;

struct ___hpix_bmp_projection_t;
typedef struct ___hpix_bmp_projection_t hpix_bmp_projection_t;

/* Functions implemented in math.c */

double hpix_average_pixel_value(const hpix_map_t * map);
void hpix_scale_pixels_by_constant_inplace(hpix_map_t * map, double constant);
void hpix_add_constant_to_pixels_inplace(hpix_map_t * map, double constant);
void hpix_remove_monopole_from_map_inplace(hpix_map_t * map);

/* Functions implemented in mem.c */

void * hpix_malloc(size_t size, size_t num);
void * hpix_calloc(size_t size, size_t num);
void * hpix_realloc(void * ptr, size_t size);
void hpix_free(void * ptr);

/* Functions implemented in misc.c */

_Bool hpix_valid_nside(hpix_nside_t nside);
hpix_pixel_num_t hpix_nside_to_npixel(hpix_nside_t);
hpix_nside_t hpix_npixel_to_nside(hpix_pixel_num_t);
double hpix_max_pixel_radius(hpix_nside_t);

/* Functions implemented in map.c */

hpix_map_t * hpix_create_map(hpix_nside_t nside,
			     hpix_ordering_scheme_t scheme);

hpix_map_t * hpix_create_map_from_array(double * array,
					    size_t num_of_elements,
					    hpix_ordering_scheme_t scheme);

void hpix_free_map(hpix_map_t * map);

hpix_map_t * hpix_create_copy_of_map(const hpix_map_t * map);

hpix_ordering_scheme_t hpix_map_ordering_scheme(const hpix_map_t * map);

hpix_coordinates_t hpix_map_coordinate_system(const hpix_map_t * map);

hpix_nside_t hpix_map_nside(const hpix_map_t * map);

double * hpix_map_pixels(const hpix_map_t * map);

size_t hpix_map_num_of_pixels(const hpix_map_t * map);

const hpix_resolution_t * hpix_map_resolution(const hpix_map_t * map);

/* Functions implemented in integer_functions.c */

unsigned int hpix_ilog2 (const unsigned int argument);
unsigned short hpix_isqrt(unsigned long argument);

/* Functions implemented in io.c */

int
hpix_load_fits_component_from_fitsptr(fitsfile * fptr,
				      unsigned short column_number,
				      hpix_map_t ** map,
				      int * status);

int hpix_load_fits_component_from_file(const char * file_name,
				       unsigned short column_number,
				       hpix_map_t ** map,
				       int * status);

int
hpix_create_empty_fits_table_for_map(fitsfile * fptr,
				       const hpix_map_t * template_map,
				       unsigned short num_of_components,
				       const char * measure_unit,
				       int * status);

int
hpix_save_fits_component_to_fitsfile(fitsfile * fptr,
				       const hpix_map_t * map,
				       int data_type,
				       const char * measure_unit,
				       int * status);

int
hpix_save_fits_component_to_file(const char * file_name,
				   const hpix_map_t * map,
				   int data_type,
				   const char * measure_unit,
				   int * status);

int
hpix_load_fits_pol_from_fitsfile(fitsfile * fptr,
				   hpix_map_t ** map_i,
				   hpix_map_t ** map_q,
				   hpix_map_t ** map_u,
				   int * status);

int
hpix_load_fits_pol_from_file(const char * file_name,
			       hpix_map_t ** map_i,
			       hpix_map_t ** map_q,
			       hpix_map_t ** map_u,
			       int * status);

int
hpix_save_fits_pol_fitsfile(const char * file_name,
			      const hpix_map_t * map_i,
			      const hpix_map_t * map_q,
			      const hpix_map_t * map_u,
			      int data_type,
			      const char * measure_unit,
			      int * status);

int
hpix_save_fits_pol_map(const char * file_name,
			 const hpix_map_t * map_i,
			 const hpix_map_t * map_q,
			 const hpix_map_t * map_u,
			 int data_type,
			 const char * measure_unit,
			 int * status);

/* Functions implemented in positions.c */

void hpix_angles_to_vector(double theta, double phi,
			  hpix_vector_t * vector);

typedef hpix_pixel_num_t hpix_angles_to_pixel_fn_t(hpix_nside_t,
						       double, double);

hpix_pixel_num_t hpix_angles_to_ring_pixel(hpix_nside_t nside,
					       double theta,
					       double phi);

hpix_pixel_num_t hpix_angles_to_nest_pixel(hpix_nside_t nside,
					       double theta,
					       double phi);

void hpix_vector_to_angles(const hpix_vector_t * vector,
			  double * theta, double * phi);

typedef hpix_pixel_num_t hpix_vector_to_pixel_fn_t(hpix_nside_t,
						  const hpix_vector_t *);

hpix_pixel_num_t hpix_vector_to_ring_pixel(hpix_nside_t nside,
					  const hpix_vector_t * vector);

hpix_pixel_num_t hpix_vector_to_nest_pixel(hpix_nside_t nside,
					  const hpix_vector_t * vector);

typedef void hpix_pixel_to_angles(hpix_nside_t, hpix_pixel_num_t,
				    double *, double *);

void hpix_ring_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
				 double * theta, double * phi);

void hpix_nest_pixel_to_angles(hpix_nside_t nside, hpix_pixel_num_t pixel,
				 double * theta, double * phi);

typedef void hpix_pixel_to_vector(hpix_nside_t, hpix_pixel_num_t,
				 hpix_vector_t * vector);

void hpix_ring_pixel_to_vector(hpix_nside_t nside,
			      hpix_pixel_num_t pixel_index,
			      hpix_vector_t * vector);

void hpix_nest_pixel_to_vector(hpix_nside_t nside,
			      hpix_pixel_num_t pixel_index,
			      hpix_vector_t * vector);

/* Functions implemented in bitmap.c */

hpix_bmp_projection_t * 
hpix_create_bmp_projection(unsigned int width, unsigned int height);

void
hpix_free_bmp_projection(hpix_bmp_projection_t * proj);

unsigned int
hpix_bmp_projection_width(const hpix_bmp_projection_t * proj);

unsigned int
hpix_bmp_projection_height(const hpix_bmp_projection_t * proj);

void
hpix_set_bmp_projection_width(hpix_bmp_projection_t * proj,
				unsigned int width);

void
hpix_set_bmp_projection_height(hpix_bmp_projection_t * proj,
				 unsigned int height);

hpix_projection_type_t hpix_bmp_projection_type(const hpix_bmp_projection_t * proj);
void hpix_set_equirectangular_projection(hpix_bmp_projection_t * proj);
void hpix_set_mollweide_projection(hpix_bmp_projection_t * proj);
_Bool hpix_bmp_projection_is_xy_inside(const hpix_bmp_projection_t * proj,
				       unsigned int x,
				       unsigned int y);
_Bool hpix_bmp_projection_xy_to_angles(const hpix_bmp_projection_t * proj,
				       unsigned int x,
				       unsigned int y,
				       double * theta,
				       double * phi);
double *
hpix_bmp_projection_trace(const hpix_bmp_projection_t * proj,
			  const hpix_map_t * map,
			  double * min_value,
			  double * max_value);

/* Functions implemented in cairo_interface.c */

#ifdef HAVE_CAIRO

#include <cairo/cairo.h>

cairo_surface_t *
hpix_bmp_projection_to_cairo_surface(const hpix_bmp_projection_t * proj,
				     const hpix_color_palette_t * palette,
				     const hpix_map_t * map,
				     double map_min, double map_max);

void
hpix_bmp_configure_linear_gradient(cairo_pattern_t * pattern, 
				   const hpix_color_palette_t * palette);

#endif /* HAVE_CAIRO */

/* Functions implemented in order_conversion.c */

hpix_pixel_num_t
hpix_nest_to_ring_idx(const hpix_resolution_t * resolution,
		      hpix_pixel_num_t nest_index);

hpix_pixel_num_t
hpix_ring_to_nest_idx(const hpix_resolution_t * resolution,
		      hpix_pixel_num_t ring_index);

void
hpix_switch_order(hpix_map_t * map);

/* Functions implemented in palette.c */

hpix_color_t hpix_create_color(double red, double green, double blue);
double hpix_red_from_color(const hpix_color_t * color);
double hpix_blue_from_color(const hpix_color_t * color);
double hpix_green_from_color(const hpix_color_t * color);

hpix_color_palette_t * hpix_create_black_color_palette(void);
hpix_color_palette_t * hpix_create_grayscale_color_palette(void);
hpix_color_palette_t * hpix_create_healpix_color_palette(void);
hpix_color_palette_t * hpix_create_planck_color_palette(void);
void hpix_free_color_palette(hpix_color_palette_t * palette);
void hpix_set_color_for_unseen_pixels_in_palette(hpix_color_palette_t * palette,
						 hpix_color_t new_color);
hpix_color_t hpix_color_for_unseen_pixels_in_palette(const hpix_color_palette_t * palette);
void hpix_add_step_to_color_palette(hpix_color_palette_t * palette,
				    double level, hpix_color_t color);
size_t hpix_num_of_steps_in_color_palette(const hpix_color_palette_t * palette);
hpix_color_t hpix_color_for_step_in_palette(const hpix_color_palette_t * palette,
					    size_t zero_based_index);
double hpix_level_for_step_in_palette(const hpix_color_palette_t * palette,
				      size_t zero_based_index);
void hpix_set_color_for_step_in_palette(hpix_color_palette_t * palette,
					size_t zero_based_index,
					hpix_color_t new_color);
void hpix_set_level_for_step_in_palette(hpix_color_palette_t * palette,
					size_t zero_based_index,
					double new_level);
void hpix_sort_levels_in_color_palette(hpix_color_palette_t * palette);
void hpix_palette_color(const hpix_color_palette_t * palette,
			double level, hpix_color_t * color);

/* Functions implemented in matrices.c */

void hpix_set_matrix_to_unity(hpix_matrix_t * matrix);
void hpix_set_matrix_to_zero(hpix_matrix_t * matrix);
void hpix_set_matrix_to_scale_transform(hpix_matrix_t * matrix,
					double scale_x,
					double scale_y,
					double scale_z);

_Bool hpix_is_matrix_zero(const hpix_matrix_t * matrix);
void hpix_print_matrix(FILE * output_file, 
		       const hpix_matrix_t * matrix,
		       unsigned int num_of_indents,
		       _Bool indent_first_line);
void hpix_matrix_vector_mul(hpix_vector_t * result,
			   const hpix_matrix_t * matrix,
			   const hpix_vector_t * vector);
void hpix_matrix_mul(hpix_matrix_t * result,
		     const hpix_matrix_t * matrix1,
		     const hpix_matrix_t * matrix2);
double hpix_matrix_determinant(const hpix_matrix_t * matrix);
_Bool hpix_matrix_inverse(hpix_matrix_t * result,
			  const hpix_matrix_t * matrix);

/* Functions implemented in equirectangular_projection.c */

_Bool hpix_equirectangular_is_xy_inside(const hpix_bmp_projection_t * proj,
					unsigned int x,
					unsigned int y);

_Bool hpix_equirectangular_xy_to_angles(const hpix_bmp_projection_t * proj,
					unsigned int x,
					unsigned int y,
					double * theta,
					double * phi);

/* Functions implemented in mollweide_projection.c */

_Bool hpix_mollweide_is_xy_inside(const hpix_bmp_projection_t * proj,
				  unsigned int x,
				  unsigned int y);

_Bool hpix_mollweide_xy_to_angles(const hpix_bmp_projection_t * proj,
				  unsigned int x,
				  unsigned int y,
				  double * theta,
				  double * phi);

/* Functions implemented in query_disc.c */

void hpix_query_disc(double theta, double phi, double radius,
		     hpix_pixel_num_t ** pixels,
		     size_t * num_of_matches);

void hpix_query_disc_inclusive(double theta, double phi, double radius,
			       hpix_pixel_num_t ** pixels,
			       size_t * num_of_matches);

/* Functions defined in rotate.c */

double hpix_calc_angular_distance_from_vectors(const hpix_vector_t * vector1,
					       const hpix_vector_t * vector2);
double hpix_calc_angular_distance_from_angles(double theta1_rad, double phi1_rad,
					      double theta2_rad, double phi2_rad);


/* Functions defined in vectors.c */

void hpix_print_vector(FILE * output_file, 
		       const hpix_vector_t * vector);
double hpix_vector_length(const hpix_vector_t * vector);

double hpix_dot_product(const hpix_vector_t * vector1,
			const hpix_vector_t * vector2);

void hpix_normalize_vector(hpix_vector_t * vector);

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
