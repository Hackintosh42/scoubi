
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : Menu.h
   Ver nr.     : 1.0
   Description : Header file for the Comms with the Battery Pack
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef BATTERYPACK_COMMS_H
#define BATTERYPACK_COMMS_H

#include ".\CommonCode\global.h"
#include ".\CommonCode\time.h"

//#define MASTER_COMMS
#define SLAVE_COMMS

//#define SERIAL_NUMBERGEN
//#define READTIME_TEST
//#define FEEVERBEEPER_TEST

#define PACKTYPE_ST             0x01
#define PACKTYPE_MB             0x02

#define CMD_GET_PACKINFO        0x50
#define CMD_SET_PACKINFO        0xDD
#define CMD_GET_RECHARGENO      0x52
#define CMD_GET_TEMPERATURE     0x53
#define CMD_GET_OPTIME_MINS     0x54
#define CMD_GET_TIME            0x55
#define CMD_SET_TIME            0x56
#define CMD_INC_RECHARGECOUNT   0x59
#define CMD_SET_STARTTIME       0x20
#define CMD_GET_STARTTIME       0x21
#define CMD_SET_STOPTIME        0x22
#define CMD_GET_STOPTIME        0x23
#define CMD_SET_STARTSTOPTIME   0x24
#define CMD_GET_STARTSTOPTIME   0x25
#define CMD_UNKNOWN             0xBB

#define COMMS_DATA_BYTES 5


//***************************************
//***************************************

//#define DEBUG_GETPKT

#define MASTER_WAITBITTIME 75
#define MASTER_WAITSETUP 10


#define SPI_PORT     PORTB
#define SPI_PORT_DDR DDRB
#define SPI_PORT_PIN PINB
#define SPI_SCK      5
#define SPI_MISO     4
#define SPI_MOSI     3
   

#define COMMS_SPI_SCK_READ (SPI_PORT_PIN & (1<< SPI_SCK))
#define COMMS_SPI_MISO_READ (SPI_PORT_PIN & (1<< SPI_MISO))
#define COMMS_SPI_SCK_1 SPI_PORT = SPI_PORT | (1<< SPI_SCK)
#define COMMS_SPI_SCK_0 SPI_PORT = SPI_PORT & ~(1<< SPI_SCK)  
#define COMMS_SPI_MOSI_1 SPI_PORT = SPI_PORT | (1<< SPI_MOSI)
#define COMMS_SPI_MOSI_0 SPI_PORT = SPI_PORT & ~(1<< SPI_MOSI)  
#define COMMS_SPI_MISO_1 SPI_PORT = SPI_PORT | (1<< SPI_MISO)
#define COMMS_SPI_MISO_0 SPI_PORT = SPI_PORT & ~(1<< SPI_MISO) 
#define COMMS_SPI_MOSI_READ (SPI_PORT_PIN & (1<< SPI_MOSI))


/* Typedefs */
typedef struct{ 
  u08 PackType;
  u16 SerialNo;  
  u16 RechargeNo;
  u16 Temperature;
  u16 Temperature_deg;      
  u16 OpTime_mins;
  sTime Time;
  sTime StartTime;
  sTime StopTime;
  sTime ChargeStart;
  sTime ChargeTime;      
  u16 MaxChargeTime;      
  u16 ChargeTime_mins;
  u16 Previous_inc_current_mins;
  u16 Connected_cnt;
  u16 volta;
  u16 currenta;
  u16 volta_mV;
  u16 currenta_mA;      
}sPackInfo;
        
  #ifdef BATTERYPACK_COMMS_C
    sPackInfo PackInfo;
  #else
    extern sPackInfo PackInfo;
  #endif

#endif

