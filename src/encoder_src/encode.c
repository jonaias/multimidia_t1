/*     	encode.c
      
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


#include "encode.h"

void difference_encode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t bytes_per_sample){
	
}


/*
 * Params: 
 * 	buffer - IN/OUT - Channel start data position
 * 	buffer_size - IN/OUT -Size of channel data in BITS.. after compressing needs to be update with ceil(buffer_size)
 * 	symbol_size_in_bits - IN - if its running after a difference encoding
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
void run_length_encode(int8_t *buffer,uint32_t *buffer_size_in_bits,int symbol_size_in_bits){
	array_stream_t *array_stream_uncompressed,*array_stream_compressed;
	int8_t *compressed_buffer;
	
	TRACE_RUN_LENGTH("\n\nCHANNEL START->");
	compressed_buffer = malloc((2*(*buffer_size_in_bits))/8*sizeof(int8_t));/*In cade of expanding instead of compressing */
	array_stream_uncompressed = MakeArrayStream((unsigned char *)buffer,AS_READ);
	array_stream_compressed = MakeArrayStream((unsigned char *)compressed_buffer,AS_WRITE);
	
	if(!symbol_size_in_bits){
	}
	else{
		uint32_t last=0,current=0;
		ArrayStreamGetBits(array_stream_uncompressed, &last, symbol_size_in_bits);
		ArrayStreamPutBits(array_stream_compressed, &last, symbol_size_in_bits);
		TRACE_RUN_LENGTH("Writing %d bits (first sample) 0x%X\n",symbol_size_in_bits,current);
		while(*buffer_size_in_bits>ArrayStreamGetBitCount(array_stream_uncompressed)){
			ArrayStreamGetBits(array_stream_uncompressed, &current, symbol_size_in_bits);
			ArrayStreamPutBits(array_stream_compressed, &current, symbol_size_in_bits);
			TRACE_RUN_LENGTH("Writing %d bits (normal sample) 0x%X\n",symbol_size_in_bits,current);
			if (current==last){
					uint32_t number_of_copy=0;
					/* Counts the run length */
					while((((float)number_of_copy)<pow(2,symbol_size_in_bits)) && (*buffer_size_in_bits>ArrayStreamGetBitCount(array_stream_uncompressed)) ){
						current =0;
						ArrayStreamGetBits(array_stream_uncompressed, &current, symbol_size_in_bits);
						if(current==last){
							number_of_copy++;
						}
						else{
							ArrayStreamPutBits(array_stream_compressed,&number_of_copy,symbol_size_in_bits);
							TRACE_RUN_LENGTH("Writing %d bits (number of copy) 0x%X\n",symbol_size_in_bits,number_of_copy);
							ArrayStreamPutBits(array_stream_compressed,&current,symbol_size_in_bits);
							TRACE_RUN_LENGTH("Writing %d bits(next sample in sequence after a number of copy) 0x%X\n",symbol_size_in_bits,current);
							break;
						}
					}
			}
				/* Store as last integer 16bit found */
			last = current;
		}
	}
	TRACE_RUN_LENGTH("<-CHANNEL STOP->\n\n");
	
	memcpy(buffer,compressed_buffer,ArrayStreamGetBitCount(array_stream_compressed)/8+(ArrayStreamGetBitCount(array_stream_compressed)%8?1:0));
	*buffer_size_in_bits = ArrayStreamGetBitCount(array_stream_compressed);
	free(compressed_buffer);
}
