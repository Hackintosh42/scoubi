/*********************************** MODULE INFO ************************************
	
   Device      : ATMega168
   File name   : ds2438.h
   Ver nr.     : 1.0
   Description : Dallas DS2438Z A/D convertor
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-07-11    JvA     
                 
*************************************************************************************/

#ifndef DS2438_H
#define DS2438_H
#include "avrheaders.h"
#include "global.h"	
#include "ow.h"
#include "owcrc.h"
#include "ownet.h"
#include "owtran.h"



// *****************************
//   DS2438 Register Addresses
// *****************************

#define DS2438_STATUS_REG_ADDR 0x00
#define DS2438_VOLTAGE_REG_ADDR 0x03 // 2 bytes

// *****************************
//   DS2438 Commands 
// *****************************
#define DS2438_CONVERT_V 0xB4

#define DS2438_WRITE_SCRATCHPAD 0x4E
#define DS2438_READ_SCRATCHPAD 0xBE
#define DS2438_COPY_SCRATCHPAD 0x48
#define DS2438_RECALL_MEMORY 0xB8



// **********************************
// Function Prototypes
// **********************************

  void DS2438_init(u08* OW_IDENTITY);
  void DS2438_Convert_V(u08* OW_IDENTITY);
  void  DS2438_Read_Scratchpad(u08* OW_IDENTITY, u08 pagenumber, u08* destptr);
  void DS2438_Write_Scratchpad(u08* OW_IDENTITY, u08 pagenumber, u08* srcptr);
  void DS2438_Copy_Scratchpad(u08* OW_IDENTITY,u08 pagenumber);
  void DS2438_Recall_Page(u08* OW_IDENTITY,u08 pagenumber);
  u08   DS2438_Read_Reg(u08* OW_IDENTITY,u08 pagenumber, u08 reg);
  void DS2438_Write_Reg(u08* OW_IDENTITY,u08 pagenumber, u08 reg,u08 data);
  u16 DS2438_read_Vad(u08* OW_IDENTITY);



#ifdef DS2438_C
#else
#endif

#endif //DS2438

