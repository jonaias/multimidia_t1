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
#include <math.h>
#include <string.h>


void difference_encode(int8_t *buffer,uint32_t *buffer_size_in_bits, uint8_t symbol_size_in_bits) {
    /* Variables */
    uint32_t        i,j;

    uint32_t        nBlocks;
    uint8_t*        compressedData;
    array_stream_t* compressedArray;
    array_stream_t* uncompressedArray;

    uint32_t*       blockData;
    int32_t         maxDifference;
    int32_t         difference;
    uint32_t        nBits;

    /* Calculates the number of compression blocks */
    nBlocks = ceil((*buffer_size_in_bits)/(8*SAMPLES_PER_BLOCK));

    /* Allocates space for block input data */
    blockData = (uint32_t*) malloc(SAMPLES_PER_BLOCK*sizeof(uint32_t));

    /* Creates input array */
    uncompressedArray = MakeArrayStream((uint8_t*) buffer, AS_READ);

    /* Creates output array */
    compressedData  = (uint8_t*) malloc(ceil((*buffer_size_in_bits)/8));
    compressedArray = MakeArrayStream(compressedData, AS_WRITE);

    /* Runs the blocks */
    for (i = 0; i < nBlocks; i++) {
        /* Gets input block data */
        for (j = 0; j < SAMPLES_PER_BLOCK; j++) {
            ArrayStreamGetBits(uncompressedArray, &(blockData[j]), symbol_size_in_bits);
        }

        /* Runs the bytes looking for the biggest difference */
        maxDifference = 0;
        for (j = 1; j < SAMPLES_PER_BLOCK; j++) {
            difference = abs(blockData[0]-blockData[j]);

            if (difference > maxDifference) {
                maxDifference = difference;
            }
        }

        /* Calculates the number of bits needed to store this difference */
        nBits = ceil(log2(maxDifference))+1;

        /* Stores this information at the compressed array */
        ArrayStreamPutBits(compressedArray, &nBits, symbol_size_in_bits);

        /* Stores first block data */
        ArrayStreamPutBits(compressedArray, &(blockData[0]), symbol_size_in_bits);

        /* Runs the block data calculating and storing theirs delta */
        for (j = 0; j < SAMPLES_PER_BLOCK; j++) {
            difference = blockData[0] - blockData[j];
            ArrayStreamPutBits(compressedArray, &difference, nBits);
        }
    }

    /* Returns the number of bits of the compressed data */
    *buffer_size_in_bits = ArrayStreamGetBitCount(compressedArray);

    /* Returns the compressed buffer */
    memcpy(buffer, compressedData, ceil((*buffer_size_in_bits)/8));

    /* Frees used variables */
    free(compressedData);
    free(blockData);
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
void run_length_encode(int8_t *buffer,uint32_t *buffer_size_in_bits,uint8_t symbol_size_in_bits){
	array_stream_t *array_stream_uncompressed,*array_stream_compressed;
	int8_t *compressed_buffer;
	
	TRACE_RUN_LENGTH("\n\nCHANNEL START->");
	compressed_buffer = malloc((2*(*buffer_size_in_bits))/8*sizeof(int8_t));/*In cade of expanding instead of compressing */
	array_stream_uncompressed = MakeArrayStream((unsigned char *)buffer,AS_READ);
	array_stream_compressed = MakeArrayStream((unsigned char *)compressed_buffer,AS_WRITE);
	
	if(!symbol_size_in_bits){
		uint32_t new_symbol_size_in_bits=0,first_sample=0,total_bits_encoded=0;
		
		while(*buffer_size_in_bits){
			uint32_t encoded_bits_in_block=0;
			ArrayStreamGetBits(array_stream_uncompressed, &new_symbol_size_in_bits, symbol_size_in_bits);
			ArrayStreamPutBits(array_stream_compressed, &new_symbol_size_in_bits, symbol_size_in_bits);
			TRACE_RUN_LENGTH("[variable run length]Reading and writing variable run length = %u bits 0x%X\n",symbol_size_in_bits,current);
			ArrayStreamGetBits(array_stream_uncompressed, &first_sample, symbol_size_in_bits);
			ArrayStreamPutBits(array_stream_compressed, &first_sample, symbol_size_in_bits);
			TRACE_RUN_LENGTH("[variable run length]Reading first sample and writing it (%u) (first sample) 0x%X\n",symbol_size_in_bits,current);
			/* Skip those two samples */
			buffer+=((2*symbol_size_in_bits)/8);
			buffer_size_in_bits-=(2*symbol_size_in_bits);
			/* Encode the block, -1 comes from 1 less sample in block(the first one) */ 
			encoded_bits_in_block=SAMPLES_PER_BLOCK*symbol_size_in_bits-1;
			run_length_encode(buffer, &encoded_bits_in_block, new_symbol_size_in_bits);
			/* Skip all samples already encoded, -1 comes from less ne block */
			buffer+=(SAMPLES_PER_BLOCK-1)*(symbol_size_in_bits/8);
			/* Sums bits encoded */
			total_bits_encoded += encoded_bits_in_block;
			return;
		}
		
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
	
	memcpy(buffer,compressed_buffer,ceil(ArrayStreamGetBitCount(array_stream_compressed)/8));
	*buffer_size_in_bits = ArrayStreamGetBitCount(array_stream_compressed);
	free(compressed_buffer);
}
