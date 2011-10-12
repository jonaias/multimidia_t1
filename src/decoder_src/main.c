/*    main.c
      
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
      MA 02110-1301, USA.
*/


#include "decode.h"
#include "../util/util.h"


void print_help(){
	printf("Usage: decode <in_file.wav> <out_file.bin>\n");
	printf("\t<in_file.wav>\t Compressed file\n");
	printf("\t<out_file.bin> \t Original file\n"); 
}
     
     
int main (int argc, char **argv)
{
	FILE *input_file,*output_file;
	int8_t *buffer,**channel_datas,*out_buffer;
	uint32_t *channel_sizes,number_of_channels,bytes_per_sample;
	uint32_t file_size=0,data_start_pos=0,compressed_file_data_size=0,original_file_data_size=0;
	int difference_flag = 0;
	int run_length_flag = 0;
	int huffman_flag = 0;
	int c;
	uint32_t i;

	opterr = 0;

	while ((c = getopt (argc, argv, ":")) != -1)
	 switch (c)
	   {
	   case '?':
		 if (isprint (optopt)){
		   printf ("Unknown option `-%c'.\n", optopt);
		   print_help();
		 }
		 else{ 
		   printf ("Unknown option character `\\x%x'.\n", optopt);
		   print_help();
		 }
		 return -1;
	   default:
		 return -1;
	   }
	   
	if((argc-optind)==2){
		printf("input file = %s, output file = %s\n",argv[optind],argv[optind+1]);
		
		
		/* Open file as binary read only and get its size */
		input_file = fopen(argv[optind], "rb");
		if(input_file != NULL)
		{
			fseek(input_file, 0, SEEK_END);
			file_size = ftell(input_file);
			rewind(input_file);
		}
		else{
			printf("Cannot open input file \"%s\", does it exists?\n",argv[optind]);
			return -1;
		}
		
		
		/* Print inicial file size */
		printf("Original file size: %d bytes\n",file_size);
		
		
		/* Malloc all the needed buffer */
		buffer = (int8_t *) malloc(file_size); 
		if (!buffer){
			printf("Cannot allocate memory.\n");
			return -1;
		}
		
		
		/* Read the whole file */
		fread(buffer,1,file_size,input_file);
		
		
		/*Find how much channels*/
		for(i=0;i<file_size-2;i++){
			/* if buffer[i] == 'data' */
			if ((buffer[i]=='f')&&(buffer[i+1]=='m')&&(buffer[i+2]=='t')){
				/* data_start_pos = buffer[i]=='d' + 8 bytes(ckID + cksize) */
				printf("Number of channes: %u\n",*(uint16_t*)(buffer+i+10));
				number_of_channels = *(uint16_t*)(buffer+i+10);
				break;
			}
		}
		
		/* Find how wav is encoded */
		for(i=0;i<file_size-2;i++){
			/* if buffer[i] == 'data' */
			if ((buffer[i]=='f')&&(buffer[i+1]=='m')&&(buffer[i+2]=='t')){
				/* data_start_pos = buffer[i]=='d' + 8 bytes(ckID + cksize) */
				printf("Encoding format: %x\n",*(uint16_t*)(buffer+i+8));
				/*TODO:Fine tune Format recognition*/
				if (  (*(uint16_t*)(buffer+i+8)&0x000F)  &&  (*(uint16_t*)(buffer+i+8)&0xE001) ){
					printf("Format supported, uncompressing..\n");
				}
				else{
					printf("ERROR->Format not supported, cannot decompress..exiting\n");
					return -1;
				}
				difference_flag = (*(uint16_t*)(buffer+i+8) >> 15)&0x0001;
				run_length_flag = (*(uint16_t*)(buffer+i+8) >> 14)&0x0001;
				huffman_flag = (*(uint16_t*)(buffer+i+8) >> 13)&0x0001;
				*(uint16_t*)(buffer+i+8) &= 0x0001;
				break;
			}
		}
		
		/*Find how  much bits per sample*/
		for(i=0;i<file_size-2;i++){
			/* if buffer[i] == 'data' */
			if ((buffer[i]=='f')&&(buffer[i+1]=='m')&&(buffer[i+2]=='t')){
				/* data_start_pos = buffer[i]=='d' + 8 bytes(ckID + cksize) */
				printf("Number of bits per sample : %u\n",(*(uint16_t*)(buffer+i+22)));
				if (*(uint16_t*)(buffer+i+22) % 8){
					printf("Warning->Sample rate not supported(not divided by 8), assuming 16bits.\n");
					bytes_per_sample = 2;
				}
				else{
					bytes_per_sample = (*(uint16_t*)(buffer+i+22))/8;
				}
				break;
			}
		}
		
		/*Find where data starts*/
		for(i=0;i<file_size-3;i++){
			/* if buffer[i] == 'data' */
			if ((buffer[i]=='d')&&(buffer[i+1]=='a')&&(buffer[i+2]=='t')&&(buffer[i+3]=='a')){
				/* data_start_pos = buffer[i]=='d' + 8 bytes(ckID + cksize) */
				data_start_pos=i+8;
				break;
			}
		}
		
		/*Find original file data size*/
		for(i=0;i<file_size-2;i++){
			/* if buffer[i] == 'data' */
			if ((buffer[i]=='d')&&(buffer[i+1]=='a')&&(buffer[i+2]=='t')&&(buffer[i+3]=='a')){
				/* data_start_pos = buffer[i]=='d' + 8 bytes(ckID + cksize) */
				printf("Original file sample size: %u\n",*(uint32_t*)(buffer+i+4));
				original_file_data_size = (*(uint32_t*)(buffer+i+4));
				break;
			}
		}
		
		/*Write data size*/
		compressed_file_data_size=file_size-data_start_pos;
		
		/*Malloc buffer for each channel and its size*/
		channel_datas =  (int8_t **) malloc(number_of_channels*sizeof(int8_t *));
		channel_sizes = (uint32_t *) malloc(number_of_channels*sizeof(uint32_t));
		for(i=0;i<number_of_channels;i++){
			channel_sizes[i]=(8*compressed_file_data_size)/number_of_channels;
			printf("The size of pre-allocated bytes for uncompressed channel %d is %d\n",i,original_file_data_size/number_of_channels);
			channel_datas[i] = (int8_t *) malloc(original_file_data_size/number_of_channels);/* multiply by 2 just in case of instead compressing, increassing file size */
		}
		
		
		fseek(input_file, data_start_pos, SEEK_SET);
		
		
		for(i=0;i<number_of_channels;i++){
				/* This uint32_t limits max channel size of ~<512mb */
				fread(&channel_sizes[i],1,sizeof(uint32_t),input_file);
				printf("The size of compressed channel %d is %d\n",i,channel_sizes[i]/8);
				fread(channel_datas[i],1,channel_sizes[i]/8,input_file);
		}
		
		fclose(input_file);

		
		if(run_length_flag){
			printf("Run length decoding ...\n");
			for(i=0;i<number_of_channels;i++){
				run_length_decode(channel_datas[i],&channel_sizes[i],1);
			}
		}
		
		
		if(difference_flag){
			printf("Difference decoding ...\n");
			for(i=0;i<number_of_channels;i++){
				difference_decode(channel_datas[i],&channel_sizes[i],bytes_per_sample);
			}
		}
		
		/* Malloc buffer for final file */
		out_buffer = malloc((data_start_pos+original_file_data_size)*sizeof(int8_t));
		
		/* Copy current header with corrected wFormatTag */
		memcpy(out_buffer,buffer,data_start_pos);
		
		
		/* Join data across channels, counting bytes per channel */
		for(i=data_start_pos;i<data_start_pos+original_file_data_size;i++){
			TRACE("i=%d number_of_channels=%d   channel_datas[%d][%d]=buffer[%d](%d)\n",i,number_of_channels,((i-data_start_pos)%(number_of_channels*bytes_per_sample))/bytes_per_sample,(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*number_of_channels))*bytes_per_sample,i,buffer[i]);
			out_buffer[i]=channel_datas[((i-data_start_pos)%(number_of_channels*bytes_per_sample))/bytes_per_sample][(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*number_of_channels))*bytes_per_sample];
		}
				
		/* To test joinning */
		TRACE("The first value byte is %X\n",channel_datas[1][1]);
		
		
		/* Open file as binary write only */
		output_file = fopen(argv[optind+1], "wb");
		if(output_file != NULL)
		{
			fwrite(out_buffer,1,data_start_pos+original_file_data_size,output_file);
			printf("Final file size: %lu bytes\n", ftell(output_file));
			fclose(output_file);
		}
		
		else{
			printf("Cannot open output file \"%s\", does it exists?\n",argv[optind]);
			return -1;
		}
		
		
		free(buffer);
		return 0;
	
	}
	else{
		printf("Incorrect number of parameters.\n");
		print_help();
		return -1;
	}


	return 0;
}
