/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : DS1920.c
   Ver nr.     : 1.0
   Description : Dallas Temperature Device
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-20    JvA  
               
 
*************************************************************************************/

#define DS1920_C
#include "ds1920.h"


u08 OW_DS1920_STATE[8];



//****************************************************
// Method that corresponds to the java api of readDevice
// and puts the result in the OW_DS1920_STATE array
//***************************************************
void DS1920_readDevice(u08* OW_IDENITY)
{
	owSelectDevice(OW_IDENITY);
	u08 i;
	u08 buffer[10];
	buffer[0] = OW_RD_SCR_CMD;
	for(i = 1; i < 10; i++)
	{
		buffer[i] = 0x0FF;
	}
	owBlock(OW.Port, FALSE, buffer,10);
	//rprintf_rom(PSTR("\r\nSHOWING BUS\r\n"));
	/*for(i=0; i<10; i++)
	{
		rprintfu08(buffer[i]);
		rprintf_rom(PSTR("\r\n"));
	}*/
	// NB --> Still gotta do crc check
	for(i = 0; i < 8; i++)
	{
		OW_DS1920_STATE[i] = buffer[i+1];
	}
	OW_DS1920_STATE[4] = 0x01;
	
	/*
	for(i=0;i<8;i++)
	{
		rprintf_rom(PSTR("\r\nByte: "));
		rprintfu08(i);
		rprintf_rom(PSTR(" "));
		rprintfu08(OW_STATE[i]);
		rprintf_rom(PSTR("\r\n"));
	}
	*/
	//rprintf_rom(PSTR("\r\nREAD TEMPERATURE\r\n"));
}

/*-----------------------------------------------------------------------------
 * Send a 1 wire command to a device, or all if no ROM ID provided
 */
void DS1920_owSendCmd(uint8_t *ROM, uint8_t cmd) 
{
    uint8_t	i;
    owTouchReset(OW.Port);
    if (ROM == NULL)
			owWriteByte(OW.Port, OW_SKIP_ROM_CMD);
    else 
		{
			owWriteByte(OW.Port, OW_MATCH_ROM_CMD);
			for (i = 0; i < 8; i++)
	    owWriteByte(OW.Port, ROM[i]);
    }
    owWriteByte(OW.Port, cmd);
}

//**************************************************
// Method that corresponds to the java api of readScratch
//readDevice should be called before this method
//**************************************************
void DS1920_readScratch(u08* OW_IDENITY)
{
	owSelectDevice(OW_IDENITY);
	u08 buffer[10]; // optimize this
	u08 i;
	buffer[0] = OW_RD_SCR_CMD;
	for(i=1; i<10; i++)
	{
		buffer[i] = 0x0FF;
	}
	owBlock(OW.Port,FALSE, &buffer[0],10);
	// still add crc checking
	
	for(i=0; i<8; i++)
	{
		//rprintf_rom(PSTR("\r\n Before:"));
		//rprintf_rom(PSTR("\r\n"));
		//rprintfu08(i);
		//rprintf_rom(PSTR(": "));
		//rprintfu08(OW_STATE[i]);
		//rprintf_rom(PSTR("\r\n"));
			OW_DS1920_STATE[i] = buffer[i+1];
		//rprintf_rom(PSTR("\r\n After:"));
		//rprintfu08(i);
		//rprintf_rom(PSTR(": "));
		//rprintfu08(OW_STATE[i]);
		//rprintf_rom(PSTR("\r\n"));
	}
}

//**************************************************
// Method that corresponds to the java api of doTemperatureConvert
//**************************************************
void DS1920_doTemperatureConvert(u08 * OW_IDENITY)
{
	//u08 tempPORTC;
	//u08 tempDDRC;
	u16 i;
	
	//rprintf_rom(PSTR("\r\nCONVERTING TEMPERATURE \r\n"));
	DS1920_readDevice(OW_IDENITY); // updates OW_STATE
      
	// DONNO how2do, ask Janus
		// Setup Power Delivery
		//adapter.setPowerDuration(adapter.DELIVERY_INFINITE);
		//adapter.startPowerDelivery(adapter.CONDITION_AFTER_BYTE);	
	
	DS1920_owSendCmd(OW_IDENITY, OW_CONVERTT_CMD);
	
	// store previous port direction and output
	//tempPORTC = PORTC; 
	//tempDDRC = DDRC;
	
	// Port pin output and pull pin high
	owLevel(OW.Port,MODE_STRONG5);
	
	
	//PORTC = PORTC | 0x01;
	//DDRC = DDRC | 0x01;
	
	//rprintf_rom(PSTR("\r\nWAITING 750ms \r\n"));
	// delay for 750ms
	//DEBUG1_1;
	//Wait(4900); // this should work on accurate timers
	
	// delay for 750ms
  // IF the modem is not running then this should be fine
  
  // Much more accurate 
  //DEBUG1_SETOUTPUT;
  //DEBUG1_1;
    
  if(Modem.Enable == 0)
  {
    for(i=0;i<1500;i++) // should be 1562 loops
      owDelay480us(); // 480us
  }
  else
  {
    // Much more accurate 
    Modem_Timer_Wait_ms(500);       
  }
  //DEBUG1_0;
  
  
	//DEBUG1_0;
	
	// restore port paramaters
	owLevel(OW.Port,MODE_NORMAL);
	
	//PORTC = tempPORTC;
	///DDRC = tempDDRC;
	
	// DONNO how2do, ask Janus	
		// Turn power back to normal.
         //adapter.setPowerNormal();
	//rprintf_rom(PSTR("\r\n READING BYTES \r\n"));	 
	while(owReadByte(OW.Port) != 0x0FF) // make sure pin is high
	{ /*do nothing*/ } 
	
	u08 mode = OW_DS1920_STATE[4];
	owSelectDevice(OW_IDENITY);
	//rprintf_rom(PSTR("\r\nREADING SCRATCHPAD \r\n"));
	DS1920_readScratch(OW_IDENITY);
	//rprintf_rom(PSTR("\r\nREAD SCRATCHPAD \r\n"));
	OW_DS1920_STATE[4] = mode;
}   

//**************************************************
// Reads the temperature out of the button and stores
// it in the temperature variable
//**************************************************
u16 DS1920_readTemperature(u08 * OW_IDENITY)
{
	DS1920_doTemperatureConvert(OW_IDENITY);
	DS1920_readDevice(OW_IDENITY);
	
	u16 temp;
	u08 tempera;
	if (((OW_DS1920_STATE [1] & 0x0ff) != 0x00) && ((OW_DS1920_STATE[1] & 0x0ff) != 0x0FF))
	{
         tempera = 0;
         temp = 0;
	}
	else
	{
		temp = (u16)((OW_DS1920_STATE[0] & 0x0FF) | (OW_DS1920_STATE[1] << 8));
  } 
  
  return(temp);
    
    // COR What to do ??????
/*
    
		if(OW_DS1920_STATE[4] == 0x01)
		{
		
			temp = (short) (temp >> 1);
			double tmp = (double) temp;
			double cr = OW_DS1920_STATE[6] & 0x0FF;
			double cpc = OW_DS1920_STATE[7] & 0x0DD;
			tmp = tmp - (double) 0.25 + (cpc - cr) / cpc;
			tempera = tmp;
		}
		else
		{
			tempera = temp / 2.0;
		}
	}
*/  
  
	//u08 t = (u08)temperature;
	//rprintf_rom(PSTR("\r\nTemperature: "));
	//print_decimal_u08(t);
	//rprintf_rom(PSTR("\r\n"));
}

