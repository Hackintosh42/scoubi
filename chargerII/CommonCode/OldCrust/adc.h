

/*********************************** MODULE INFO ****************************
	
   File name    : Adc.h
   Ver nr.      : 1.0
   Description  : Routines to sread the ADC channels
   Compiler     : GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-08-14  Initial 
 
****************************************************************************/
#ifndef ADC_H
#define ADC_H
#include "headers.h"

#define ADC_STEP 48   // 4.888mV      // 5V/1024
#define STEP_DIV 10


#define ADC_RSSI 0x06

u16 Adc_Read(u08 chan);
u16 Adc_ReadRSSI_u16(void);
u08 Adc_ReadRSSI_u08(void);


#endif //ADC_H
