/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : Barometer.h
   Ver nr.     : 1.0
   Description : Mororola MPX5100D                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA     
                 
*************************************************************************************/

#ifndef BAROMETER_H
#define BAROMETER_H
#include "avrheaders.h"
#include "global.h"		
#include "ow.h"
#include "owcrc.h"
#include "ownet.h"
#include "owtran.h"
#include "DS2438.h"
#include "io.h"





void Barometer_init(void);
u16 Barometer_read(void);




#ifdef BAROMETER_C
  u08 OW_DS2438_ID_BAROMETER[8];
#else
#endif
#endif //BAROMETER_H

