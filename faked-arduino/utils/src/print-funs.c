/*****
 *                                                                   
 *                   Searduino
 *                      
 *   Copyright (C) 2011, 2012 Henrik Sandklef 
 *                                                                   
 * This program is free software; you can redistribute it and/or     
 * modify it under the terms of the GNU General Public License       
 * as published by the Free Software Foundation; either version 3    
 * of the License, or any later version.                             
 *                                                                   
 *                                                                   
 * This program is distributed in the hope that it will be useful,   
 * but WITHOUT ANY WARRANTY; without even the implied warranty of    
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     
 * GNU General Public License for more details.                      
 *                                                                   
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software       
 * Foundation, Inc., 51 Franklin Street, Boston,            
 * MA  02110-1301, USA.                                              
 ****/

#include "stdio.h"
#include "stdarg.h"

void 
print_function_name (FILE* stream,  const char *fun, char * str,  ...) 
{
  va_list ap;
  char buffer[256];

  va_start(ap, str);
  fprintf (stderr, "%s (", fun);
  vsprintf (buffer, str, ap);
  fprintf (stderr,"%s)\n",buffer);
  va_end(ap);

  return;
}


void 
print_dummy_function_implementation (FILE* stream,  
				     const char *file, 
				     int line, 
				     const char *fun) 
{
  fprintf (stream, "%s:%d %s ()", file, line, fun);
  fprintf (stream," - stub implementation.  *** WARNING, this function has no real implementation ***\n");
  return;
}