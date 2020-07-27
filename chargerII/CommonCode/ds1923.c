/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : DS1923.c
   Ver nr.     : 1.0
   Description : Dallas Hydrochron
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-05-12    JvA  
               
 
*************************************************************************************/

#define DS1923_C
#include "ds1923.h"




//***************************************************
// Once Weatherstation in major production
// this has to be included and hence , the functions 
// have to be completed
//***************************************************
#ifdef DS1923_START_STOP_MISSION


//***************************************************
// Method that clears the memory of the Hygrochron
//***************************************************
void DS1923_clearMemory(u08* OW_IDENTITY)
{
	u08 i;
	owSelectDevice(OW_IDENTITY);
	u08 buffer[10];
  buffer [0] = STOP_MISSION_PW_COMMAND;
	
	// password stuff, not really sure
	for(i=1; i<9; i++)
	{
		buffer[i] = 0x00;
	}
  buffer[9] = (u08)0xFF;
	owBlock(OW.Port, FALSE, buffer, 10);
	//wait 2ms for memory clearing to complete
	Wait(13); 	//  4900/750
}

//***************************************************
// Method that starts a new mission to run on the
// DS1923chron
//***************************************************
void DS1923_startMission(u08* OW_IDENTITY)
{
	
}

//***************************************************
// Method that stops the mission currently running on 
// the Hygrochron
//***************************************************
void DS1923_stopMission(u08* OW_IDENTITY)
{
	u08 i;
	owSelectDevice(OW_IDENTITY);
	u08 buffer[10];
  buffer [0] = STOP_MISSION_PW_COMMAND;
	
	// password stuff, not really sure
	for(i=1; i<9; i++)
	{
		buffer[i] = 0x00;
	}
  buffer[9] = (u08)0xFF;
	owBlock(OW.Port, FALSE, buffer, 10);
}
#endif



//***************************************************
// Method that reads one byte at particular memAddress
// of the Hygrochron
//***************************************************
u08 DS1923_readByte(u08* OW_IDENTITY, u16 memAddress)
{

	u08 i;
	u08 msbAddress = (u08)((memAddress >> 8) & 0x0FF);
	u08 lsbAddress = (u08)(memAddress & 0x0FF);
	u08 numBytesToEndOfPage = 32 - (lsbAddress & 0x1F);
	u08 bufferLength = 11 + numBytesToEndOfPage + 2;
	u08 buffer[bufferLength];
	
	owSelectDevice(OW_IDENTITY);
	
	buffer[0] = READ_MEMORY_PSW_COMMAND;
	buffer[1] = lsbAddress;
	buffer[2] = msbAddress;
	for(i=0; i<3; i++)
	{
		buffer[i+3] = (u08)0x00;
	}
	for(i=11; i<bufferLength; i++)
	{
		buffer[i] = (u08)0x0FF;
	}
	owBlock(OW.Port, FALSE, buffer, bufferLength);
	return buffer[11];
}

//***************************************************
// Method that reads the temperature out of the 
// Hygrochron and stores values in lowTempByte
// and highTempByte
//***************************************************
u16 DS1923_readTemperature(u08* OW_IDENTITY)
{	
u08 high,low;

		owTouchReset(OW.Port);
		low = DS1923_readByte(OW_IDENTITY, (u16)0x20C);
		owTouchReset(OW.Port);
		high = DS1923_readByte(OW_IDENTITY, (u16)0x20D);
		owTouchReset(OW.Port);
    
    u16 toreturn = (high<<8) | (low);
    return(toreturn);    
}

//***************************************************
// Method that reads the Humidity out of the 
// Hygrochron and stores values in lowHumidByte
// and highHumidByte
//***************************************************
u16 DS1923_readHumidity(u08* OW_IDENTITY)
{
u08 high,low;

		owTouchReset(OW.Port);
		low = DS1923_readByte(OW_IDENTITY, (u16)0x20E);
		owTouchReset(OW.Port);
		high = DS1923_readByte(OW_IDENTITY, (u16)0x20F);
		owTouchReset(OW.Port);	
    
    u16 toreturn = (high<<8) | (low);
    return(toreturn);
}





