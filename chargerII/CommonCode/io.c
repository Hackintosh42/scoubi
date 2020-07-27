/*********************************** MODULE INFO ****************************
	
   File name    : New_Comms.c
   Ver nr.      : 1.0
   Description  : Communications protocols for data transmission 
				   and reception over Melexis RF connection
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/
#define IO_C
#include "io.h"

//**************************************************
// Sets power to the wind sensors
//**************************************************     
void Wind_powerpin(u08 pwr)
{

  DDRC = (DDRC | (1<<PC2)); // Make sure it's an output

#ifdef TRANSISTOR_BC_807
	if(!pwr)
#else
	if(pwr)  
#endif  
	{
		PORTC = (PORTC | 0x04);
	}
	else
	{
		PORTC = PORTC & (~(0x04));
	}
}


//**************************************************
// Read the Pins to check if we should deep sleep
// 
//**************************************************
u08 IO_Read_SleepJumper(void)
{  
  // MAke IO2 the ground
  // Make IO1 weak pull up and input
  DEBUG2_SETOUTPUT;
  DEBUG1_SETINPUT;
  
  DEBUG1_1;
  DEBUG2_1;

  if(PINC&(1<<IOPIN_1))
  {
    return(0);
  }
  else
    return(1);
}







#ifdef SWITCHES  
  //**************************************************
  // Read the switch
  // 
  //**************************************************
  void Init_Switch(void)
  {
  
    DDRC    = DDRC &(0xFD); //IO1 is input
    PORTC   = PORTC | 0x02; // Pull up resistor on portc.0
  }
  
  
  
  //**************************************************
  // Read MISO
  // 
  //**************************************************
  u08 Read_MISO(void)
  {  
    // Remeber to Configure MISO as an input
    // Pull down occurs from MOSI
    DDRB = DDRB & ~(0x10);
    
    if((PINB&0x10) == 0x00)  
    {
      return(1);
    }
    else
      return(0);
  }



  //**************************************************
  // Read the switch
  // 
  //**************************************************
  u08 Read_Switch(void)
  {  
    if(PINC&0x02)  
    {
      return(1);
    }
    else
      return(0);
  }
  
   
   
  //************************************************
  // Debug Routine sets or clears the Receiver RSSI line
  //  connected to IO0 (PORTC bit 0) during debug
  //*************************************************
  void IO_SetRSSI_dbg(u08 val)
  {
    //IO_SetIO2_dbg(val);
  }

#endif

