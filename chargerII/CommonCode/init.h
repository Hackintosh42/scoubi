/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : Init.h
   Ver nr.     : 1.0
   Description : Initialize the Tag hardware
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-05    JvA     
                 
*************************************************************************************/

#ifndef INIT_H
#define INIT_H
#include "avrheaders.h"
#include "global.h"		

#include "i2c.h"		
#include "ds620.h"
#include "modem.h"
#include "network.h"
#include "ClimatagProtocol.h"
#include "time.h"
#include "uart.h"
#include "timers.h"
#include "hardwareinterrupts.h"




void InitPorts(void);
void InitWireless(void);
void InitTag(void);
void Init_LinkType(u08 modemlinktype,u08 phylayer);




#endif //INIT

