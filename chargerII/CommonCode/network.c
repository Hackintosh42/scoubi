

/*********************************** MODULE INFO ****************************
	
   File name    : Network.c
   Ver nr.      : 1.0
   Description  : Network Layer Protocol functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt
 
   Change log   : 2005-06-29  Initial 
 
****************************************************************************/

#define NETWORK_C
#include "network.h"

// TODO :there should only be one global table for all 
// lookup routines to use
static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";
//extern char __attribute__ ((progmem)) HexChars[];

//**************************************************
// initialize the network layer
//**************************************************
void net_init(void)
{
  net_clearbuf();
  uart_ClearRXbuffers();
}


//**************************************************
// Clear the buffer
//**************************************************
void net_clearbuf(void)
{
  // Leave the first 6 bytes free for the length, source and destination addresses netid
  Net.index = 6;
}


// ********************************************************
// ********************************************************



//*************************************************
// Check if there is a message in the Q waiting
// to be processed
//*************************************************
u08 net_Check_Q(void)
{
  return(NetMsgQ.New);
}

//*************************************************
// Get the Message from the Q
//*************************************************
sNetMsg *net_GetMsg_Q(void)
{
	// If there is no msg return Null
	if(!NetMsgQ.New)
		return NULL;
	else
	{			
    NetMsgQ.New = 0;
		return(&NetMsgQ.NetMsg[0]);		// return the pointer to the message
	}
}

//*************************************************
// Add a new network message to the Q
//*************************************************
u08 net_AddMsg_Q(sNetMsg *message) 
{
u08 i; 
	
  // ******** NOTE **************
  // The Q is only 1 deep for now.
  // Make it a proper Q
  // ****************************
  
	if (message->datalen > MSG_BUFFER_LENGTH)
  {
		//rprintf_rom(PSTR("!AddMsg_Q"));
    return(1);
	}
	else
	{
    NetMsgQ.New = 1;      
    // Copy the message out now
    NetMsgQ.NetMsg[0].sourceaddr = message->sourceaddr; 
    NetMsgQ.NetMsg[0].destaddr   = message->destaddr; 
    NetMsgQ.NetMsg[0].netid      = message->netid;         
    NetMsgQ.NetMsg[0].rssi       = message->rssi;    
    NetMsgQ.NetMsg[0].command    = message->command;        
    NetMsgQ.NetMsg[0].datalen    = message->datalen;    
		for(i = 0;i< message->datalen; i++)
    {
				NetMsgQ.NetMsg[0].databuf[i] = message->databuf[i];
    }
    return(0);
	}
}



//**************************************************
// Thread that must be called as often as possible
// to handle Messages up from the link layer
//************************************************** 
 
u08 net_Thread(void)
{
sNetMsg tmpNetMsg;
sLinkMsg *LinkMsg_ptr;
   
    if(Link_Thread())
    {
      LinkMsg_ptr = Link_GetNextMsgPtr();
      //rprintf_rom(PSTR("WirelessMsg_ptr ->")); rprintf(&WirelessMsg_ptr->buf[0]);
      if(LinkMsg_ptr != NULL)
      {               
        //u08 len =  LinkMsg_ptr->buf[0];                
      
        // Add the message received over the air to the message Q
        // Parse out the source and destination address        
        tmpNetMsg.sourceaddr = (LinkMsg_ptr->buf[1] << 8) | LinkMsg_ptr->buf[2];
        tmpNetMsg.destaddr   = (LinkMsg_ptr->buf[3] << 8) | LinkMsg_ptr->buf[4];        
        tmpNetMsg.rssi       =  LinkMsg_ptr->rssi;
        tmpNetMsg.netid      =  LinkMsg_ptr->buf[5];
        tmpNetMsg.command    =  LinkMsg_ptr->buf[6]; 
        tmpNetMsg.datalen    =  LinkMsg_ptr->buf[0] -6;                
        u08 i;
        for(i= 0; i<tmpNetMsg.datalen;i++)
          tmpNetMsg.databuf[i] = LinkMsg_ptr->buf[7+i];
                                  
        net_AddMsg_Q(&tmpNetMsg);
        
        // ************ Net debug ***************
        #define NET_DEBUG_LINKMSG
        #ifdef  NET_DEBUG_LINKMSG
          rprintf_rom(PSTR("\r\n{"));              
          for(i = 0;i<LinkMsg_ptr->len;i++)
          {
            rprintfu08(LinkMsg_ptr->buf[i]);
            rprintfChar(' ');
          }
          rprintf("}\r\n");          
        #endif            
        
        return(1);/// Indicate that a network message is ready for processing
      }
      else
      {
        //rprintf_rom(PSTR("Modemptr NULL\r\n"));
        return(0);
      }
    }    
    return(0);
}




