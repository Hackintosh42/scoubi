

/*********************************** MODULE INFO ****************************
	
   File name    : Link.c
   Ver nr.      : 1.0
   Description  : Link Layer Protocol functions
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt
 
   Change log   : 2005-04-17  Initial 
 
****************************************************************************/

#define LINK_C
#include "link.h"




//**************************************************
// Check if there is an link message waiting to be 
// processed
//**************************************************  
sLinkMsg *Link_GetNextMsgPtr(void)
{
   return(&Link.LinkMsg);
}



//**************************************************
// Thread that must be called as often as possible
// to handle Messages up from the link layer
//**************************************************  
u08 Link_Thread(void)
{
u08 i;
sModemRxMsg *ModemMsg_ptr;


    //*************************************************************************
    // Check for link layer messages sent over RS232
    //*************************************************************************
		if(((volatile u08)RxSerial.EndDelim_flag) && (Link.MessageFormat == LINK_CLIMATAG_FORMAT))
		{
/*    
      rprintfChar('!');
      rprintf_rom(PSTR("EndDelim_flag = "));					          
      rprintfu08(RxSerial.EndDelim_flag);
      rprintf_rom(PSTR("\r\nRxSerial.wr_index = "));					          
      rprintfu08(RxSerial.wr_index);
      rprintf_rom(PSTR("\r\n"));					          
*/      

      
			RxSerial.EndDelim_flag--;   
      u08 delimpos = RxSerial.wr_index;
      
      // Now find the start deliminator and using the lenght hopefully arrive at the same 
      // end delim position
      u08 foundmessage = 0x00;
      for(i=0;i<delimpos;i++)
      {
        if(!foundmessage)
        {
          if(RxSerial.charbuffer[i] == '[')
          {
            //rprintf("[ ->");rprintfu08(i);rprintf("\r\n");
            u08 len = RxSerial.charbuffer[i+1];
            //rprintf("len = ");rprintfu08(len);rprintf("\r\n");            
            u08 tmp = i+len+2;
            //rprintf("delim = ");rprintfChar(RxSerial.charbuffer[tmp]);rprintf("\r\n");       
            if((RxSerial.charbuffer[tmp] == ']'))
            {
              //rprintf_rom(PSTR("Found message\r\n"));
              foundmessage = 1;
              RxSerial.StartDelim_flag = RxSerial.EndDelim_flag = 0;
              RxSerial.wr_index = 0;
              
              
              //if(Q_AddMsg(&RxSerial.charbuffer[i+1],len+1))
                Link.LinkMsg.len = tmp+1;
                Link.LinkMsg.rssi = 0x00;
                Link.LinkMsg.phy = LINK_PHY_RS232;
                for(i= 0; i<Link.LinkMsg.len;i++)
                  Link.LinkMsg.buf[i] = RxSerial.charbuffer[i+1]; //ModemMsg_ptr->buf[i];              
             
              u08 nn;
              // Clear the buffer of any '[''s that might be hanging around              
              for(nn= 0; nn<RX_SERIAL_BUFFER_SIZE;nn++)              
                RxSerial.charbuffer[nn] = 0x00;
              RxSerial.wr_index = 0;                    
            }
          }
        }
      }
              
     if(!foundmessage)
     {
         if(RxSerial.EndDelim_flag == 0)         
          rprintf_rom(PSTR("MFE!\r\n"));					      
         return(0);     
     }
     else
         return(1);
     
     			
    }
    //**************************************************
    // Check for link layer messages sent via the Modem
    //**************************************************    
    else if(Modem_CheckRxQ())
    {
      ModemMsg_ptr = Modem_GetNextRxMsg(); 
      if(ModemMsg_ptr != NULL)
      {   
        //rprintf_rom(PSTR("Link Message from Modem\r\n"));
        
        // Copy the message from the Physical layer to the Link Layer
        // Temporary buffer
        Link.LinkMsg.len = ModemMsg_ptr->len;
        Link.LinkMsg.rssi = ModemMsg_ptr->rssi;
        Link.LinkMsg.phy = LINK_PHY_RF;
        for(i= 0; i<ModemMsg_ptr->len;i++)
          Link.LinkMsg.buf[i] = ModemMsg_ptr->buf[i];
        
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
// Add the characters to the string
//**************************************************
void link_send(u08 *srcptr,u08 len)
{
u08 i;

  //**************************************************
  // Send over the RS232 link
  //**************************************************
  if((Link.phylayer == LINK_PHY_RS232) || (Link.phylayer == LINK_PHY_RF_AND_RS232))
  {
      if(Link.MessageFormat == LINK_CLIMATAG_FORMAT)
        rprintfChar('[');            
      for(i=0;i<len;i++)
        rprintfChar(srcptr[i]);          
      if(Link.MessageFormat == LINK_CLIMATAG_FORMAT)      
        rprintfChar(']');                  
  }  
    
  //**************************************************
  // Send over the RF Air link
  //**************************************************
  if((Link.phylayer == LINK_PHY_RF) || (Link.phylayer == LINK_PHY_RF_AND_RS232))
  {
    if(len > MODEM_MAX_TX_MSG_LEN)
      rprintf_rom(PSTR("2large"));
    else
    {
      MODEM_RESTART;
      //#define DEBUG_LINK
      #ifdef DEBUG_LINK
        rprintf_rom(PSTR("Link = "));
        u08 i;
        for(i=0;i<len;i++)
          rprintfu08(srcptr[i]);
        rprintf("\r\n");
        for(i=0;i<len;i++)
          rprintfChar(srcptr[i]);
        rprintf("\r\n");   
      #endif
   
      Modem_Send(srcptr,len);  
    }
  }
  
  //rprintf_rom(PSTR("Link_send(complete)\r\n"));
}





//**************************************************
// Set the physical layer for the link layer to use
//**************************************************
void link_setphy(u08 phy)
{
  Link.phylayer = phy;
}



