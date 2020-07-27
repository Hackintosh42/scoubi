
/*********************************** MODULE INFO ****************************
	
   File name    : Link.h
   Ver nr.      : 1.0
   Description  : Link Layer Protocol functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt
 
   Change log   : 2005-04-17  Initial 
 
****************************************************************************/


#ifndef LINK_H
#define LINK_H
#include "avrheaders.h"
#include "global.h"		
#include "modem.h"
#include "uart.h"


enum{
  LINK_PHY_RS232 =0,
  LINK_PHY_RF_AND_RS232,
  LINK_PHY_RF
};

enum{
LINK_CLIMATAG_FORMAT,
LINK_CARRIAGERETURN_FORMAT,
};



#define LINK_MAX_BUF_LEN MODEM_MAX_TX_MSG_LEN


typedef struct
{
   u08 phy;   // Which Physical layer the message came from 
   u08 rssi;   
   u08 len;
   u08 buf [LINK_MAX_BUF_LEN];
}sLinkMsg;



typedef struct{ 
  u08 phylayer;
  u08 MessageFormat;
  sLinkMsg  LinkMsg;
}sLink;


sLinkMsg *Link_GetNextMsgPtr(void);
u08 Link_Thread(void);

void link_setphy(u08 phy);
void link_send(u08 *srcptr,u08 len);

/*
void link_send(void);
void link_clear(void);
void link_printfChar(u08 data);
void link_printf(u08 *string);
void link_printf_rom(const u08 *string);
void link_printfu04(unsigned char data);
void link_printfu08(u08 data);
void link_printfu16(u16 data);
void link_printfu32(u32 data);
*/



#ifdef LINK_C
sLink Link;  
#else
extern sLink Link;  
#endif





#endif //LINK_H