//**************************************************
// Add the Source and desination pack it all up and
// send it baby
//**************************************************
void net_send(u16 destaddr, u08 ack)
{  

  // Fill in the length, source and destination address
  Net.buf[0] = Net.index-1;
  // Add source address
  Net.buf[1] = Net.LocalAddr>>8;
  Net.buf[2] = Net.LocalAddr;

  // Add destination address  
  Net.buf[3] = (destaddr>>8) ;
  Net.buf[4] = destaddr;

  Net.buf[5] = Net.NetID;

  Net.buf[Net.index] = 0x00; // Add the null just to terminate
  
  
      //#define DEBUG_NET
      #ifdef DEBUG_NET
        rprintf_rom(PSTR("Net index = "));
        rprintfu08(Net.index);
        rprintf_rom(PSTR("\r\n"));        
        u08 i;
        for(i=0;i<Net.index;i++)
          rprintfu08(Net.buf[i]);
        rprintf("\r\n");
        for(i=0;i<Net.index;i++)
          rprintfChar(Net.buf[i]);
        rprintf("\r\n");   
      #endif  
  
  
  // **************************************************
  // ****** Send the message over the link layer ******
  // **************************************************
  link_send(&Net.buf[0],Net.index);
  
  //#define NETWORK_ACK_ENABLED
  #ifdef NETWORK_ACK_ENABLED
  // Wait for the ack back
  if(ack)
  {
    u08 ackcomplete = 0;
    u08 exitloop = 0;
    u08 ack_cnt = 0;
    while((!ackcomplete) && (!exitloop))
    {
      // ***************************************************************
      // Wait for a predetermined time then Check if an Ack was recieved
      // ***************************************************************    
      #define NET_ACK_TIMEOUT 250
      if((volatile u08)Modem.AckTimer > NET_ACK_TIMEOUT)
      {
        if(++ack_cnt <3)
        {
          #ifdef DEBUG_NET
            rprintf_rom(PSTR("Net_resend\r\n"));
          #endif
          Modem.AckTimer = 0x0000;
          // Up the Transmit power do we have enough power captain?
          Modem_TxLevel_inc();
          link_send(&Net.buf[0],Net.index);
        }
        else
          exitloop = 1;
      }
      
      // *******************************************************
      // Check if an Ack has been received from the destination
      // address last packet was sent to
      // *******************************************************
      if(Modem.AckReceived)
      {
          Modem.AckReceived = 0;
          ackcomplete = 1;
          #ifdef DEBUG_NET
            rprintf_rom(PSTR("Ack received"));
          #endif
      }
    }
    
    if(exitloop)
    {
      #ifdef DEBUG_NET
        rprintf_rom(PSTR("No ack received\r\n"));
      #endif
    }
    
    // Reset the modem level back to the default base amount
    Modem_Reset_TxLevel();
  }
  #endif
    
  // **********************************************
  // *******        Clear the buffer      *********
  // **********************************************
  Net.buf[0] = Net.buf[1] = Net.buf[2] = Net.buf[3] = Net.buf[4] = Net.buf[5] = 0x00;  
  net_clearbuf();
}


//****************************************
// Add the byte to the buffer to be sent
//****************************************
void net_printfChar(u08 data)
{
  Net.buf[Net.index++] = data;
  if(Net.index > NET_MAX_BUF_LEN)
    rprintf_rom(PSTR("2Large\r\n"));
}


//**************************************************
// Append the string to the message to be sent
//**************************************************
void net_printf(u08 *string)
{
  while(*(string) != 0x00)
  {
    net_printfChar(*string++);    
  }
}

//**************************************************
// 
//**************************************************
void net_printf_rom(const u08 *string)
{
  while(pgm_read_byte(string) != 0x00)
  {
    net_printfChar(pgm_read_byte(string));    
    string++;
  }  
}

  //**************************************************
  // 
  //**************************************************
  void net_printfu04(unsigned char data)
  {
    net_printfChar(pgm_read_byte( &HexChars[0]+(data&0x0f) ));
  }
  
  
  //**************************************************
  // 
  //**************************************************
  void net_printfu08(u08 data)
  {
    // print 8-bit hex value
    net_printfu04(data>>4);
    net_printfu04(data);
  }

//#define NETWORK_PRINTFU08
#ifdef NETWORK_PRINTFU08  
  //**************************************************
  // 
  //**************************************************
  void net_printfu16(u16 data)
  {
    // print 8-bit hex value
    net_printfu08(data>>8);
    net_printfu08(data&0xFF);
  }
  
  //**************************************************
  // 
  //**************************************************
  void net_printfu32(u32 data)
  {
    // print 8-bit hex value
    net_printfu08(data>>24);
    net_printfu08(data>>16);
    net_printfu08(data>>8);
    net_printfu08(data);  
  }
#endif 


