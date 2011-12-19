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

#include "chpx.h"
#include <assert.h>

chpx_pixel_num_t
chpx_nest_to_ring_idx(chpx_nside_t nside, chpx_pixel_num_t nest_index)
{
    return 0;
}

chpx_pixel_num_t
chpx_ring_to_nest_idx(chpx_nside_t nside, chpx_pixel_num_t ring_index)
{
    return 0;
}

void
chpx_switch_order(chpx_map_t * map)
{
    assert(map);
}
