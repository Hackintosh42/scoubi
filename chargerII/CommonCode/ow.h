#ifndef OW_H
#define OW_H

#include "avrheaders.h"
#include "global.h"		
#include "uart.h"

#define OW_AVR_PORTPIN_IMPLEMENTATION_ENABLE
//#define OW_AVR_DS2482_IMPLEMENTATION_ENABLE


#define MAX_PORTNUM    3


#include "owlnk_avr.h"
#include "owlnk_ds2482.h"
#include "ownet.h"
#include "owtran.h"


typedef struct
{
	u08 Port;
}sOW;



#define OW_PORT0 0
#define OW_PORT1 1
#define OW_PORT2 2


#define OWERROR(err) owRaiseError(err)

//******************************************************************

// mode bit flags
#define MODE_NORMAL                    0x00
#define MODE_OVERDRIVE                 0x01
#define MODE_STRONG5                   0x02
#define MODE_PROGRAM                   0x04
#define MODE_BREAK                     0x08

// From original 1wire.h
/* Return codes for OWFirst()/OWNext() */
#define OW_BADWIRE		-3
#define OW_BADCRC		-2
#define OW_NOPRESENCE		-1
#define OW_NOMODULES		0
#define OW_FOUND		1

/* General 1 wire commands */
#define OW_OVRDRV_SKIP_CMD	0x3c
#define OW_SEARCH_ALRM_CMD	0xec
#define OW_SEARCH_ROM_CMD	0xf0
#define OW_READ_ROM_CMD		0x33
#define OW_MATCH_ROM_CMD	0x55
#define OW_SKIP_ROM_CMD		0xcc

/* DS1820/1920 commands */
#define OW_CONVERTT_CMD		0x44
#define OW_RD_SCR_CMD		0xbe
#define OW_WR_SCR_CMD		0x4e
#define OW_CPY_SCR_CMD		0x48
#define OW_RECALL_CMD		0xb8
#define OW_RD_PSU_CMD		0xb4

/* HYGROCHRON commands */
#define READ_MEMORY_PSW_COMMAND  	0x69
#define STOP_MISSION_PW_COMMAND   0x33
#define CLEAR_MEMORY_PW_COMMAND   0x96
#define PAGE_LENGTH_HYGRO 			32
#define PAGE_LENGTH		 			64


/* Addressable Switch Commands DS2408 */
#define READ_MEMORY_COMMAND		0xF0
#define CHANNEL_ACCESS_WRITE    0x5A;





/* Family codes */
#define OW_FAMILY_ROM			0x09

#define OW_FAMILY_DS1920		0x10
#define OW_FAMILY_TEMP			OW_FAMILY_1923

#define OW_FAMILY_DS1923		0x41
#define OW_FAMILY_HYGRO			OW_FAMILY_1923
#define OW_FAMILY_THERMO		0x21
#define OW_FAMILY_SWITCH		0x29
#define OW_FAMILY_2405      0x05
#define OW_FAMILY_DS2438    0x26


//*******************************************************************


//*******************************************************************
void owDelay(void);
void owDelay480us(void);
void owRaiseError(u08 errorno);


//***************************************************************************
// Wrapper functions around the lowlevel 1-wire link level commands
// for accessing botht the Microprocessor 1-wire port and ds2482 ports
//***************************************************************************


void owSelectDevice(u08* OW_IDENTITY);
void owScanforDevices(u08 portnum, u08 numdevices, u08 Rom_array[][8], u08 targetFamily);
void owPrintOW_ROMID(u08* tmprom);

u08 owAcquire(u08 portnum, char *port_zstr);
void owRelease(u08 portnum);

u08 owTouchReset(u08 portnum);
u08 owTouchBit(u08 portnum, u08 sendbit);
u08 owTouchByte(u08 portnum, u08 sendbyte);
u08 owWriteByte(u08 portnum, u08 sendbyte);
u08 owReadByte(u08 portnum);
u08 owLevel(u08 portnum, u08 new_level);



/* 480 usec */
#define DELAY_H asm volatile(	\
	"ldi r21, 240\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")
  
/* 70 usec */    
#define DELAY_I asm volatile(	\
	"ldi r21, 70\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")  
  

#ifdef OW_C
  sOW OW;
#else
   extern sOW OW;
#endif 





#endif

