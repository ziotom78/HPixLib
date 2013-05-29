/* map2ppm.c -- Save the Mollweide projection of a map into a PPM file
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

#include <stdio.h>
#include <hpixlib/hpix.h>
   
void output_map_to_file(const hpix_map_t * map,
			    hpix_color_palette_t * palette,
			FILE * out)
{
    /* Keeping it double makes calculations more efficient */
    const double max_color_level = 255.0;

    hpix_bmp_projection_t * proj = 
	hpix_create_bmp_projection(640, 320);
    hpix_set_mollweide_projection(proj);

    double min, max;
    double * bitmap = hpix_bmp_projection_trace(proj, map, &min, &max);

    /* Write the PPM header */
    fprintf(out, "P3\n%u %u\n%u\n",
	    hpix_bmp_projection_width(proj),
	    hpix_bmp_projection_height(proj),
	    (unsigned) max_color_level);

    double *restrict cur_pixel = bitmap;
    for(unsigned y = 0; y < hpix_bmp_projection_height(proj); ++y)
    {
	for(unsigned x = 0; x < hpix_bmp_projection_width(proj); ++x)
	{
	    hpix_color_t pixel_color;
	    hpix_palette_color(palette,
			       (*cur_pixel++ - min) / (max - min),
			       &pixel_color);
	    fprintf(out, "%3u %3u %3u\t",
		    (unsigned) (hpix_red_from_color(&pixel_color) * max_color_level),
		    (unsigned) (hpix_green_from_color(&pixel_color) * max_color_level),
		    (unsigned) (hpix_blue_from_color(&pixel_color) * max_color_level));
	}

	fputc('\n', out);
    }
  
    hpix_free(bitmap);
    hpix_free_bmp_projection(proj);
}

int main(int argc, const char ** argv)
{
    if(argc != 2)
    {
	fputs("You must specify the name of a FITS file on the command line\n",
	      stderr);
	return 1;
    }

    hpix_map_t * map;
    const char * file_name = argv[1];
    int status;
    if(! hpix_load_fits_component_from_file(file_name, 1, &map, &status))
    {
	fprintf(stderr, "Unable to read map %s\n", file_name);
	return 1;
    }

    hpix_color_palette_t * palette = hpix_create_planck_color_palette();
    output_map_to_file(map, palette, stdout);
    hpix_free_color_palette(palette);
    hpix_free_map(map);
}
