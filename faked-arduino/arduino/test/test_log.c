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
#include "Arduino.h"
#include "searduino.h"

START_TEST (test_inc_log)
{
  int level ;

  level = searduino_get_log_level();
  fail_if(level!=0);

  level = searduino_inc_log_level();
  fail_if(level!=1);
}
END_TEST

START_TEST (test_dec_log)
{
  int level ;
  int ret;

  /* reset to 0 */
  searduino_set_log_level(0);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=0);

  level = searduino_get_log_level();
  fail_if(level!=0);

  level = searduino_dec_log_level();
  fail_if(level!=0);

  /* set to 3 */
  searduino_set_log_level(3);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=3);

  level = searduino_dec_log_level();
  fail_if(level!=2);

  level = searduino_dec_log_level();
  fail_if(level!=1);

  level = searduino_dec_log_level();
  fail_if(level!=0);

  level = searduino_dec_log_level();
  fail_if(level!=0);



}
END_TEST


START_TEST (test_set_log)
{
  int level ;
  int ret   ;

  /* Set to 3 */
  ret = searduino_set_log_level(3);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=3);

  /* Set to 0 */
  ret = searduino_set_log_level(0);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=0);

  /* Set to 7 */
  ret = searduino_set_log_level(7);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=7);
  
  /* reset to 0 */
  searduino_set_log_level(0);
  fail_if(ret!=0);
  level = searduino_get_log_level();
  fail_if(level!=0);

  /* Set to 11 - should fail */
  ret = searduino_set_log_level(11);
  fail_if(ret==0);
  level = searduino_get_log_level();
  fail_if(level!=0);

  /* Set to -11 - should fail */
  ret = searduino_set_log_level(-11);
  fail_if(ret==0);
  level = searduino_get_log_level();
  fail_if(level!=0);

}
END_TEST



START_TEST (test_log_msg)
{
  int level ;
  int ret; 

  ret = searduino_set_log_level(SEARDUINO_LOG_NONE);
  fail_if(ret!=0);

  ret = searduino_log((SEARDUINO_LOG_DEBUG, 
		 "\t\t\t *** This should NOT be seen during tests ****\n"));
  fail_if(ret!=0);


  ret = searduino_set_log_level(SEARDUINO_LOG_ERROR);
  fail_if(ret!=0);

  ret = searduino_log((SEARDUINO_LOG_DEBUG, 
		 "\t\t\t *** This should NOT be seen during tests ****\n"));
  fail_if(ret!=0);
  ret = searduino_log((SEARDUINO_LOG_ERROR, 
		 "\t\t\t *** This should be seen during tests ****   :) \n"));
  fail_if(ret==0);


  ret = searduino_set_log_level(SEARDUINO_LOG_DEBUG);
  fail_if(ret!=0);

  ret = searduino_log((SEARDUINO_LOG_DEBUG, 
		 "\t\t\t *** This should be seen during tests ****   :) \n"));
  fail_if(ret==0);

  ret = searduino_set_log_level(SEARDUINO_LOG_DEBUG);
  fail_if(ret!=0);

  ret = searduino_log((SEARDUINO_LOG_ERROR, 
		 "\t\t\t *** This should be seen during tests ****   :) \n"));
  fail_if(ret==0);

}
END_TEST



Suite *
buffer_suite(void) {
  Suite *s = suite_create("Log_Fuctions");
  TCase *tc_core = tcase_create("Core");
  suite_add_tcase (s, tc_core);

  printf ("Testing logging functions in faked-arduino/arduino\n");

  tcase_add_test(tc_core, test_set_log);
  tcase_add_test(tc_core, test_inc_log);
  tcase_add_test(tc_core, test_dec_log);
  tcase_add_test(tc_core, test_log_msg);

  return s;
}

int main(void)
{
  int num_failed;
  //  test_micros();

  searduino_set_arduino_code_name("../../../extensions/arduino-lib/.libs/libarduino-code.so");

  Suite *s = buffer_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  /*   test_delay(); */

  return 0;
}