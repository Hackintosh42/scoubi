
/*********************************** MODULE INFO ****************************
	
   File name    : uartQs.h
   Ver nr.      : 1.0
   Description  : Module that handles the receiving of data over the uart. It is a high-level abstraction
   Compiler     : AVR GCC
   Company      : Fairbridge Technologies
   Author       : Jaco Raubenheimer
 
   Change log   : 2005-06-02  Initial 
 
****************************************************************************/
#ifndef UARTQS_H
#define UARTQS_H
#include "avrheaders.h"
#include "global.h"		
#include "uart.h"


#define MSG_BUFFER_LENGTH 				(RX_SERIAL_BUFFER_SIZE - 4) //msg length should not be longer than 35


typedef struct
{
  volatile u08 New;
	u08 Len;
	u08 Msg[MSG_BUFFER_LENGTH];
}sMESSAGE_Q;


u08 UartQs_isInvalidChar(u08 theChar);
u08 UartQs_ContainsInvalidChars(u08 *msg);
void UartQs_Copyto_MESSAGE_Q(u08 * sourceBuffer);
void UartQs_Q_AddMsg(u08 *Msg_ptr,u08 len);
u08 *UartQs_Q_GetMsg(void);


//************************************
// Added by NIS
//************************************
u08 Check_Valid_CommandMsg(u08 *src_ptr,u08 len);
u08 Q_Check(void);
sMESSAGE_Q *Q_GetMsg(void);
u08 Q_AddMsg(u08 *ptr,u08 len);




u08 firstTimeSerial; // variable to make sure that 1st serialport message is also valid


#ifdef UARTQS_C
	sMESSAGE_Q   							MESSAGE_Q;			
#else
  extern	sMESSAGE_Q   	    MESSAGE_Q;
#endif 

#endif



