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

#include "Arduino.h"
#include "searduino.h"
#include "searduino_pin.h"
#include "boards.h"
#include "stdio.h"

typedef int   (*board_setup_ptr)(void); 
static int board_index=0;

const char *SEARDUINO_BOARD_UNO_NAME = "Uno";

enum   
  {
    SEARDUINO_BOARD_UNSET,
    SEARDUINO_BOARD_UNO,
    SEARDUINO_BOARD_MEGA,
    SEARDUINO_BOARD_MEGA_2560,
    SEARDUINO_BOARD_LAST
  };

struct searduino_board_t
{
  int             id;
  char           *name;
  board_setup_ptr setup ;
} ;




static struct searduino_board_t searduino_boards[] = 
  {
    { SEARDUINO_BOARD_UNSET, 
      "none",
      NULL
    },
    { SEARDUINO_BOARD_UNO, 
      "Uno",
      board_setup_uno
    },
    { SEARDUINO_BOARD_MEGA, 
      "Mega",
      NULL
    },
    { SEARDUINO_BOARD_MEGA_2560, 
      "Mega2560",
      board_setup_mega2560
    },
    { SEARDUINO_BOARD_LAST, 
      NULL,
      NULL
    }
  };


char *
get_board_name(void)
{
  /*
   * print warning in case board not set
   * 
   */
  if ( ( board_index <= SEARDUINO_BOARD_UNSET ) ||
       ( board_index >= SEARDUINO_BOARD_LAST ) )    
    {
      fprintf(stderr, "No board name set\n");
      return NULL;
    }

  /*  printf ("board index: %d\n", board_index);
  printf ("board name:  %p\n", searduino_boards[board_index].name);
  printf ("board name:  %s\n", searduino_boards[board_index].name);
  */
  return searduino_boards[board_index].name;
}


int
set_board_name(char *board)
{
  int i ;
  board_index = 0;

  if (board==NULL)
    {
      printf ("Invalid board name (NULL)\n");
      return 0;
    }
  
  for (i=1;i<SEARDUINO_BOARD_LAST;i++)
    {
      printf ("   checking %d '%s' for settings\n", i, searduino_boards[i].name);
      if (searduino_boards[i].name==NULL)
	{
	  fprintf(stderr, "Major internal error: %s:%d (%s) when setting board to '%s'\n", 
		  __FILE__, __LINE__, __func__, board);
	  return 0;
	}
      if ( (strncasecmp(board, 
		      searduino_boards[i].name, 
		      strlen(searduino_boards[i].name))==0)
	   && (strlen(searduino_boards[i].name) == strlen(board)))
	{
	  board_index = i;
	  return i;
	}
    }

  printf ("Could not match board name %s\n", board);
  return board_index;
}


int
board_setup(void)
{
  if ( ( board_index <= SEARDUINO_BOARD_UNSET ) ||
       ( board_index >= SEARDUINO_BOARD_LAST ) )    
    {
      fprintf(stderr, "No board name set, defaulting to Uno\n");
      board_index = SEARDUINO_BOARD_UNO;
    }
  
  if ( searduino_boards[board_index].setup == NULL )
    {
      fprintf(stderr, "Missing setup function for '%s'\n", 
	      searduino_boards[board_index].name);
      return 1;
    }

  return searduino_boards[board_index].setup();
}

static void print_board_digital_pins(char *s, int type)
{
  int i ; 
  printf ("%s", s);
  for (i=0;i<NR_OF_ARDUINO_PINS;i++)
    {
      if ( get_generic_pin_type(i) == type)
	{
	  printf ("%d, ", i);
	}
    }
  printf ("\n");
}

static void print_board_analog_pins(void)
{
  int i ; 
  int j ; 
  int   apins[]     = { A0, A1, A2, A3, A4, A5, A6, A7} ;
  char* apins_str[] = { "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7"} ;
 

  printf ("Analog pins:  ");
  for (i=0;i<NR_OF_ARDUINO_PINS;i++)
    {
      /* printf (" *%d \n", i); */
      if ( get_generic_pin_type(i) == SEARDUINO_PIN_TYPE_ANALOG )
	{
	  printf ("%d (A%d), ", i, i - A0);
	  /*for (j=0;j<8;j++)
	    {
	      if (apins[j]==i)
		{
		  printf (" (%s)", apins_str[j]);
		  break;
		}
	    }
	    printf (", "); */
	}
    }
  printf ("\n");
}

void 
print_board_setup(void)
{
  printf ("\n");
  printf ("Board settings:\n");
  printf ("---------------\n");
  printf ("Name:         %s\n", get_board_name());
  print_board_digital_pins("Digital pins: ", SEARDUINO_PIN_TYPE_DIGITAL);
  print_board_digital_pins("PWM pins:     ", SEARDUINO_PIN_TYPE_PWM);
  print_board_analog_pins();
  printf ("\n");
}

