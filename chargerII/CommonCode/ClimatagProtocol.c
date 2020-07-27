/*********************************** MODULE INFO ****************************
	
   File name    : ClimatagProtocol.c
   Ver nr.      : 1.0
   Description  : Contains the commands for the communication between the ClimaTags and the ClimaAPI
   Compiler     : AVR GCC
   Company      : Fairbridge Technologies
   Author       : Jaco Raubenheimer
 
   Change log   : 2005-06-29  Initial 
 
****************************************************************************/

#define CLIMAPROTOCOL_C
#include "ClimatagProtocol.h"

// *****************************************
// Initialization
// *****************************************
void initClimatagProtocol(void)
{
	CLIMAPROTOCOL.NO_OF_DEVICES = 0x00;
	CLIMAPROTOCOL.DEVICE_ID = 0x0100;
	CLIMAPROTOCOL.NETWORK_ID = NO_DEFINED_NETWORK;
	CLIMAPROTOCOL.MODEMLINK = 0x00;
}

// *****************************************
// Method that sets the Net address to 
// that of the DEVICE_ID
// *****************************************
void setupNetAddress(void)
{
	//Net.LocalAddr[0] = (u08)(CLIMAPROTOCOL.DEVICE_ID >> 8);
  //Net.LocalAddr[1] = (u08)(CLIMAPROTOCOL.DEVICE_ID & 0xFF);
	Net.LocalAddr = (u16)(CLIMAPROTOCOL.DEVICE_ID);
	Net.NetID = CLIMAPROTOCOL.NETWORK_ID;
}

// *****************************************
// Set the destination address
// *****************************************
void setDestinationAddress(u16 addr)
{
	CLIMAPROTOCOL.DESTINATION_ADDRESS = addr;
}

// *****************************************
// Method that sends out a success command
// in order to indicate that a certain 
// command was received successfully
// *****************************************
void sendCommandwithNoData(u08 cmdType)
{
	net_printfChar(cmdType);
	net_send(CLIMAPROTOCOL.DESTINATION_ADDRESS, NET_NOACK); //change this to be with an ack
}

// *****************************************
// Method that sends out a command
// with data associated with the command
// *****************************************
void sendCommandwithData(u08 cmdType, u08 data[], u08 dataLength)
{
	u08 i = 0;
	net_printfChar(cmdType);
	for(i=0;i<dataLength;i++)
	{
		net_printfChar(data[i]);
	}
	net_send(CLIMAPROTOCOL.DESTINATION_ADDRESS, NET_NOACK); //change this to be with an ack
}

// *****************************************
// Method that sends out a message to the 
// ClimaReader in resonse to the reader's 
// Discover Tag Command
// *****************************************
void notifyOfPresence(void)
{
	
}

// *****************************************
// Method that sets the network address to 
// which the climatag belongs
// *****************************************
void setNetworkAddress(u08 newAddress)
{
	CLIMAPROTOCOL.NETWORK_ID = newAddress;
}


