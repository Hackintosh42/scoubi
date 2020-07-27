
#define vxtuart_C
#include "headers.h"

static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";


#define VXT_MAX_STRING_LEN 30

u08 vxt_index;
u08 vxtstring[VXT_MAX_STRING_LEN];


/*
// **************************************************
// Add the characters to the string
// **************************************************
void vxtsend(void)
{
  if(vxt_index > CWG_MAX_MSG_LEN){}
    //debug_rom("ERROR vxt too large",1);
  else
    Comms_SendMsg(Modem.DestAddr,vxtstring,vxt_index);
  
  vxt_index = 0;
}

// **************************************************
// Add the characters to the string
// **************************************************
void vxtclear(void)
{
  vxt_index = 0;
}


// ****************************************
// Printf over the air until  
// the circular transmit buffer.
// ***************************************
void vxtprintf(u08 *string)
{
  while(*(string) != 0x00)
  {
    vxtprintfChar(*string++);    
  }
}


// ****************************************
// Send the byte to the Uart, or write to 
// the circular transmit buffer.
// ***************************************
void vxtprintfChar(u08 data)
{
  vxtstring[vxt_index++] = data;
}

*/
/*

void vxtprintf_rom(const u08 *string)
{
  while(pgm_read_byte(string) != 0x00)
  {
    vxtprintfChar(pgm_read_byte(string));    
    string++;
  }  
}


void vxtprintfu04(unsigned char data)
{
	vxtprintfChar(pgm_read_byte( HexChars+(data&0x0f) ));
}


void vxtprintfu08(u08 data)
{
	// print 8-bit hex value
	vxtprintfu04(data>>4);
	vxtprintfu04(data);
}

void vxtprintfu16(u16 data)
{
	// print 8-bit hex value
	vxtprintfu08(data>>8);
	vxtprintfu08(data&0xFF);
}

void vxtprintfu32(u32 data)
{
	// print 8-bit hex value
	vxtprintfu08(data>>24);
	vxtprintfu08(data>>16);
	vxtprintfu08(data>>8);
	vxtprintfu08(data);  
}

*/

