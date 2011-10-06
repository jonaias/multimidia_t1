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

void run_length_encode(uint16_t *buffer,uint32_t *buffer_size){
		uint32_t buffer_index=0;
		uint32_t temp_buffer_index=0;
		uint16_t last;
		uint16_t *temp_buffer;
		temp_buffer = malloc(*buffer_size);
		if (!temp_buffer){
			printf("Cannot allocate memory.\n");
			return;
		} 
		
		last = buffer[buffer_index];
		temp_buffer[temp_buffer_index]= buffer[buffer_index];
		printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);
		buffer_index++;
		temp_buffer_index++;
		
		while(	(buffer_index<(*buffer_size/sizeof(uint16_t)))&&
				(temp_buffer_index<(*buffer_size/sizeof(uint16_t)))
			){
				temp_buffer[temp_buffer_index]=buffer[buffer_index];
				/*printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);*/
				temp_buffer_index++;
				/* Check if its a runlength */
				if (buffer[buffer_index]==last){
					printf("Found carrier\n");
					uint16_t number_of_copy=0;
					buffer_index++;
					/* Counts the run length */
					while((buffer[buffer_index]==last) && (number_of_copy<0xffff) && (buffer_index<(*buffer_size/sizeof(uint16_t)))){
						number_of_copy++;
						buffer_index++;
					}
					/* Write the number of run length found */
					temp_buffer[temp_buffer_index] = number_of_copy;
					/*printf("Runlength %d writen to temp_buffer[%d]\n",number_of_copy,temp_buffer_index);*/
					temp_buffer_index++;
					
					if ((buffer_index<(*buffer_size/sizeof(uint16_t)))){
						/* To avoid 0x0000 0x0000 0xFFFF 0x0000 0xFFFF */
						temp_buffer[temp_buffer_index]=buffer[buffer_index];
						/*printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);*/
						buffer_index++;
						temp_buffer_index++;
					}
				}
				/* Store as last integer 16bit found */
				last = buffer[buffer_index];
				buffer_index++;
		}
		free(buffer);
		buffer = temp_buffer;
		*buffer_size = temp_buffer_index;
		
}


void difference_encode(uint16_t *buffer,uint32_t *buffer_size){
		uint32_t buffer_index=0;
		uint32_t temp_buffer_index=0;
		uint16_t last;
		uint16_t *temp_buffer;
		temp_buffer = malloc(*buffer_size);
		if (!temp_buffer){
			printf("Cannot allocate memory.\n");
			return;
		} 
		
		last = buffer[buffer_index];
		temp_buffer[temp_buffer_index]= buffer[buffer_index];
		printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);
		buffer_index++;
		temp_buffer_index++;
		
		while(	(buffer_index<(*buffer_size/sizeof(uint16_t)))&&
				(temp_buffer_index<(*buffer_size/sizeof(uint16_t)))
			){
				temp_buffer[temp_buffer_index]=buffer[buffer_index];
				/*printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);*/
				temp_buffer_index++;
				/* Check if its a runlength */
				if (buffer[buffer_index]==last){
					printf("Found carrier\n");
					uint16_t number_of_copy=0;
					buffer_index++;
					/* Counts the run length */
					while((buffer[buffer_index]==last) && (number_of_copy<0xffff) && (buffer_index<(*buffer_size/sizeof(uint16_t)))){
						number_of_copy++;
						buffer_index++;
					}
					/* Write the number of run length found */
					temp_buffer[temp_buffer_index] = number_of_copy;
					/*printf("Runlength %d writen to temp_buffer[%d]\n",number_of_copy,temp_buffer_index);*/
					temp_buffer_index++;
					
					if ((buffer_index<(*buffer_size/sizeof(uint16_t)))){
						/* To avoid 0x0000 0x0000 0xFFFF 0x0000 0xFFFF */
						temp_buffer[temp_buffer_index]=buffer[buffer_index];
						/*printf("Copied %d from buffer[%d] to temp_buffer[%d]\n",buffer[buffer_index],buffer_index,temp_buffer_index);*/
						buffer_index++;
						temp_buffer_index++;
					}
				}
				/* Store as last integer 16bit found */
				last = buffer[buffer_index];
				buffer_index++;
		}
		free(buffer);
		buffer = temp_buffer;
		*buffer_size = temp_buffer_index;
		
}
