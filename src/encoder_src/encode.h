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
 * 	buffer - IN/OUT - Channel start data position
 * 	buffer_size - IN/OUT -Size of channel data in BITS.. after compressing needs to be update with ceil(buffer_size)
 * 	bytes_per_sample - IN -Number of BYTES per sample
 * 
 * 	What it does?
 * 	Compress buffer with difference encoding each SAMPLES_PER_BLOCK(how to define SAMPLES_PER_BLOCK? static in a define?) samples with a best calculated delta size(acording to the group of samples)
 * 	Example:
 * 	Considering bytes_per_sample=2
 * 	Original stream:
 * 	0x4440 0x4421 0x4400 0x4400 0x4422 0x4421 0x4400 0x4400 0x4404 0x4407 0x4400 0x4403 0x4405
 * 	using SAMPLES_PER_BLOCK 5 bytes
 * 	Final stream: 
 * 	0x06 0x4440 -29 -21 0 22 -1 0x04 0x4400 4 3 -7 3 2
 *  The first bytes_per_sample*8 bits is how much bits_for_difference in the block(max size off course is bytes_per_sample*8)
 * 	second is the first sample of the block
 * 	Every group of next bits_for_difference represents delta between samples
 * */
void difference_encode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t bytes_per_sample);




/*
 * Params: 
 * 	buffer - IN/OUT - Channel start data position
 * 	buffer_size - IN/OUT -Size of channel data in BITS.. after compressing needs to be update with ceil(buffer_size)
 * 	running_after_difference_encoding - IN - if its running after a difference encoding
 * 
 * 	What it does?
 * 	Compress buffer with runlength
 * 	if symbol_size_in_bits is == 0, read first sample every block to determine run length size symbol
 * 	(assume its used after a difference encode, and only incode inside the block, not bits_for_difference neither first sample) else,
 * 	use symbol size of symbol_size_in_bits 
 * 	The encoding is done after one symbol appears twice.. Then the next char represents how much copies it has.
 * 	Ex:
 * 	abcddddeeoooooabc
 * 	abcdd2ee0oo3abc
 * */
void run_length_encode(int8_t *buffer,uint32_t *buffer_size_in_bits, int symbol_size_in_bits);
