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

#define READ_SIZE 1024


void print_help(){
	printf("Usage: decode -d -c -h <arquivo.wav> <saida.bin>\n");
	printf("\tdecode \t\t nome do codificador\n");
	printf("\t-d \t\t utilizacao da codificacao por diferenças\n");
	printf("\t-c \t\t utilizacao da codificacao por carreira\n");
	printf("\t-h \t\t utilizacao da codificacao Huffman\n");
	printf("\t<entrada.wav>\t arquivo original sem compressao\n");
	printf("\t<saida.bin> \t arquivo binario comprimido\n"); 
	printf("At least one codification and two file parameters must be specified.\n");
}
     
int main (int argc, char **argv)
{
	FILE *input_file;
	uint16_t *buffer;
	uint32_t buffer_size;
	int dflag = 0;
	int cflag = 0;
	int hflag = 0;
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "cdh:")) != -1)
	 switch (c)
	   {
	   case 'd':
		 dflag = 1;
		 break;
	   case 'c':
		 cflag = 1;
		 break;
	   case 'h':
		 hflag = 1;
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
	   
	if((dflag+cflag+hflag)&&((argc-optind)==2)){
		printf ("dflag = %d, cflag = %d, hflag = %d\n",dflag, cflag, hflag);
		printf("input file = %s, output file = %s\n",argv[optind],argv[optind+1]);

		input_file = fopen(argv[optind], "rb");
		if(input_file != NULL)
		{
			fseek(input_file, 0, SEEK_END);
			buffer_size = ftell(input_file);
			fseek(input_file, 0, SEEK_SET);
		}
		else{
			printf("Cannot open file \"%s\", does it exists?\n",argv[optind]);
			return -1;
		}
		
		printf("File size %d\n",buffer_size);
		
		/* Malloc all the needed buffer */
		buffer = (uint16_t *) malloc(buffer_size); 
		if (!buffer){
			printf("Cannot allocate memory.\n");
			return -1;
		} 
		
		fread(buffer,READ_SIZE,buffer_size/READ_SIZE+1,input_file);
		printf("First byte %d\n",buffer[0]);
		
		if(cflag){
			run_length_decode(buffer,&buffer_size);
		}
		
		if(dflag){
			difference_decode(buffer,&buffer_size);
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
