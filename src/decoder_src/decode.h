/*      decode.h

        Copyright 2011 jonas <jonaias@jonaias-MX6453>

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.
        
        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
        
        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
        MA 02110-1301, USA.*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h> /*uint8_t*/
#include <ctype.h>  /*isprint*/

#include "../util/arraystream.h"
#include "../util/util.h"


void difference_decode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t symbol_size_in_bits);

void run_length_decode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t symbol_size_in_bits);
