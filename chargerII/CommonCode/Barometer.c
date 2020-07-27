/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : Barometer.c
   Ver nr.     : 1.0
   Description : Mororola MPX5100D
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA  
               
 
*************************************************************************************/

#define BAROMETER_C
#include "barometer.h"



// *****************************************
// Initialize the Barometer
// *****************************************
void Barometer_init(void)
{
  // Find the pressure sensor's DS2438 on the one wire bus
  owScanforDevices(OW.Port, 1, &OW_DS2438_ID_BAROMETER, OW_FAMILY_DS2438);
  owPrintOW_ROMID(OW_DS2438_ID_BAROMETER); 

  DS2438_init(OW_DS2438_ID_BAROMETER);
}


// *****************************************
// Initialize the Barometer
// *****************************************
u16 Barometer_read(void)
{
  Wind_powerpin(1);  

#define MPX5100_VOLTAGE 4790 
#define MPX5100_CALIBRATION_ERROR 11

  u32 tmp = DS2438_read_Vad(OW_DS2438_ID_BAROMETER);  
  //print_decimal_u16(tmp);
  //rprintf("\r\n");
  tmp = ((tmp*1111)/MPX5100_VOLTAGE) + 105 - MPX5100_CALIBRATION_ERROR; // from datasheet
  return((u16)tmp);
  //return(DS2438_read_Vad(OW_DS2438_ID_BAROMETER));  
  //Wind_powerpin(0);  
  
}
