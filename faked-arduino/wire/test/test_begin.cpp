/*****
 *                                                                   
 *                   Searduino
 *                      
 *   Copyright (C) 2012 Henrik Sandklef 
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



#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
  #include "types.h"
  #include "print.h"
}
#endif

#include "Wire.h"




START_TEST (test_begin)
{
  Wire.begin();
  uint8_t a=0;
  uint8_t b=0;

  //  fail_if(Wire.requestFrom(a,b)!=42);
}
END_TEST



Suite *
buffer_suite(void) {
  Suite *s = suite_create("Begin_Fuctions");
  TCase *tc_core = tcase_create("Core");
  suite_add_tcase (s, tc_core);

  printf ("Testing wire begin in faked-arduino/wire\n");

  tcase_add_test(tc_core, test_begin);

  return s;
}

int main(void)
{
  int num_failed;

  Suite *s = buffer_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
