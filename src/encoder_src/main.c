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


#include "encode.h"
#include "../util/util.h"


void print_help(){
	printf("Usage: encode -d -c -h <in_file.wav> <out_file.bin>\n");
	printf("\tEncode flag\t Name of encoding\n");
	printf("\t-d \t\t For difference encoding\n");
	printf("\t-c \t\t For run length encoding\n");
	printf("\t-h \t\t For Huffman encoding\n");
	printf("\t<in_file.wav>\t Original uncompressed file\n");
	printf("\t<out_file.bin> \t Compressed file\n"); 
	printf("At least one codification and two file parameters must be specified.\n");
}
     
     
int main (int argc, char **argv)
{
	FILE *input_file,*output_file;
	int8_t *buffer,**channel_datas;
	uint32_t *channel_sizes,number_of_channels,bytes_per_sample;
	uint32_t file_size=0,data_start_pos=0,data_size=0;
	int difference_flag = 0;
	int run_length_flag = 0;
	int huffman_flag = 0;
	int c;
	uint32_t i;

	opterr = 0;

	while ((c = getopt (argc, argv, "cdh")) != -1)
	 switch (c)
	   {
	   case 'd':
		 difference_flag = 1;
		 break;
	   case 'c':
		 run_length_flag = 1;
		 break;
	   case 'h':
		 huffman_flag = 1;
		 break;
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
	   
	if((difference_flag+run_length_flag+huffman_flag)&&((argc-optind)==2)){
		printf("difference_flag = %d, run_length_flag = %d, huffman_flag = %d\n",difference_flag, run_length_flag, huffman_flag);
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
		fclose(input_file);
		
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
				if (*(uint16_t*)(buffer+i+8) != 0x0001){
					printf("Warning->Format not supported, assuming PCM.\n");
				}
				*(uint16_t*)(buffer+i+8) |= (difference_flag<<15) | (run_length_flag<<14) | (huffman_flag<<13); 
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
		
		/*Write data size*/
		data_size=file_size-data_start_pos;
		
		
		/*Malloc buffer for each channel and its size*/
		channel_datas =  (int8_t **) malloc(number_of_channels*sizeof(int8_t *));
		channel_sizes = (uint32_t *) malloc(number_of_channels*sizeof(uint32_t));
		for(i=0;i<number_of_channels;i++){
			channel_sizes[i]=(8*data_size)/number_of_channels;
			channel_datas[i] = (int8_t *) malloc((data_size/number_of_channels)*2*sizeof(int8_t));/* multiply by 2 just in case of instead compressing, increassing file size */
		}
		
		/* Split data across channels, counting bytes per channel */
		for(i=data_start_pos;i<file_size;i++){
			TRACE("i=%d number_of_channels=%d   channel_datas[%d][%d]=buffer[%d](%d)\n",i,number_of_channels,((i-data_start_pos)%(number_of_channels*bytes_per_sample))/bytes_per_sample,(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*number_of_channels))*bytes_per_sample,i,buffer[i]);
			channel_datas[((i-data_start_pos)%(number_of_channels*bytes_per_sample))/bytes_per_sample][(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*number_of_channels))*bytes_per_sample]=buffer[i];
		}
				
		/* To test splitting */
		TRACE("The first value byte is %X\n",channel_datas[1][1]);
		


		if(difference_flag){
			for(i=0;i<number_of_channels;i++){
				difference_encode(channel_datas[i],&channel_sizes[i],bytes_per_sample);
			}
		}
		
		if(run_length_flag){
			for(i=0;i<number_of_channels;i++){
				run_length_encode(channel_datas[i],&channel_sizes[i],difference_flag?0:bytes_per_sample*8);
			}
		}
		
		
		/* Open file as binary write only */
		output_file = fopen(argv[optind+1], "wb");
		if(output_file != NULL)
		{
			fwrite(buffer,1,data_start_pos,output_file);
			for(i=0;i<number_of_channels;i++){
				/* This uint32_t limits max channel size of ~<512mb */
				fwrite(&channel_sizes[i],1,sizeof(uint32_t),output_file);
				fwrite(channel_datas[i],1,channel_sizes[i]/8,output_file);
			}
			printf("Final file size: %lu bytes\n", ftell(output_file));
			fclose(output_file);
		}
		
		else{
			printf("Cannot open output file \"%s\", does it exists?\n",argv[optind]);
			return -1;
		}
		
		for(i=0;i<number_of_channels;i++){
			free(channel_datas[i]);
		}
		free(channel_sizes);
		free(channel_datas);
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
