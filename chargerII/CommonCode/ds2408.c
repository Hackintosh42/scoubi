/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : DS2408.c
   Ver nr.     : 1.0
   Description : Dallas DS620 temperature sensor                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-05-12    JvA  
               
 
*************************************************************************************/

#define DS2408_C
#include "ds2408.h"




//**************************************************
// void DS2438_init(void)
//**************************************************
void DS2408_init(void)
{
  // Scan the bus and get the ds2408's ROMID

  owScanforDevices(OwPortNumber, 1, OW_DS2408_ID, OW_FAMILY_DS2408);
  owPrintOW_ROMID(&OW_DS2408_ID[0][0]); 
  
}

//**************************************************
// Method that reads the EEPROM of the DS2408
// Puts the result in the readBuf array
//**************************************************
void DS2408_readMemory(u08* OW_IDENTITY, u08 startAddr, u08* readBuf, u08 offset, u08 len, u08 startPhysicalAddr)
{
	u08 buff[20];
	u08 i;
	u08 addr = startAddr + startPhysicalAddr;
	for(i=0; i<20; i++) 
	{
		buff[i] = 0xFF;
	}
	if(startAddr + len > 3) // size = 3
		return;
	Weather_selectDevice(OW_IDENTITY);
	buff[0] = READ_MEMORY_COMMAND;

    // address 1
    buff[1] = (u08) (addr & 0xFF);
    // address 2
    buff[2] = (u08) (((addr & 0xFFFF) >> 8) & 0xFF);
	owBlock(OwPortNumber, FALSE, buff, len+3);
	for(i=0; i<len; i++)
	{
		readBuf[i] = buff[i+3];
	}
}

//**************************************************
// Method that writes to the EEPROM of the DS2408
//**************************************************
void DS2408_writeMemory(u08* OW_IDENTITY, u08 startAddr, u08* writeBuf, u08 offset, u08 len, u08 startPhysicalAddress)
{
	u08 i;
//	u08 es_data[3];
//	u08 scratchpad[8];
	if(len == 0)
		return;
	if((startAddr + len) > startPhysicalAddress)
		return;
	if(((startPhysicalAddress+startAddr) == 137) && (len == 1))
    {
		Weather_selectDevice(OW_IDENTITY);

        u08 buffer[5];

        buffer[0] = CHANNEL_ACCESS_WRITE;
        buffer[1] = writeBuf[offset];
        buffer[2] = (u08) ~writeBuf[offset];
		for(i=0; i<2; i++)
		{
			buffer[i+3] = 0xFF;
		}
		owBlock(OwPortNumber, FALSE, buffer, 5);

        if(buffer[3] != (u08) 0x00AA)
        {
            return;
        }
    }
    else if(((startPhysicalAddress+startAddr) > 138) &&
              ((startPhysicalAddress+startAddr+len) < 143))
    {
         Weather_selectDevice(OW_IDENTITY);
		 //rprintf_rom(PSTR("\r\nCORRECT INDEX\r\n"));

        u08 buffer[6];

        buffer[0] = (u08) 0xCC;
        buffer[1] = (u08) ((startAddr+startPhysicalAddress) & 0xFF);
        buffer[2] = (u08) ((((startAddr+startPhysicalAddress) & 0xFFFF) >> 8) & 0xFF);
		
		for(i=0; i<len; i++)
		{
			buffer[i+3] = writeBuf[i+offset];
		}
		owBlock(OwPortNumber, FALSE, buffer, len+3);	
		//rprintf_rom(PSTR("\r\nFINISHED WRITING\r\n"));
    }
    else if(((startPhysicalAddress+startAddr) > 127) &&
              ((startPhysicalAddress+startAddr+len) < 130))
	{}
//    {
//        u08 buffer[8];
//        u08 addr = 128;
//        u08 buff[11];
//		
//		for(i=0; i<11; i++)
//		{
//			buff[i] = 0xFF;
//		}
//
//        Weather_selectDevice(OW_IDENTITY);
//
//        buff[0] = READ_MEMORY_COMMAND;
//
//        // address 1
//        buff[1] = ( byte ) (addr & 0xFF);
//        // address 2
//        buff[2] = ( byte ) (((addr & 0xFFFF) >> 8) & 0xFF);
//
//		owBlock(OwPortNumber, FALSE, buff, 11);
//		
//		for(i=0; i<8; i++)
//		{
//			buffer[i] = buff[i+3];
//		}
//		for(i=0; i<len; i++)
//		{
//			writeBuf[i+offset] = buffer[i];
//		}
//
//         // write the page of data to scratchpad
//        if(!writeScratchpad(startPhysicalAddress+startAddr,buffer,0,8))
//           throw new OneWireIOException("Invalid CRC16 in write");
//
//         if(!readScratchpad(scratchpad,0,8,es_data))
//            throw new OneWireIOException("Read scratchpad was not successful.");
//
//         if((es_data[2] & 0x20) == 0x20)
//         {
//            throw new OneWireIOException("The write scratchpad command was not completed.");
//         }
//         else
//         {
//            for(i=0;i<8;i++)
//               if(scratchpad[i] != buffer[i])
//               {
//                  throw new
//                     OneWireIOException("The read back of the data in the scratch pad did " +
//                                        "not match.");
//               }
//         }
//
//         // Copy data from scratchpad into memory
//         copyScratchpad(es_data);
//      }
//      else
//         throw new OneWireIOException("Trying to write read-only memory.");
}

