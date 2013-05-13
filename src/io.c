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

#include <hpixlib/hpix.h>
#include <fitsio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

/****************************************************************************/

int
hpix_load_fits_component_from_fitsptr(fitsfile * fptr,
				      unsigned short column_number,
				      hpix_map_t ** map,
				      int * status)
{
    /* Local Declarations */
    long row_idx;
    long num_of_rows;
    char coord_sys_key[FLEN_KEYWORD] = "";
    char ordering_key[FLEN_KEYWORD] = "";
    double * pixels;
    long nside;
    long elements_per_row;
    hpix_ordering_scheme_t ordering;

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

    assert(hpix_nside_to_npixel((hpix_nside_t) nside)
	   == num_of_rows * elements_per_row);

    if(fits_read_key(fptr, TSTRING, "COORDSYS", &coord_sys_key[0], NULL, status))
	*status = 0;

    if(fits_read_key(fptr, TSTRING, "ORDERING", &ordering_key[0], NULL, status))
	*status = 0;

    /* Read the array */
    switch(ordering_key[0])
    {
    case 'N': ordering = HPIX_ORDER_SCHEME_NEST; break;
    default: ordering = HPIX_ORDER_SCHEME_RING; break;
    }

    *map = hpix_create_map(nside, ordering);
    pixels = hpix_map_pixels(*map);
    for (row_idx = 0; row_idx < num_of_rows; ++row_idx)
    {
	if(fits_read_col_dbl(fptr, column_number,
			     row_idx + 1, 1,
			     elements_per_row, NAN,
			     &pixels[row_idx * elements_per_row],
			     NULL, status))
	{
	    hpix_free_map(*map);
	    map = NULL;
	    return 0;
	}
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpix_load_fits_component_from_file(const char * file_name,
				   unsigned short column_number,
				   hpix_map_t ** map,
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

    if(! hpix_load_fits_component_from_fitsptr(fptr, column_number, map, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpix_free_map(*map);
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpix_create_empty_fits_table_for_map(fitsfile * fptr,
				     const hpix_map_t * template_map,
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
    nside = hpix_map_nside(template_map);
    num_of_pixels = hpix_map_num_of_pixels(template_map);
    if (hpix_map_ordering_scheme(template_map) == HPIX_ORDER_SCHEME_NEST)
	strcpy(ordering_key, "NESTED");
    else
	strcpy(ordering_key, "RING");

    switch(hpix_map_coordinate_system(template_map))
    {
    case HPIX_COORD_ECLIPTIC: coord_sys_key[0] = 'E'; break;
    case HPIX_COORD_GALACTIC: coord_sys_key[0] = 'G'; break;
    case HPIX_COORD_CUSTOM: coord_sys_key[0] = 'Q'; break;
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
hpix_save_fits_component_to_fitsptr(fitsfile * fptr,
				    const hpix_map_t * map,
				    int data_type,
				    const char * measure_unit,
				    int * status)
{
    assert(fptr);
    assert(map);

    if(! hpix_create_empty_fits_table_for_map(fptr, map, 1,
					      measure_unit, status))
	return 0;

    if(fits_write_col(fptr, TDOUBLE, 1, 1, 1, hpix_map_num_of_pixels(map),
		      hpix_map_pixels(map), status))
	return 0;

    return 1;
}

/****************************************************************************/


int
hpix_save_fits_component_to_file(const char * file_name,
				 const hpix_map_t * map,
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

    if(! hpix_save_fits_component_to_fitsptr(fptr, map, data_type,
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
hpix_load_fits_pol_from_fitsptr(fitsfile * fptr,
				hpix_map_t ** map_i,
				hpix_map_t ** map_q,
				hpix_map_t ** map_u,
				int * status)
{
    assert(fptr);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    *map_i = *map_q = *map_u = NULL;

    if(! hpix_load_fits_component_from_fitsptr(fptr, 1, map_i, status))
    {
	*map_i = NULL;
	return 0;
    }

    if(! hpix_load_fits_component_from_fitsptr(fptr, 2, map_q, status))
    {
	hpix_free_map(*map_i);
	*map_i = NULL;
	return 0;
    }

    if(! hpix_load_fits_component_from_fitsptr(fptr, 3, map_u, status))
    {
	hpix_free_map(*map_i);
	hpix_free_map(*map_q);
	*map_i = *map_q = NULL;
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpix_free_map(*map_i);
	hpix_free_map(*map_q);
	hpix_free_map(*map_u);
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpix_load_fits_pol_from_file(const char * file_name,
			     hpix_map_t ** map_i,
			     hpix_map_t ** map_q,
			     hpix_map_t ** map_u,
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

    if(! hpix_load_fits_pol_from_fitsptr(fptr, map_i, map_q, map_u, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))
    {
	hpix_free_map(*map_i);
	hpix_free_map(*map_q);
	hpix_free_map(*map_u);
	*map_i = *map_q = *map_u = NULL;
	return 0;
    }

    /* Later */
    return 1;
}

/****************************************************************************/


int
hpix_save_fits_pol_to_fitsptr(fitsfile * fptr,
			      const hpix_map_t * map_i,
			      const hpix_map_t * map_q,
			      const hpix_map_t * map_u,
			      int data_type,
			      const char * measure_unit,
			      int * status)
{
    long num_of_pixels;

    assert(fptr);
    assert(map_i);
    assert(map_q);
    assert(map_u);

    assert(hpix_map_nside(map_i) == hpix_map_nside(map_q));
    assert(hpix_map_nside(map_i) == hpix_map_nside(map_u));

    if(! hpix_create_empty_fits_table_for_map(fptr, map_i, 3,
					      measure_unit, status))
	return 0;

    num_of_pixels = (long) hpix_map_num_of_pixels(map_i);
    if(fits_write_col(fptr, TDOUBLE, 1, 1, 1, num_of_pixels,
		      hpix_map_pixels(map_i), status)
       || fits_write_col(fptr, TDOUBLE, 2, 1, 1, num_of_pixels,
			 hpix_map_pixels(map_q), status)
       || fits_write_col(fptr, TDOUBLE, 3, 1, 1, num_of_pixels,
			 hpix_map_pixels(map_u), status))
	return 0;

    return 1;
}

/****************************************************************************/


int
hpix_save_fits_pol_to_file(const char * file_name,
			   const hpix_map_t * map_i,
			   const hpix_map_t * map_q,
			   const hpix_map_t * map_u,
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

    if(! hpix_save_fits_pol_to_fitsptr(fptr, map_i, map_q, map_u,
				       data_type, measure_unit, status))
    {
	fits_close_file(fptr, NULL);
	return 0;
    }

    if(fits_close_file(fptr, status))
	return 0;

    return 1;
}
