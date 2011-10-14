/*     	decode.c
      
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


#include "decode.h"

void difference_decode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t bytes_per_sample){
	
}


void run_length_decode(int8_t *buffer,uint32_t *buffer_size_in_bits, int symbol_size_in_bits){
	array_stream_t *array_stream_uncompressed,*array_stream_compressed;
	int8_t *uncompressed_buffer;
	
	TRACE_RUN_LENGTH("\n\nCHANNEL START->");
	uncompressed_buffer = malloc((2*(*buffer_size_in_bits))/8*sizeof(int8_t));/*In cade of expanding instead of compressing */
	array_stream_compressed = MakeArrayStream((unsigned char *)buffer,AS_READ);
	array_stream_uncompressed = MakeArrayStream((unsigned char *)uncompressed_buffer,AS_WRITE);
	
	if(!symbol_size_in_bits){
	}
	else{
		uint32_t last=0,current=0;
		ArrayStreamGetBits(array_stream_compressed, &last, symbol_size_in_bits);
		ArrayStreamPutBits(array_stream_uncompressed, &last, symbol_size_in_bits);
		TRACE_RUN_LENGTH("Writing %d bits (first sample) 0x%X\n",symbol_size_in_bits,last);
		while(*buffer_size_in_bits>ArrayStreamGetBitCount(array_stream_compressed)){
			ArrayStreamGetBits(array_stream_compressed, &current, symbol_size_in_bits);
			ArrayStreamPutBits(array_stream_uncompressed, &current, symbol_size_in_bits);
			TRACE_RUN_LENGTH("Writing %d bits (normal sample) 0x%X\n",symbol_size_in_bits,current);
			TRACE_RUN_LENGTH("Current=%X  Last=%X\n",current,last);
			if ((current==last)){
				if (*buffer_size_in_bits>ArrayStreamGetBitCount(array_stream_compressed)){
					ArrayStreamGetBits(array_stream_compressed, &current, symbol_size_in_bits);
					while(current){
						TRACE_RUN_LENGTH("Writing %d bits (copies samples) 0x%X\n",symbol_size_in_bits,last);
						ArrayStreamPutBits(array_stream_uncompressed,&last,symbol_size_in_bits);
						current--;
					}
				}
			}
			else{
				last = current;
			}
		}
	}
	TRACE_RUN_LENGTH("<-CHANNEL STOP->\n\n");
	
	memcpy(buffer,uncompressed_buffer,ArrayStreamGetBitCount(array_stream_uncompressed)/8+((ArrayStreamGetBitCount(array_stream_uncompressed)%8)?1:0));
	*buffer_size_in_bits = ArrayStreamGetBitCount(array_stream_compressed);
	free(uncompressed_buffer);
}
