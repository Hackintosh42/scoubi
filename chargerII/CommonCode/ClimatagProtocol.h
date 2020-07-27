/*********************************** MODULE INFO ****************************
	
   File name    : ClimatagProtocol.h
   Ver nr.      : 1.0
   Description  : Contains the commands for the communication between the ClimaTags and the ClimaAPI
   Compiler     : AVR GCC
   Company      : Fairbridge Technologies
   Author       : Jaco Raubenheimer
 
   Change log   : 2005-06-29  Initial 
 
****************************************************************************/


#ifndef CLIMAPROTO_H
#define CLIMAPROTO_H

#include "avrheaders.h"
#include "global.h"
#include "network.h"

// GENERAL DEFINES
#define BROADCAST_ADDRESS										0xFFFF
#define	NO_DEFINED_NETWORK									0xFF

// Tag to Reader Commands
#define PRESENCE_COMMAND										0x98
#define MISSION_CONNECT_SUCCESS_COMMAND 		0x02
#define MISSION_DISCONNECT_SUCCESS_COMMAND 	0x04
#define MISSION_CMD_SUCCESS_COMMAND					0x10
#define SENSOR_READING_COMMAND							0x22
#define SENSOR_DEVICE_COUNT_COMMAND					0x25
#define SENSOR_ID_COMMAND										0x27
#define DEVICE_SAMPLECOUNT_COMMAND					0x32
#define DEVICE_MISSION_TIME_STAMP_COMMAND		0x34
#define DEVICE_MISSION_DATE_STAMP_COMMAND		0x36
#define DEVICE_TIME_COMMAND									0x38
#define DEVICE_DATE_COMMAND									0x42
#define DEVICE_LOGSIZE_COMMAND							0x44
#define DEVICE_SAMPLERATE_COMMAND						0x46
#define DEVICE_HIGH_AMBIENT_ALARM_COMMAND		0x48
#define DEVICE_LOW_AMBIENT_ALARM_COMMAND		0x4A
#define TRANSMIT_ONLY_SENSOR_READING_COMMAND 0x4B
#define MODEM_CMD_SUCCESS_COMMAND						0x50
#define GENERAL_CMD_SUCCESS_COMMAND					0x20


// Reader to Tag Commands
#define TX_ONLY_ACK_COMMAND									0x2A
#define DISCOVER_ALL_TAGS_CMD								0x99
#define MISSION_CONNECT_COMMAND							0x01
#define MISSION_DISCONNECT_COMMAND					0x03
#define TAG_SET_TIME_COMMAND								0x11
#define TAG_SET_DATE_COMMAND								0x12
#define TAG_SET_SAMPLERATE_COMMAND					0x13
#define TAG_SET_STARTNEWMISSION_COMMAND			0x14
#define TAG_SET_NETWORK_COMMAND							0x15
#define TAG_SET_HIGH_AMBIENT_ALARM_COMMAND	0x16
#define TAG_SET_LOW_AMBIENT_ALARM_COMMAND		0x17
#define TAG_DOWNLOAD_LOG_ALL_COMMAND 				0x21
#define TAG_DOWNLOAD_LOG_ONE_COMMAND				0x23
#define TAG_GET_SENSOR_DEVICE_COUNT_COMMAND	0x24
#define TAG_GET_SENSOR_ID_COMMAND						0x26
#define TAG_SET_REALTIME_ENABLE_COMMAND			0x28
#define TAG_SET_REALTIME_DISABLE_COMMAND		0x29
#define TAG_GET_SAMPLECOUNT_COMMAND					0x31
#define TAG_GET_MISSION_TIME_STAMP_COMMAND	0x33
#define TAG_GET_MISSION_DATE_STAMP_COMMAND	0x35
#define TAG_GET_DEVICE_TIME_COMMAND					0x37
#define TAG_GET_DEVICE_DATE_COMMAND					0x41
#define TAG_GET_LOGSIZE_COMMAND							0x43
#define TAG_GET_SAMPLERATE_COMMAND					0x45
#define TAG_GET_HIGH_AMBIENT_ALARM_COMMAND	0x47
#define TAG_GET_LOW_AMBIENT_ALARM_COMMAND		0x49
#define TAG_SET_RAM_COMMAND									0x51
#define TAG_SET_ARM_COMMAND									0x52
#define TAG_SET_TX_ONLY_COMMAND							0x53
#define TAG_SET_TX_WITH_ACK_COMMAND					0x54


// CLIMATAG VARIABLES *************
typedef struct
{
	u08 NO_OF_DEVICES;// = 0x00;
	u16 DEVICE_ID;// = 0x0100;
	u08 NETWORK_ID;// = NO_DEFINED_NETWORK;
	u08 MODEMLINK;// = 0x00;
	u16 DESTINATION_ADDRESS;
}sCLIMAPROTOCOL;
// ********************************

// FUNCTION PROTOTYPES ************
void initClimatagProtocol(void);
void setupNetAddress(void);
void setDestinationAddress(u16 addr);
void sendCommandwithNoData(u08 cmdType); // SHOULD COMBINE THE METHODS INTO ONE TO SAVE SPACE
void sendCommandwithData(u08 cmdType, u08 data[], u08 dataLength);
void notifyOfPresence(void);
void setNetworkAddress(u08 newAddress);
// ********************************

#ifdef CLIMAPROTOCOL_C
	sCLIMAPROTOCOL	CLIMAPROTOCOL;
#else
  extern sCLIMAPROTOCOL	CLIMAPROTOCOL;
#endif 


#endif


