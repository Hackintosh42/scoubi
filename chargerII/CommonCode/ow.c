
#define OW_C
#include "ow.h"



//***************************************************
// Method that selects the device who's ROM_ID
// corresponds to that of OW_IDENTITY
//***************************************************
void owSelectDevice(u08* OW_IDENTITY)
{
	owTouchReset(OW.Port);
	u08 send_packet[9];
	u08 i;
	send_packet[0] = 0x55;
	for(i = 1;i < 9;i++)
	{
		send_packet[i] = OW_IDENTITY[i-1];
	}
	owDelay();
	owDelay();
	owBlock(OW.Port, FALSE, send_packet,9); // check if this was successful
}




//**************************************************
// Find a specific device on the one wire bus
//**************************************************
void owScanforDevices(u08 portnum, u08 numdevices, u08 Rom_array[][8], u08 targetFamily)
{
u08 nn;

    for(nn =0; nn<numdevices;nn++)
    {
      owTouchReset(portnum);              
      owDelay();           	
    
      memset(&Rom_array[nn][0], 0, 8);    
      // target a specific family
      Rom_array[nn][0] = targetFamily;
      LastDiscrepancy[portnum] = 0x40;
      LastDevice[portnum] = FALSE;
      LastFamilyDiscrepancy[portnum] = 0;         
      owSerialNum(portnum, &Rom_array[nn][0], FALSE); // Store the serial       
              	
      if(nn == 0)
      {
        owNext(portnum, FALSE, FALSE);        
        owSerialNum(portnum, &Rom_array[nn][0], TRUE); // copy out the ROM number that was discovered                
      }
      else
      {      
        owNext(portnum, FALSE, FALSE);        
        owSerialNum(portnum, &Rom_array[nn][0], TRUE);         
      } 
    }
}


//**************************************************
// prints out the OW rom code
//**************************************************
void owPrintOW_ROMID(u08* tmprom)    
{
u08 i;

    for(i=0; i<8; i++)
    {
        rprintfu08(tmprom[i]);        
        rprintf_rom(PSTR(":"));    
    }
    rprintf_rom(PSTR("\r\n"));      
}





//**************************************************
//  OWdelay 
//**************************************************
void owDelay(void) 
{
    DELAY_I;
}


//**************************************************
//  OWdelay 
//**************************************************
void owDelay480us(void) 
{
    DELAY_H;
}


//**************************************************
// owRaiseError
//**************************************************
/*
void owRaiseError(u08 errorno)
{
  rprintf_rom(PSTR("OW ERROR "));
  rprintfu08(errorno);
}
*/

//***************************************************************************
// Wrapper functions around the lowlevel 1-wire link level commands
// for accessing botht the Microprocessor 1-wire port and ds2482 ports
//***************************************************************************

#ifdef OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE
#ifdef OW_AVR_DS2482_IMPLEMENTATION_ENABLE

  u08 owAcquire(u08 portnum, char *port_zstr)
  {
    if(portnum == OW_PORT2)
      return(owAcquire_avr(portnum, port_zstr));
    else
      return(owAcquire_ds2482(portnum, port_zstr));
  }
  
  void owRelease(u08 portnum)
  {
    if(portnum == OW_PORT2)
      owRelease_avr(portnum);
  
  }
  
  
  
  u08 owTouchReset(u08 portnum)
  {
    if(portnum == OW_PORT2)
    {
      return(owTouchReset_avr(portnum));
    }
    else
      return(owTouchReset_ds2482(portnum));
  }
  
  u08 owTouchBit(u08 portnum, u08 sendbit)
  {
    if(portnum == OW_PORT2)
      return(owTouchBit_avr(portnum, sendbit));
    else
      return(owTouchBit_ds2482(portnum, sendbit));
  }
  
  
  u08 owTouchByte(u08 portnum, u08 sendbyte)
  {
    if(portnum == OW_PORT2)
      return(owTouchByte_avr(portnum, sendbyte));
    else
      return(owTouchByte_ds2482(portnum, sendbyte));
  }
  
  u08 owWriteByte(u08 portnum, u08 sendbyte)
  {
    if(portnum == OW_PORT2)
      return(owWriteByte_avr(portnum, sendbyte));
    else
      return(owWriteByte_ds2482(portnum, sendbyte));
  }
  
  u08 owReadByte(u08 portnum)
  {
    if(portnum == OW_PORT2)
      return(owReadByte_avr(portnum));
    else
      return(owReadByte_ds2482(portnum));
  }
  
  
  u08 owLevel(u08 portnum, u08 new_level)
  {
    if(portnum == OW_PORT2)
      return(owLevel_avr(portnum, new_level));
    else
      return(owLevel_ds2482(portnum, new_level));
  }
  #endif
#endif
  
  #ifdef OW_AVR_DS2482_IMPLEMENTATION_ENABLE
  #ifndef OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE
  
    u08 owAcquire(u08 portnum, char *port_zstr)
    {
        return(owAcquire_ds2482(portnum, port_zstr));
    }
    
    void owRelease(u08 portnum)
    {
      // HACK // There should be a better way to do this.
      // Actually in this case the R16 resistor shouldn't even be there
      // make the pin an input with no pull up
      OWIREDDR = OWIREDDR & ~(1<<OWIREOUTPIN);
      OWIREOUTPORT = OWIREOUTPORT & ~(1<<OWIREOUTPIN);  
    
    }
      
    u08 owTouchReset(u08 portnum)
    {
        return(owTouchReset_ds2482(portnum));
    }
    
    u08 owTouchBit(u08 portnum, u08 sendbit)
    {
        return(owTouchBit_ds2482(portnum, sendbit));
    }
    
    
    u08 owTouchByte(u08 portnum, u08 sendbyte)
    {
        return(owTouchByte_ds2482(portnum, sendbyte));
    }
    
    u08 owWriteByte(u08 portnum, u08 sendbyte)
    {
        return(owWriteByte_ds2482(portnum, sendbyte));
    }
    
    u08 owReadByte(u08 portnum)
    {
        return(owReadByte_ds2482(portnum));
    }
    
    
    u08 owLevel(u08 portnum, u08 new_level)
    {
        return(owLevel_ds2482(portnum, new_level));
    }
    
  #endif
  #endif

  #ifdef OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE
  #ifndef OW_AVR_DS2482_IMPLEMENTATION_ENABLE
    u08 owAcquire(u08 portnum, char *port_zstr)
    {
        return(owAcquire_avr(portnum, port_zstr));
    }
    
    void owRelease(u08 portnum)
    {
        return(owRelease_avr(portnum));      
    }
      
    u08 owTouchReset(u08 portnum)
    {
        return(owTouchReset_avr(portnum));
    }
    
    u08 owTouchBit(u08 portnum, u08 sendbit)
    {
        return(owTouchBit_avr(portnum, sendbit));
    }
    
    
    u08 owTouchByte(u08 portnum, u08 sendbyte)
    {
        return(owTouchByte_avr(portnum, sendbyte));
    }
    
    u08 owWriteByte(u08 portnum, u08 sendbyte)
    {
        return(owWriteByte_avr(portnum, sendbyte));
    }
    
    u08 owReadByte(u08 portnum)
    {
        return(owReadByte_avr(portnum));
    }
    
    
    u08 owLevel(u08 portnum, u08 new_level)
    {
        return(owLevel_avr(portnum, new_level));
    }
    
  #endif
  #endif




  
  













