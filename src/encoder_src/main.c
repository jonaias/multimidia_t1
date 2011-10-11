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
     
     
#define  channel_number 2
#define bytes_per_sample 2
     
int main (int argc, char **argv)
{
	FILE *input_file;
	int8_t *buffer,**buffer_channels;
	uint32_t file_size=0,data_start_pos=0,max_difference=0,data_size=0;
	int difference_flag = 0;
	int run_length_flag = 0;
	int huffman_flag = 0;
	int c;
	uint32_t i;

	opterr = 0;

	while ((c = getopt (argc, argv, "cdh:")) != -1)
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
		TRACE("difference_flag = %d, run_length_flag = %d, huffman_flag = %d\n",difference_flag, run_length_flag, huffman_flag);
		TRACE("input file = %s, output file = %s\n",argv[optind],argv[optind+1]);
		
		
		/* Open file as binary read only and get its size */
		input_file = fopen(argv[optind], "rb");
		if(input_file != NULL)
		{
			fseek(input_file, 0, SEEK_END);
			file_size = ftell(input_file);
			rewind(input_file);
		}
		else{
			printf("Cannot open file \"%s\", does it exists?\n",argv[optind]);
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
		
		
		/*Find where data starts*/
		fread(buffer,1,file_size,input_file);
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
		
		
		/*Malloc buffer for each channel*/
		buffer_channels =  (int8_t **) malloc(channel_number*sizeof(int8_t *));
		for(i=0;i<channel_number;i++)
			buffer_channels[i] = (int8_t *) malloc((data_size/channel_number)*2*sizeof(int8_t));/* multiply by 2 just in case of instead compressing, increassing file size */
		
		/* Split data across channels, counting bytes per channel */
		for(i=data_start_pos;i<data_size;i++){
			TRACE("i=%d channel_number=%d   buffer_channels[%d][%d]=buffer[%d](%d)\n",i,channel_number,((i-data_start_pos)%(channel_number*bytes_per_sample))/bytes_per_sample,(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*channel_number))*bytes_per_sample,i,buffer[i]);
			buffer_channels[((i-data_start_pos)%(channel_number*bytes_per_sample))/bytes_per_sample][(i-data_start_pos)%bytes_per_sample+((i-data_start_pos)/(bytes_per_sample*channel_number))*bytes_per_sample]=buffer[i];
		}
				
		/* To test splitting */
		TRACE("O valor do primeiro byte é %X\n",buffer_channels[1][1]);
		


		if(run_length_flag){
			int channel_size_in_bits=(data_size*8)/(channel_number*bytes_per_sample);
			run_length_encode(buffer_channels[0],&data_size,channel_size_in_bits);
		}
		
		if(difference_flag){
			/* Testing */
			for(i=data_start_pos;i<file_size/2-2;i++){
				if ((abs(buffer[i+2]-buffer[i]))>abs(max_difference)){
					max_difference=abs(buffer[i+2]-buffer[i]);
				}
			}
			/*difference_encode(&buffer[data_start_pos],&data_size);*/
		}
		
		
		
		free(buffer);
		fclose(input_file);
	
	}
	else{
		printf("Incorrect number of parameters.\n");
		print_help();
		return -1;
	}


	return 0;
}
