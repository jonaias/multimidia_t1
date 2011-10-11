/*      encode.h
      
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
/*
 * Params: 
 * 	buffer - Channel start data position
 * 	buffer_size - Size of channel data in BITS
 * 	bytes_per_sample - Number of BYTES per sample
 * 
 * 	What it does?
 * 	Compress buffer with difference encoding each SAMPLES_PER_BLOCK(how to define SAMPLES_PER_BLOCK? static in a define?) samples with a best calculated delta size(acording to the group of samples)
 * 	Example:
 * 	Considering bytes_per_sample=2
 * 	0x4440 0x4421 0x4400 0x4400 0x4422 0x4421 0x4400 0x4400 0x4404 0x4407 0x4400 0x4403 0x4405
 * 	using SAMPLES_PER_BLOCK 5 bytes 
 * 	0x06 0x4440 -29 -21 0 22 -1 0x04 0x4400 4 3 -7 3 2
 *  first byte how much bits_for_difference in the block(max size off course is bytes_per_sample*8)
 * 	second is the first sample of the block
 * 	Every group of next bits_for_difference represents delta between samples
 * */
void difference_encode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t bytes_per_sample);
void run_length_encode(int8_t *buffer,uint32_t *buffer_size_in_bits);