//**************************************************
// Method that reads the DS2408 and obtains the REGISTER
// array to be used by the setResetMode() method
//**************************************************
void DS2408_readRegister(u08* OW_IDENTITY, u08* REGISTER)
{
	/*for(i=0; i<3; i++)
	{
		STATE[i] = 0xFF;
	}*/
	DS2408_readMemory(OW_IDENTITY, 0, REGISTER, 0, 3, 0x8B); // startAddr is 139 as per dataSheet
}

//**************************************************
// Method that sets the reset mode of the DS2408
// The RSZT gets set or unset
//**************************************************
void DS2408_setResetMode(u08* OW_IDENTITY, u08 set)
{
	u08 REGISTER[3];
	DS2408_readRegister(OW_IDENTITY, REGISTER);
	/*u08 i;
	rprintf_rom(PSTR("\r\nREGISTER:\r\n"));
	for(i=0; i<3; i++)
	{
		rprintf_rom(PSTR("["));
		rprintfu08(REGISTER[i]);
		rprintf_rom(PSTR("]"));
	}
	rprintf_rom(PSTR("\r\n"));*/
	if((set == TRUE) && ((REGISTER[2] & 0x04) == 0x04))
    {
        REGISTER[2] = (u08) (REGISTER[2] & (u08) 0xFB);
    }
    else if((set == FALSE) && ((REGISTER[2] & (u08) 0x04) == (u08) 0x00))
    {
        REGISTER[2] = (u08) (REGISTER[2] | (u08) 0x04);
    }
	
	
	// this is what the register block should look like, so its a bit of a hack since readRegister doesnt wanna work
	//REGISTER[0] = 0x00;
	//REGISTER[1] = 0x00;
	//REGISTER[2] = 0x0C; 
	DS2408_writeMemory(OW_IDENTITY, 0, REGISTER, 0, 3, 0x8B); // startAddr is 139 as per dataSheet
}

//**************************************************
// Method that reads the DS2408 and obtains the STATE
// array to be used by the getlatchState() method
//**************************************************
void DS2408_readDevice(u08* OW_IDENTITY, u08* STATE)
{
	u08 i;
	for(i=0; i<3; i++)
	{
		STATE[i] = 0xFF;
	}	
	DS2408_readMemory(OW_IDENTITY, 0, STATE, 0, 3, 136); // startAddr is 136 as per dataSheet
}

//**************************************************
// Reads the state of a channel, either high=1 or low=0
//**************************************************
u08 DS2408_getLevel(u08* OW_IDENTITY, u08 channel)
{
	u08 level = (u08)(0x01 << channel);
	u08 STATE[3];
	DS2408_readDevice(OW_IDENTITY, STATE);
	/*
	u08 i;
	rprintf_rom(PSTR("\r\nSTATE:\r\n"));
	for(i=0; i<3; i++)
	{
		rprintf_rom(PSTR("["));
		rprintfu08(STATE[i]);
		rprintf_rom(PSTR("]"));
	}*/
    return ((STATE[0] & level) == level);
}

//**************************************************
// Assigns the byte of latch states (windDirection)
//**************************************************
u08 DS2408_getAllLatchLevel(u08* OW_IDENTITY)
{	
	u08 theByte = 0x00;
	u08 i;
	for(i=0; i<8; i++)
	{
		theByte = theByte << 1;
		if(DS2408_getLevel(OW_IDENTITY, i))
		{
			theByte = theByte | (0x01); 
		}
	}
	return(theByte);
}	
