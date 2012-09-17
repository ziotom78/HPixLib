/* io.c -- Read/write maps into FITS files
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

#include <hpxlib.h>
#include <fitsio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

/****************************************************************************/

int
hpxlib_load_fits_component_from_fitsptr(fitsfile * fptr,
					unsigned short column_number,
					hpxlib_map_t ** map,
					int * status)
{
    /* Local Declarations */
    long row_idx;
    long num_of_rows;
    char coord_sys_key[FLEN_KEYWORD];
    char ordering_key[FLEN_KEYWORD];
    double * pixels;
    long nside;
    long elements_per_row;
    hpxlib_coordinates_t coord_sys;

    assert(fptr);
    assert(map);
    *map = NULL;

    /* Get the number of rows */
    if(fits_get_num_rows(fptr, &num_of_rows, status))
	return 0;

    /* Usually one row contains many elements (this is for
     * efficiency), so we must read the repeat count as well. */
    if(fits_get_bcolparms(fptr, column_number,
			  NULL, NULL, /* ttype, tunit */
			  NULL, /* typechar */
			  &elements_per_row,
			  NULL, NULL, /* scale, zero */
			  NULL, NULL, /* nulval, disp */
			  status))
	return 0;

    if(fits_read_key_lng(fptr, "NSIDE", &nside, NULL, status))
	return 0;

    assert(hpxlib_nside_to_npixel((hpxlib_nside_t) nside)
	   == num_of_rows * elements_per_row);

    if(fits_read_key(fptr, TSTRING, "COORDSYS", coord_sys_key, NULL, status))
	status = 0;

    if(fits_read_key(fptr, TSTRING, "ORDERING", ordering_key, NULL, status))
	status = 0;

    /* Read the array */
    switch(ordering_key[0])
    {
    case 'G': coord_sys = HPXLIB_COORD_GALACTIC; break;
    case 'E': coord_sys = HPXLIB_COORD_ECLIPTIC; break;
    case 'Q': coord_sys = HPXLIB_COORD_CUSTOM; break;
    default: coord_sys = HPXLIB_COORD_CELESTIAL; break;
    }

    *map = hpxlib_create_map(nside, coord_sys);
    pixels = hpxlib_map_pixels(*map);
    for (row_idx = 0; row_idx < num_of_rows; ++row_idx)
    {
	if(fits_read_col_dbl(fptr, column_number,
			     row_idx + 1, 1,
			     elements_per_row, NAN,
			     &pixels[row_idx * elements_per_row],
			     NULL, status))
	{
	    hpxlib_free_map(*map);
	    map = NULL;
	    return 0;
	}
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpxlib_load_fits_component_from_file(const char * file_name,
				     unsigned short column_number,
				     hpxlib_map_t ** map,
				     int * status)
{
    /* Local Declarations */
    fitsfile *fptr;

    assert(file_name);
    assert(map);
    *map = NULL;

    /* Open the file and move to the specified HDU */
    if(fits_open_table(&fptr, file_name, READONLY, status))
	return 0;

    if(! hpxlib_load_fits_component_from_fitsptr(fptr, column_number, map, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpxlib_free_map(*map);
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpxlib_create_empty_fits_table_for_map(fitsfile * fptr,
				       const hpxlib_map_t * template_map,
				       unsigned short num_of_components,
				       const char * measure_unit,
				       int * status)
{
    int bitpix = SHORT_IMG;
    long naxis = 0;
    long naxes[] = {0,0};

    long num_of_pixels;

    char ordering_key[FLEN_KEYWORD]; /* HEALPix ordering */
    char extname[] = "BINTABLE";     /* extension name */
    char *ttype[] = { "I_STOKES", "Q_STOKES", "U_STOKES" };
    char *tform[] = { "1E", "1E", "1E" };
    char *tunit[3];
    char coord_sys_key[] = " ";
    long nside;

    tunit[0] = tunit[1] = tunit[2] = (char *) measure_unit;
    nside = hpxlib_map_nside(template_map);
    num_of_pixels = hpxlib_map_num_of_pixels(template_map);
    if (hpxlib_map_ordering(template_map) == HPXLIB_ORDER_NEST)
	strcpy(ordering_key, "NESTED");
    else
	strcpy(ordering_key, "RING");

    switch(hpxlib_map_coordinate_system(template_map))
    {
    case HPXLIB_COORD_ECLIPTIC: coord_sys_key[0] = 'E'; break;
    case HPXLIB_COORD_GALACTIC: coord_sys_key[0] = 'G'; break;
    case HPXLIB_COORD_CUSTOM: coord_sys_key[0] = 'Q'; break;
    default: coord_sys_key[0] = 'C';
    }

    if(fits_create_img(fptr, bitpix, naxis, naxes, status)
       || fits_write_date(fptr, status)
       || fits_movabs_hdu(fptr, 1, NULL, status)
       || fits_create_tbl(fptr, BINARY_TBL, num_of_pixels,
			  num_of_components, ttype, tform,
			  tunit, extname, status)
       || fits_write_key(fptr, TSTRING, "PIXTYPE", "HEALPIX",
			 "HEALPIX Pixelisation", status)
       || fits_write_key(fptr, TSTRING, "ORDERING", ordering_key,
			 "Pixel ordering scheme, either "
			 "RING or NESTED", status)
       || fits_write_key(fptr, TLONG, "NSIDE", &nside,
			 "Resolution parameter for HEALPIX", status)
       || fits_write_key(fptr, TSTRING, "COORDSYS", coord_sys_key,
			 "Coordinate system used in the map", status)
       || fits_write_comment(fptr,
			     "           "
			     "G = Galactic, "
			     "E = ecliptic, "
			     "C = celestial = equatorial", status))
	return 0;

    return 1;
}

/****************************************************************************/


int
hpxlib_save_fits_component_to_fitsptr(fitsfile * fptr,
				      const hpxlib_map_t * map,
				      int data_type,
				      const char * measure_unit,
				      int * status)
{
    assert(fptr);
    assert(map);

    if(! hpxlib_create_empty_fits_table_for_map(fptr, map, 1,
						measure_unit, status))
	return 0;

    if(fits_write_col(fptr, TDOUBLE, 1, 1, 1, hpxlib_map_num_of_pixels(map),
		      hpxlib_map_pixels(map), status))
	return 0;

    return 1;
}

/****************************************************************************/


int
hpxlib_save_fits_component_to_file(const char * file_name,
				   const hpxlib_map_t * map,
				   int data_type,
				   const char * measure_unit,
				   int * status)
{
    fitsfile * fptr = NULL;

    assert(file_name);
    assert(map);

    /* create new FITS file */
    if (fits_create_file(&fptr, file_name, status))
	return 0;

    if(! hpxlib_save_fits_component_to_fitsptr(fptr, map, data_type,
					       measure_unit, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))       /* close the FITS file */
	return 0;

    return 1;
}

/****************************************************************************/


int
hpxlib_load_fits_pol_from_fitsptr(fitsfile * fptr,
				  hpxlib_map_t ** map_i,
				  hpxlib_map_t ** map_q,
				  hpxlib_map_t ** map_u,
				  int * status)
{
    assert(fptr);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    *map_i = *map_q = *map_u = NULL;

    if(! hpxlib_load_fits_component_from_fitsptr(fptr, 1, map_i, status))
    {
	*map_i = NULL;
	return 0;
    }

    if(! hpxlib_load_fits_component_from_fitsptr(fptr, 2, map_q, status))
    {
	hpxlib_free_map(*map_i);
	*map_i = NULL;
	return 0;
    }

    if(! hpxlib_load_fits_component_from_fitsptr(fptr, 3, map_u, status))
    {
	hpxlib_free_map(*map_i);
	hpxlib_free_map(*map_q);
	*map_i = *map_q = NULL;
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpxlib_free_map(*map_i);
	hpxlib_free_map(*map_q);
	hpxlib_free_map(*map_u);
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpxlib_load_fits_pol_from_file(const char * file_name,
			       hpxlib_map_t ** map_i,
			       hpxlib_map_t ** map_q,
			       hpxlib_map_t ** map_u,
			       int * status)
{
    /* Local Declarations */
    fitsfile *fptr;

    assert(file_name);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    if(fits_open_table(&fptr, file_name, READONLY, status))
    {
	return 0;
    }

    if(! hpxlib_load_fits_pol_from_fitsptr(fptr, map_i, map_q, map_u, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpxlib_free_map(*map_i);
	hpxlib_free_map(*map_q);
	hpxlib_free_map(*map_u);
	*map_i = *map_q = *map_u = NULL;
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpxlib_save_fits_pol_to_fitsptr(fitsfile * fptr,
				const hpxlib_map_t * map_i,
				const hpxlib_map_t * map_q,
				const hpxlib_map_t * map_u,
				int data_type,
				const char * measure_unit,
				int * status)
{
    long num_of_pixels;

    assert(fptr);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    assert(hpxlib_map_nside(map_i) == hpxlib_map_nside(map_q));
    assert(hpxlib_map_nside(map_i) == hpxlib_map_nside(map_u));

    if(! hpxlib_create_empty_fits_table_for_map(fptr, map_i, 3,
						measure_unit, status))
	return 0;

    num_of_pixels = (long) hpxlib_map_num_of_pixels(map_i);
    if(fits_write_col(fptr, TDOUBLE, 1, 1, 1, num_of_pixels,
		      hpxlib_map_pixels(map_i), status)
       || fits_write_col(fptr, TDOUBLE, 2, 1, 1, num_of_pixels,
			 hpxlib_map_pixels(map_q), status)
       || fits_write_col(fptr, TDOUBLE, 3, 1, 1, num_of_pixels,
			 hpxlib_map_pixels(map_u), status))
	return 0;

    return 1;
}

/****************************************************************************/


int
hpxlib_save_fits_pol_to_file(const char * file_name,
			     const hpxlib_map_t * map_i,
			     const hpxlib_map_t * map_q,
			     const hpxlib_map_t * map_u,
			     int data_type,
			     const char * measure_unit,
			     int * status)
{
    fitsfile * fptr = NULL;

    assert(file_name);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    /* create new FITS file */
    if (fits_create_file(&fptr, file_name, status))
	return 0;

    if(! hpxlib_save_fits_pol_to_fitsptr(fptr, map_i, map_q, map_u,
				       data_type, measure_unit, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))       /* close the FITS file */
	return 0;

    return 1;
}
