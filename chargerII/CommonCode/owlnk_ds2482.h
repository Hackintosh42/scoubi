

#ifndef OWLNK_DS2482_H
#define OWLNK_DS2482_H
#include "avrheaders.h"
#include "global.h"		
#include "i2c.h"
#include "ow.h"


#define DS2482_I2C_ADDR0 0x18


#define DS2482_DEVICE_RESET 0xF0
#define DS2482_SET_READ_POINTER 0xE1
#define DS2482_WRITE_CONFIGURATION 0xD2
#define DS2482_1WIRE_RESET 0xB4
#define DS2482_1WIRE_SINGLE_BIT 0x87
#define DS2482_1WIRE_WRITE_BYTE 0xA5
#define DS2482_1WIRE_READ_BYTE 0x96
#define DS2482_1WIRE_TRIPLET 0x78

#define DS2482_STATUSREGISTER 0xF0
#define DS2482_DATAREGISTER 0xE1
#define DS2482_CONFIGREGISTER 0xC3

#define DS2482_STATUSBIT_DIR 7
#define DS2482_STATUSBIT_TSB 6
#define DS2482_STATUSBIT_SBR 5
#define DS2482_STATUSBIT_RST 4
#define DS2482_STATUSBIT_LL  3
#define DS2482_STATUSBIT_SD  2
#define DS2482_STATUSBIT_PPD 1
#define DS2482_STATUSBIT_1WB 0

#define DS2482_CONFIGBIT_1WS  3
#define DS2482_CONFIGBIT_SPU  2
#define DS2482_CONFIGBIT_PPM  1
#define DS2482_CONFIGBIT_APU  0



//void owTestScan(u08 portnum);


#ifdef OW_AVR_DS2482_IMPLEMENTATION_ENABLE

void DS2482_DeviceReset(u08 addr);
void DS2482_WriteConfiguration(u08 addr, u08 configbyte);
void DS2482_SetReadPointer(u08 addr, u08 addrreg);



u08 owAcquire_ds2482(u08 portnum, char *port_zstr);

void owBusyWait_ds2482(u08 portnum);
// exportable link-level functions
u08 owTriplet_ds2482(u08 portnum);

u08 owTouchReset_ds2482(u08);
u08 owTouchBit_ds2482(u08,u08);
u08 owTouchByte_ds2482(u08,u08);
u08 owWriteByte_ds2482(u08,u08);
u08 owReadByte_ds2482(u08);
u08 owLevel_ds2482(u08,u08);

#endif //OW_AVR_DS2482_IMPLEMENTATION_ENABLE

#endif


