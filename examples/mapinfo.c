/* mapinfo.c -- Dump information about a FITS map
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

#include <hpixlib/hpix.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* Compute the peak-to-peak difference of the value of the
   pixels in the map */
double peak_to_peak_amplitude(const hpix_map_t * map)
{
  size_t idx;
  double min, max;
  double * pixels;

  assert(map);

  pixels = hpix_map_pixels(map);
  min = max = pixels[0];
  for(int idx = 1; idx < hpix_map_num_of_pixels(map); ++idx)
  {
    if(isnan(pixels[idx])) /* Skip unseen pixels */
      continue;

    if(min > pixels[idx])
      min = pixels[idx];

    if(max < pixels[idx])
      max = pixels[idx];
  }

  return max - min;
}

int main(int argc, char ** argv)
{
  char * error_message = NULL;

  /* Skip the program name */
  ++argv; --argc;

  if(argc == 0) {
      fputs("Usage: mapinfo FILE1 [FILE2...]\n", stderr);
      return EXIT_SUCCESS;
  }

  while(argc--) {
      int cfitsio_status = 0;
      hpix_map_t * map;

      hpix_load_fits_component_from_file(argv[0], 1, &map, &cfitsio_status);

      if(map)
      {
	  printf("File name: %s\n", *argv);
	  printf("NSIDE: %u\n", hpix_map_nside(map));
	  printf("Ordering: %s\n",
		 hpix_map_ordering_scheme(map) == HPIX_ORDER_SCHEME_RING ?
		 "RING" : "NEST");
	  printf("Peak-to-peak variation: %.4g\n",
		 peak_to_peak_amplitude(map));
	  
	  hpix_free_map(map);
      } else {
	  fprintf(stderr, "Error: %s\n", error_message);
	  hpix_free(error_message);
      }
      
      ++argv;
  }
  
  return EXIT_SUCCESS;
}
