#include <Arduino.h>
#include <searduino.h>
#include <seasim.h>


void setup(void)
{
  pinMode(13, OUTPUT);
}



int main(void)
{
  uint8_t i = 0;
  uint8_t j = 0 ;
  int ctr=0;
  init();

  seasim_set_board_name("leonardo");
  seasim_setup_board();

  seasim_setup();



  setup();

  for(i=0;;i++)
    {    
      /* Set the analog pin 11 & 12, 
	 which is used by get_distance() */
      seasim_fake_analog_input (A0,i);
      seasim_fake_analog_input (A1,j);

      /*      printf ("get_distance()=%.4d  (i:%.4d  j:%.4d  ctr:%.4d)  A0:%.4d  A1:%.4d  (%d %d %s)\n", 
		get_distance(), i, j , ctr++,
		analogRead(A0) , analogRead(A1), A0,  A1, seasim_get_board_name());
      */
      
      if (get_distance()!=(uint8_t)(i+j))
	{
	  printf(" ERROR: %d != %d.   %d succeeded before this one failed\n", 
		 get_distance(), (uint8_t)(i+j), ctr);
	  return 1;
	}
      if (i==255) 	  j++;    
      if (j==255) 	  break ; 
      ctr++;
    }  

  printf ("%d tests passed\n", ctr);
  return 0;
}
