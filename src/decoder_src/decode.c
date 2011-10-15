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
#include <math.h>


void difference_decode(int8_t* buffer, uint32_t* buffer_size_in_bits, uint8_t symbol_size_in_bits) {
    /* Variables */
    uint32_t        i,j;

    uint32_t        nSamples;
    uint32_t        nBlocks;
    uint8_t*        uncompressedData;
    array_stream_t* uncompressedArray;
    array_stream_t* compressedArray;

    uint32_t*       blockData;
    int32_t         difference;
    uint32_t        nBits;

    /* Gets the number of samples and blocks */
    nSamples = (*buffer_size_in_bits)/symbol_size_in_bits;
    nBlocks  = ceil(nSamples/SAMPLES_PER_BLOCK);

    /* Allocates space for block output data */
    blockData = (uint32_t*) malloc(SAMPLES_PER_BLOCK*sizeof(uint32_t));

    /* Creates input array */
    compressedArray = MakeArrayStream((uint8_t*) buffer, AS_READ);

    /* Creates output array */
    uncompressedData  = (uint8_t*) malloc(ceil((*buffer_size_in_bits)/8));
    uncompressedArray = MakeArrayStream(uncompressedData, AS_WRITE);

    /* Runs the blocks */
    for (i = 0; i < nBlocks; i++) {
        /* Gets the number of bits of compressed data elements */
        ArrayStreamGetBits(compressedArray, &nBits, symbol_size_in_bits);

        /* Gets first block data */
        ArrayStreamGetBits(uncompressedArray, &(blockData[0]), symbol_size_in_bits);

        /* Runs input block restoring original values */
        for (j = 1; j < SAMPLES_PER_BLOCK; j++) {
            ArrayStreamGetBits(uncompressedArray, &difference, nBits);

            blockData[j] = blockData[0] + difference;
        }
    }

    /* Returns the number of bits of the compressed data */
    *buffer_size_in_bits = ArrayStreamGetBitCount(uncompressedArray);

    /* Returns the compressed buffer */
    memcpy(buffer, uncompressedData, ceil((*buffer_size_in_bits)/8));

    /* Frees used variables */
    free(uncompressedData);
    free(blockData);
}


void run_length_decode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t symbol_size_in_bits){
	array_stream_t *array_stream_uncompressed,*array_stream_compressed;
	int8_t *uncompressed_buffer;
	
	TRACE_RUN_LENGTH("\n\nCHANNEL START->");
	uncompressed_buffer = malloc((2*(*buffer_size_in_bits))/8*sizeof(int8_t));/*In cade of expanding instead of compressing */
	array_stream_compressed = MakeArrayStream((unsigned char *)buffer,AS_READ);
	array_stream_uncompressed = MakeArrayStream((unsigned char *)uncompressed_buffer,AS_WRITE);
	
	if(!symbol_size_in_bits){
		uint32_t size_left=*buffer_size_in_bits;
		
		
		uint32_t new_symbol_size_in_bits=0,first_sample=0,total_bits_encoded=0;
		
		while(size_left>0){
			uint32_t decoded_bits_in_block=0;
			ArrayStreamGetBits(array_stream_compressed, &new_symbol_size_in_bits, symbol_size_in_bits);
			TRACE_RUN_LENGTH("[variable run length]Reading variable run length = %u bits 0x%X\n",symbol_size_in_bits,current);
			ArrayStreamGetBits(array_stream_compressed, &first_sample, symbol_size_in_bits);
			ArrayStreamPutBits(array_stream_uncompressed, &first_sample, symbol_size_in_bits);
			TRACE_RUN_LENGTH("Reading first sample and writing it (%u) (first sample) 0x%X\n",symbol_size_in_bits,current);
			/* Skip those two samples */
			buffer+=((2*symbol_size_in_bits)/8);
			buffer_size_in_bits-=(2*symbol_size_in_bits);
			/* Encode the block, -1 comes from 1 less sample in block(the first one) */ 
			if(size_left>((SAMPLES_PER_BLOCK-1)*symbol_size_in_bits)){
				decoded_bits_in_block = (SAMPLES_PER_BLOCK-1)*new_symbol_size_in_bits;
			}
			else{
				decoded_bits_in_block = size_left/new_symbol_size_in_bits;
			}
			run_length_decode(buffer, &decoded_bits_in_block, new_symbol_size_in_bits);
			/* Skip all samples already encoded, -1 comes from less ne block */
			buffer+=(SAMPLES_PER_BLOCK-1)*(symbol_size_in_bits/8);
			/* Sums bits encoded */
			size_left -= (SAMPLES_PER_BLOCK-1)*symbol_size_in_bits;
			return;
		}
		
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
	
	memcpy(buffer,uncompressed_buffer,ceil(ArrayStreamGetBitCount(array_stream_uncompressed)/8));
	*buffer_size_in_bits = ArrayStreamGetBitCount(array_stream_compressed);
	free(uncompressed_buffer);
}