int
board_setup_uno(void)
{

  /*
    From the Arduino Uno page:

    "It has 14 digital input/output pins (of which 6 can be used as PWM outputs), 6 analog inputs,"

    PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite() function. 

    LED: 13. There is a built-in LED connected to digital pin 13. When the pin is HIGH value, the LED is on, when the pin is LOW, it's off. 

   */
  A0 = 14;
  A1 = 15;
  A2 = 16;
  A3 = 17;
  A4 = 18;
  A5 = 19;

  /*
   *    6 PWM pins
   */
  set_generic_pin_type(3,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(5,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(6,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(9,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(10, SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(11, SEARDUINO_PIN_TYPE_PWM);

  /* 
   *   Digital pins
   */
  set_generic_pin_type(1,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(2,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(4,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(7,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(8,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(12, SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(13, SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(0, SEARDUINO_PIN_TYPE_DIGITAL);

  set_generic_pin_type(A0, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A1, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A2, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A3, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A4, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A5, SEARDUINO_PIN_TYPE_ANALOG);
  
  

  printf ("\n\t*** UNO BOARD SETUP done\n\n");
  print_board_setup();
  return 0;
}


int
board_setup_mega2560(void)
{
  int i;
  /*
    From the Arduino Mega2560 page:

    It has 54 digital input/output pins
        (of which 14 can be used as PWM outputs), 16 analog inputs

    PWM: 2 to 13 and 44 to 46. 
         Provide 8-bit PWM output with the analogWrite() function.

    LED: 13. There is a built-in LED connected to digital pin 13. When the pin is HIGH value, the LED is on, when the pin is LOW, it's off. 

   */
  
  A0  = 54;
  A1  = 55;
  A2  = 56;
  A3  = 57;
  A4  = 58;
  A5  = 59;
  A6  = 60;
  A7  = 61;
  A8  = 62;
  A9  = 63;
  A10 = 64;
  A11 = 65;
  A12 = 66;
  A13 = 67;
  A14 = 68;
  A15 = 69;

  /* Set Digital pin type */
  set_generic_pin_type(0,  SEARDUINO_PIN_TYPE_DIGITAL);
  set_generic_pin_type(1,  SEARDUINO_PIN_TYPE_DIGITAL);
  for(i = 14; i < A0; i++) {
    set_generic_pin_type(i,  SEARDUINO_PIN_TYPE_DIGITAL);
  }

  /* Set PWM pin type */
  for(i = 2; i <= 13; i++) {
    set_generic_pin_type(i,  SEARDUINO_PIN_TYPE_PWM);    
  }
  set_generic_pin_type(44,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(45,  SEARDUINO_PIN_TYPE_PWM);
  set_generic_pin_type(46,  SEARDUINO_PIN_TYPE_PWM);

  /* Set Analog pin type */
  set_generic_pin_type(A0,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A1,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A2,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A3,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A4,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A5,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A6,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A7,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A8,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A9,  SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A10, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A11, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A12, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A13, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A14, SEARDUINO_PIN_TYPE_ANALOG);
  set_generic_pin_type(A15, SEARDUINO_PIN_TYPE_ANALOG);


  printf ("\n\t*** MEGA2560 BOARD SETUP done\n\n");
  print_board_setup();
  return 0;
}



int
board_setup_leonardo(void)
{
  int i;
  /*
    From the Arduino Leonardo page[1]:

    It has 20 digital input/output pins 
       (of which 7 can be used as PWM outputs and 12 as analog inputs),

    PWM: 3, 5, 6, 9, 10, 11, and 13. 
       Provide 8-bit PWM output with the analogWrite() function. 

    LED: 13. There is a built-in LED connected to digital pin 13. 
       When the pin is HIGH value, the LED is on, when the pin is LOW, it's off. 

    Analog Inputs: A0-A5, A6 - A11 (on digital pins 4, 6, 8, 9, 10, and 12). 
       The Leonardo has 12 analog inputs, labeled A0 through A11, all
       of which can also be used as digital i/o. Pins A0-A5 appear in
       the same locations as on the Uno; inputs A6-A11 are on digital
       i/o pins 4, 6, 8, 9, 10, and 12 respectively. Each analog input
       provide 10 bits of resolution (i.e. 1024 different values). By
       default the analog inputs measure from ground to 5 volts,
       though is it possible to change the upper end of their range
       using the AREF pin and the analogReference() function.

    There are a couple of other pins on the board:

    AREF. Reference voltage for the analog inputs. Used with analogReference().
    Reset. Bring this line LOW to reset the microcontroller. Typically used to add a reset button to shields which block the one on the board. 

   [1] http://arduino.cc/en/Main/ArduinoBoardLeonardo
   */
  

  return 0;
}