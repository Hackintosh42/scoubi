

/*********************************** MODULE INFO ****************************
	
   File name    : Melexis.h
   Ver nr.      : 1.0
   Description  : Routines to set the internal registers of the Melexis TH7122
   Compiler     : 
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-08-14  Initial 
 
****************************************************************************/

#ifndef MEL_H
#define MEL_H
#include "avrheaders.h"
#include "global.h"		


#include "MelexisRegVal.h"


#define VCOCUR_SZ    2 


//************************************
// Defines
//************************************


#define MELEXIS_DATA_IN_DDR DDRB
#define MELEXIS_DATA_IN_PORT PORTB  
#define MELEXIS_DATA_IN_PORTPIN	 1
  
#define MELEXIS_DATA_OUT_DDR DDRC
#define MELEXIS_DATA_OUT_PORT PORTC
#define MELEXIS_DATA_OUT_PORTPIN	 3



// VTAG_III
#define SPI_PORT PORTB
#define SDA_PIN 3
#define SCL_PIN 5

#define SDEN_PORT PORTB
#define SDEN_PIN  2	

//*********************************************
//*********************************************
	
enum{
  FREQ_411_MHZ,
  FREQ_433_MHZ,
  FREQ_456_MHZ
};

enum{
  FREQ_TX_403,
  FREQ_TX_433
};

enum{
  FREQ_RX_403,
  FREQ_RX_433
};
    

enum{ MEL_STANDBY,
	   MEL_TX,
	   MEL_RX,
	   MEL_IDLE
};   

enum {
  MEL_REG_A = 0,
  MEL_REG_B,
  MEL_REG_C,
  MEL_REG_D
};

typedef struct{ 
  u32 A;
  u32 B;
  u32 C;
  u32 D;
}sMel_Reg;

typedef struct{ 
  u08 A[3];
  u32 B[3];
  u32 C[3];
  u32 D[3];
}sUltraMel_Reg;



typedef struct{ 
  u08 Mode;
  u08 TxFreq;
  u08 RxFreq;
  u08 txLevel_u08;
  u08 lna_u08;  
}sMel;


//***********************************
// Prototypes
//***********************************


void Mel_SDA(u08 val);
void Mel_SCL(u08 val);
void Mel_SDEN(u08 val);

//void Mel_Wr(u08 addr, u32 data);
void Mel_Wrreg(u32 data);

void Mel_LoadRegs(void);
void Mel_Init(void);
void Mel_Mode(u08 data);
void Mel_SetRxFreq(u08 freq);
void Mel_SetTxFreq(u08 freq);



#ifdef MELEXIS_C
  sMel     Mel;
	sMel_Reg Mel_Reg;
#else
  extern sMel     Mel;
	extern sMel_Reg Mel_Reg;
#endif


#endif //MELEXIS_H




