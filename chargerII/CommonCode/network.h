
/*********************************** MODULE INFO ****************************
	
   File name    : Network.h
   Ver nr.      : 1.0
   Description  : Network Layer Protocol functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt
 
   Change log   : 2005-06-29  Initial 
 
****************************************************************************/


#ifndef NETWORK_H
#define NETWORK_H
#include "avrheaders.h"
#include "global.h"		
#include "link.h"
#include "uart.h"
#include "uartQs.h"


#define NET_MAX_BUF_LEN LINK_MAX_BUF_LEN

#define NET_ACK 1
#define NET_NOACK 0

typedef struct{ 
    u16 LocalAddr;
    u16 DestAddr;
    u08 NetID;
    u08 index;
    u08 buf[NET_MAX_BUF_LEN-5];
}sNet;


typedef struct
{
   u16 sourceaddr;
   u16 destaddr;   
   u08 rssi;
   u08 netid;
   u08 command;
   u08 datalen;      
   u08 databuf[NET_MAX_BUF_LEN-5];   
}sNetMsg;

typedef struct
{
   u08 New;
   u08 rd;
   u08 wr;   
   sNetMsg  NetMsg[1];
}sNetMsgQ;




// *************************************************
// Function Prototype
// *************************************************

u08 net_Check_Q(void);
sNetMsg *net_GetMsg_Q(void);
u08 net_AddMsg_Q(sNetMsg *message);
u08 net_Thread(void);

void net_init(void);
void net_send(u16 destaddr, u08 ack);
void net_clearbuf(void);

void net_printfChar(u08 data);
void net_printf(u08 *string);
void net_printf_rom(const u08 *string);
void net_printfu04(unsigned char data);
void net_printfu08(u08 data);
void net_printfu16(u16 data);
void net_printfu32(u32 data);


#ifdef NETWORK_C
sNet Net;  
sNetMsgQ NetMsgQ;
#else
extern sNet Net;  
#endif





#endif //NETWORK_H

