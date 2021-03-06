/*****
 *                                                                   
 *                   Searduino
 *                      
 *   Copyright (C) 2011-2015 Henrik Sandklef 
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

#define USBCON
#define CDC_ENABLED

#include "Platform.h"
#include "Stream.h"
#include "USBAPI.h"

#include "utils/print.h"

#if defined(USBCON)
#ifdef CDC_ENABLED

static int cdc_serial_initialised = 0;



void Reboot()
{
  printf ("Reboot called in CDC.cpp\n");
}

int CDC_GetInterface(u8* interfaceNum)
{
  return 0;
}

bool CDC_Setup(Setup& setup)
{
  return false;
}

int _serialPeek = -1;
void Serial_::begin(uint16_t baud_count)
{
  cdc_serial_initialised = 1;
}

void Serial_::end(void)
{
  ;
}

void Serial_::accept(void) 
{
  ;
}

int Serial_::available(void)
{
  return USB_Available(CDC_RX);
}

//	peek is nasty
int Serial_::peek(void)
{
  return 0;
}

int Serial_::read(void)
{
  return 0;
}

void Serial_::flush(void)
{
  ;
}

size_t Serial_::write(uint8_t c)
{
  if ( cdc_serial_initialised )
    {
      serial_print_c(c);
    }
  else
    {
      fprintf(stderr, "You need to call begin before using the write functions\n");
    }
  return 1;
}

Serial_ Serial;


#endif
#endif /* if defined(USBCON) */
