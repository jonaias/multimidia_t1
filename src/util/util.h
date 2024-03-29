/*
 * util.h
 * 
 * Copyright 2011  <jonaias@jonaias-laptop>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
 /*
  * Use it as Makefile flag
  * */
  
#include <math.h>

#define SAMPLES_PER_BLOCK 1024
  
#define NDEBUG

#ifndef NDEBUG
#define TRACE printf
#else
#define TRACE(...)
#endif

#define NDEBUG_RUN_LENGTH

#ifndef NDEBUG_RUN_LENGTH
#define TRACE_RUN_LENGTH printf
#else
#define TRACE_RUN_LENGTH(...)
#endif


/*
 *  Need to adapt 
 * http://michael.dipperstein.com/bitlibs/
 * 
*/
