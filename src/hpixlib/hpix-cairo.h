/* hpix-cairo.h -- header file for the HPixLib library (Cairo functions)
 *
 * Copyright 2011-2013 Maurizio Tomasi.
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

#ifndef HPIX_CAIRO_H
#define HPIX_CAIRO_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <cairo/cairo.h>
#include <hpixlib/hpix.h>

/* Functions implemented in cairo_interface.c */

cairo_surface_t *
hpix_bmp_projection_to_cairo_surface(const hpix_bmp_projection_t * proj,
				     const hpix_color_palette_t * palette,
				     const hpix_map_t * map,
				     double map_min, double map_max);

void
hpix_bmp_configure_linear_gradient(cairo_pattern_t * pattern, 
				   const hpix_color_palette_t * palette);

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
