/*
//**************************************************
// Add the characters to the string
//**************************************************
void link_send(void)
{
  Link.buf[Link.index] = 0x00; // Add the null just to terminate
  
  //**************************************************
  // Send over the RS232 link
  //**************************************************
  if((Link.phylayer == LINK_PHY_RS232) || (Link.phylayer == LINK_PHY_RF_AND_RS232))
  {
      if(Modem.ClimatagMessageFormat)
        rprintfChar('[');            
      rprintf(Link.buf);          
      if(Modem.ClimatagMessageFormat)      
        rprintfChar(']');                  
  }  
    
  //**************************************************
  // Send over the RF Air link
  //**************************************************
  if((Link.phylayer == LINK_PHY_RF) || (Link.phylayer == LINK_PHY_RF_AND_RS232))
  {
    if(Link.index > MODEM_MAX_TX_MSG_LEN)
      rprintf_rom(PSTR("ERROR link string too large"));
    else
    {
      MODEM_ENABLE;
      Modem_Send(Link.buf,Link.index);  
      //#define DEBUG_LINK_BUF
      #ifdef DEBUG_LINK_BUF
        rprintf_rom(PSTR("Link.buf = "));
        u08 i;
        for(i=0;i<Link.index;i++)
          rprintfu08(Link.buf[i]);
        rprintf("\r\n");
        for(i=0;i<Link.index;i++)
          rprintfChar(Link.buf[i]);
        rprintf("\r\n");
        
      #endif
    }
  }


  Link.index = 0;
}

//**************************************************
// Add the characters to the string
//**************************************************
void link_clear(void)
{
  Link.index = 0;
}



//****************************************
// Send the byte to the Uart, or write to 
// the circular transmit buffer.
// This should only be called by the higher
// network layer, as it adds the source 
// destination and length 
//***************************************
void link_printfChar(u08 data)
{
  Link.buf[Link.index++] = data;
  if(Link.index > LINK_MAX_BUF_LEN)
    rprintf_rom(PSTR("Link string too large\r\n"));
}


void link_printf(u08 *string)
{
  while(*(string) != 0x00)
  {
    link_printfChar(*string++);    
  }
}


void link_printf_rom(const u08 *string)
{
  while(pgm_read_byte(string) != 0x00)
  {
    link_printfChar(pgm_read_byte(string));    
    string++;
  }  
}


void link_printfu04(unsigned char data)
{
	link_printfChar(pgm_read_byte( HexChars+(data&0x0f) ));
}


void link_printfu08(u08 data)
{
	// print 8-bit hex value
	link_printfu04(data>>4);
	link_printfu04(data);
}

void link_printfu16(u16 data)
{
	// print 8-bit hex value
	link_printfu08(data>>8);
	link_printfu08(data&0xFF);
}

void link_printfu32(u32 data)
{
	// print 8-bit hex value
	link_printfu08(data>>24);
	link_printfu08(data>>16);
	link_printfu08(data>>8);
	link_printfu08(data);  
}
*/


