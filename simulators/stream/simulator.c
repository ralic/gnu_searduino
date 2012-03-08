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

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <Arduino.h>
#include <pthread.h>

#include "communication/comm.h"
#include "communication/ext_io.h"
#include "arduino/setup.h"


void sim_sighandler(int sig);


pthread_t arduino_thread;

/* 
 * Function to register in the Searduino code
 *
 * This function will be called every time the Arduino program updates a digital output pin (on change only!)
 */
void 
my_do_sim_callback(uint8_t pin, uint8_t val)
{
  printf ("%s:%s(%d:%d)\n",__FILE__, __func__, pin, val);
  printf ("\n");
}

int 
sim_setup(char *ard_lib)
{
  int ret ; 

  searduino_disable_streamed_output();

  searduino_set_arduino_code_name(ard_lib);

  ret = searduino_setup();
  if (ret!=0)
    {
      return ret;
    }
  
  ret  = comm_register_digout_sim_cb(my_do_sim_callback);
  if (ret != SEARD_COMM_OK)
    {
      fprintf (stderr, "Failed to register callback for Digital output (pin, val)\n");
      return ret;
    }
 


  return 0;
}

extern searduino_main_ptr_ptr searduino_main_entry;

void* arduino_code(void *in)
{
  if (searduino_main_entry!=NULL)
    {
      searduino_main_entry(NULL);
    }
  else
    {
      fprintf (stderr, "Couldn't find an entry point for the Arduino code.\n");
      fprintf (stderr, "This probably means you haven't provided searduino with a shared library containing Arduino code\n");
      fflush (stderr);
      printf ("\n");
      printf ("*** Searduino will now try to close the simulator\n");
      printf ("*** If this fails, type 'quit' followed by enter (or press Ctrl-c) ***\n");
      kill(getpid(), SIGUSR1);
    }
  return NULL;
}

void* command_reader(void* in)
{
  char buf[100];
  char *tmp;
  int  pin;
  int  val;

  printf ("COMMAND READER begins\n");
    
  for (;;)
    {
/*       printf ("COMMAND READER reads\n"); */
      fgets(buf, 100, stdin);
/*       printf ("Just read: '%s'\n", buf);  */
      
      if (strncmp(buf,"pin:",4)==0)
	{
	  tmp=&buf[4];
/* 	  printf (": '%s'\n", buf);  */
/*  	  printf ("will parse: '%s'\n", tmp);  */
	  sscanf(tmp, "%d:%d", &pin, &val);
  	  printf ("SIM WILL SET pin:%d val:%d  \n", pin, val);  
	  di_callback(pin,val);
	}
      else if (strncmp(buf,"quit",4)==0)
	{
	  searduino_set_halted();
	  return ;
	}
      else if (strncmp(buf,"pause",5)==0)
	{
	  printf ("Will pause sim\n");
	  searduino_set_paused();
	}
      else if (strncmp(buf,"resume",5)==0)
	{
	  printf ("Will resume sim\n");
	  searduino_set_running();
	}
      else if (strncmp(buf,"quit",4)==0)
	{
	  return ;
	}
      usleep (100);
    }
}


static const char* ARDUINO_CODE_ARG_LONG  = "--arduino-code";
static const char* ARDUINO_CODE_ARG_SHORT = "-ac";
static const char* HELP_ARG_LONG  = "--help";
static const char* HELP_ARG_SHORT = "-h";

static usage(void)
{
  printf ("searduino-stream-sim\n\n");
  printf ("OPTIONS\n");
  printf ("\t%s, %s <library>\n", ARDUINO_CODE_ARG_LONG, ARDUINO_CODE_ARG_SHORT);
  printf ("\t\tspecify what arduino code (shared library) to load\n");
  printf ("\t%s, %s <library>\n", HELP_ARG_LONG, HELP_ARG_SHORT);
  printf ("\t\tprint this text\n");
  printf ("\n");
}

#define ARGCMP(arg, along, ashort) 					\
    ( ( strncmp(arg, along, strlen(along))==0)				\
      ||								\
      ( strncmp(arg, ashort, strlen(ashort))==0))


static void 
close_simulator(pthread_t *t)
{
  static int already_closed = 0;

  if (!already_closed)
    {
      printf ("Waiting for simulator thread to return\n");
      pthread_join(*t, NULL);
      
      usleep(1000*100);
      fflush(stdout);
      printf ("\n *** Simulator will now be closed ***\n");
      fflush(stdout);
      already_closed = 1 ;
    }
}

int 
main(int argc, char **argv)
{
  pthread_t command_thread;
  char *ard_code = "" ;
  int i = 0;

  for (i=1;i<argc;i++)
    {
      if (ARGCMP(argv[i], 
		 ARDUINO_CODE_ARG_LONG, 
		 ARDUINO_CODE_ARG_SHORT))
	{
	  if (argv[2]==NULL)
	    {
	      printf ("Missing argument to %s, %s\n",
		      ARDUINO_CODE_ARG_LONG, ARDUINO_CODE_ARG_SHORT);
	      return 1;
	    }
	  else
	    {
	      ard_code = argv[i+1];
	      i++;
	    }
	}
      else if (ARGCMP(argv[i], 
		 HELP_ARG_LONG, 
		 HELP_ARG_SHORT))
	{
	  usage();
	  return ;
	}
      else
	{
	  usage();
	  return ;
	}

    }

  signal(SIGUSR1, sim_sighandler);

  
  printf ("Using arduino code from library: %s\n",
	  ard_code);

  sim_setup(ard_code);

  pthread_create(&arduino_thread, NULL, arduino_code, NULL);

  command_reader(NULL);

  close_simulator(&arduino_thread);

  return 0;
}


void sim_sighandler(int sig)
{
  close_simulator(&arduino_thread);
  exit(0);
}
