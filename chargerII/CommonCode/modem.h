
#ifndef MODEM_H
#define MODEM_H
#include "avrheaders.h"
#include "global.h"		
#include "melexis.h"
#include "io.h"
#include "led.h"
#include "timers.h"
#include "uart.h"


//#define MODEM_8SAMPLES
#define MODEM_4SAMPLES
//#define MODEM_2SAMPLES


enum{
  MODEM_IDLE,
  MODEM_RX,
  MODEM_START_TX,
  MODEM_TX_VCOWAIT,
  MODEM_TX_BITS,
  MODEM_TX_FF
};
//MODEM_STANDBY



enum{
  MODEM_NONE,
  MODEM_TX_ONLY,
  MODEM_TX_THEN_RX, 
  MODEM_FULL_DUPLEX
};


typedef struct
{
	u16 RX_AFTER_TX_TIME;
}sRF_MISSION_DETAILS;




typedef struct{    
    volatile  u08 State;
    volatile  u08 NextState;
    volatile  u08 Enable;
    volatile  u08 SamplerEnabled;
    volatile  u08 BaseMode; 
    volatile  u08 TxFreq; 
    volatile  u08 RxFreq;    
    volatile  u08 CheckforSignal_flg;
    volatile  u08 StartTx_flg;
    volatile  u08 sample_cnt;
    volatile  u08 signal_check_complete;
    volatile  u08 Switch_IDLE_flg;
    volatile  u08 Switch_RX_flg;
    volatile  u08 Switch_TX_FF_flg;  
    volatile  u08 Switch_STANDBY;      
    volatile  u08 CheckNSamples_flg;    
    volatile  u16 NSamples;  
    volatile  u16 Start_NSample_Value;
    volatile  u08 TXBusy;
    volatile  u08 RXBusy;  
    volatile  u08 WakeSignalfound_flg;
    volatile  u08 Wakeup_flag;
    volatile  u08 VCO_cnt;
    volatile  u08 CW;
//    volatile  u08 LastRxSerial_cnt;
    volatile  u08 LoopBack;
    volatile  u08 BaseTxLevel;  
    volatile  u08 TxLevel; 
    volatile  u08 AckTimer;
    volatile  u08 AckReceived;   
    volatile  u08 Promiscuous;
    volatile  u08 LinkType;    
    volatile  u08 tick_wrap;    
    volatile  u08 tick_high;
    volatile  u08 tick_low;    
}sModem;


/*
#define MODEM_TXBIT_1     MELEXIS_DATA_IN_PORT |= (1<<MELEXIS_DATA_IN_PORTPIN)
#define MODEM_TXBIT_0     MELEXIS_DATA_IN_PORT &= ~(1<< MELEXIS_DATA_IN_PORTPIN)
#define MODEM_RXBIT       ((MELEXIS_DATA_OUT_PORT & (1 << MELEXIS_DATA_OUT_PORTPIN))>>MELEXIS_DATA_OUT_PORTPIN)
*/

#define MODEM_TXBIT_1     MODEM_TX_PORT |= (1<< MODEM_TX_PIN)
#define MODEM_TXBIT_0     MODEM_TX_PORT &= ~(1<< MODEM_TX_PIN)
#define MODEM_RXBIT       (MODEM_RX_PORT & (1 << MODEM_RX_PIN))>>MODEM_RX_PIN



#define MODEM_TIMER_DISABLE	TIMSK0 &= ~(1<<TOIE0); 
#define MODEM_TIMER_ENABLE	TIMSK0 |= (1<<TOIE0);	


//#define MODEM_SAMPLER_DISABLE	Modem.SamplerEnabled = 0; 
//#define MODEM_SAMPLER_ENABLE	Modem.SamplerEnabled = 1;	



#define MODEM_PAUSE	MODEM_TIMER_DISABLE;
#define MODEM_RESTART	if(Modem.Enable) MODEM_TIMER_ENABLE;



// **********************************************************************************
// **********************************************************************************

//#define MODEM_ADD_SOURCE_AND_DESTINATION_ADDRESS



#ifdef MODEM_4SAMPLES
  #define CWG_ONES_UPPER_THRESH 2
  #define MODEM_SAMPLE_CNT_TOP 4
  #define MODEM_SHIFTREG_XOR 0x02  
  #define MODEM_SYNC_THRESHOLD 15 
  //#define MODEM_SYNC_THRESHOLD 16 
  
  #define PREAMBLE_LEN 8    
  #define PREAMBLE_END_SYNC_MAXBITS ((PREAMBLE_LEN+3)*8)
  //#define PREAMBLE_END_SYNC_MAXBITS ((PREAMBLE_LEN)*8)
  
  #define MODEM_SHIFTREG_MSB 0x08
#endif

#ifdef MODEM_8SAMPLES
  #define CWG_ONES_UPPER_THRESH 4
  #define MODEM_SAMPLE_CNT_TOP 8
  #define MODEM_SHIFTREG_XOR 0x08  
  #define MODEM_SYNC_THRESHOLD 7
  #define PREAMBLE_LEN 6
  #define PREAMBLE_END_SYNC_MAXBITS ((PREAMBLE_LEN+3)*8)
  #define MODEM_SHIFTREG_MSB 0x80 
#endif



//#define COMMS_RX_SAMPLES_PER_ENCODED_BIT 4
//#define COMMS_RX_SAMPLES_PER_BIT 8


#define MODEM_RX_PORT PINC
#define MODEM_RX_PIN 3

#define MODEM_TX_PORT PORTB
#define MODEM_TX_PIN 1

#define DISABLE 0
#define ENABLE 1



//**********************************************
// The  structure
//**********************************************
#define MAX_TX_PKT_SIZE 30//30


#ifdef MODEM_ADD_SOURCE_AND_DESTINATION_ADDRESS
  #define CWG_PKT_HEADER_LEN PREAMBLE_LEN+8
#else
  #define CWG_PKT_HEADER_LEN PREAMBLE_LEN+4
#endif

#define MODEM_MAX_TX_MSG_LEN (MAX_TX_PKT_SIZE - CWG_PKT_HEADER_LEN)

#define MAX_RX_BITBUF_SIZE (MAX_TX_PKT_SIZE+2) 


#define PREAMBLE_BYTE 0xFF
#define ENDPREAMBLE_BYTE 0x3C
#define MAGIC_BYTE 0x42


enum{
  CWG_NONE = 0,
  CWG_SYNC,
  CWG_PREAMBLE,
  CWG_COMPLETE
};

typedef struct{ 
  u08 timeout_flg;
  u08 stoptick_low;
  u08 stoptick_high;
  u08 stoptick_wrap;
}sBackGroundTimer;


typedef struct{ 
 
  u16 StartRSSI;
  u08 Complete;
  u08 SignalRSSI;
  u08 CRCPass;
  u08 ReceivedMsgReady;
  u08 ReceivedPacketLen;  
  u08 ReceivedPacket_RSSI; 
  u16 samplescaptured_cnt;
  u08 bytescaptured;
  u08 bitbuf[MAX_RX_BITBUF_SIZE];
}sCWG;


typedef struct{ 
  u08 Ones_cnt;
  
  u08 Ones_cmp;  
  u08 Ones_cmp_previous;    
  u08 Ones_cmp_sample;
  u08 Ones_cmp_sample_previous;
  
  u08 Sync;
  u08 SyncPos;
  u08 Sync_cnt;
  u08 ShiftReg;
  u08 Shifty;  
  u08 GoodBitCount;
  u08 Preamblefound;
  u08 Complete;
  
  u08 bit;
  u08 preamble_cnt;
  u08 preamblebyte[3];
  
  u08 bitbuf_index;
  u08 bit_bitmask;  
  u08 len;
  sCWG *CWG;  
}sSMP;



#define MAX_MSG_Q_SIZE 2


typedef struct
{
   //u16 sourceaddr;   
   u08 rssi;   
   u08 len;
   u08 buf [MAX_RX_BITBUF_SIZE];
}sModemRxMsg;

typedef struct
{
    u08 rd;
    u08 wr;
    sModemRxMsg Msg[MAX_MSG_Q_SIZE];
}sModemRxMsgQ;



typedef struct{ 
  u08 Busy;
  u08 SignalFound;
  u16 RSSI;
//  u16 StartRSSI;
  //u08 RSSIbuf_tailptr;
  //u16 NoiseAccum;
  //u16 NoiseFloor; 
  u16 RSSIInitRx;  
//  u16 RSSIbuf[RSSIbuf_SIZE];  
}sRX;



typedef struct{ 
	u08 Tx_cnt;
	u08 Complete;
	u08 bitmask;
	u08 bytecnt;
	u08 txbyte;	
	u08 pktbuf_ptr;
	u08 pktbuf_len;
	u08 pktbuf[MAX_TX_PKT_SIZE];
}sTX;


/*crc-16 standard root*/
#define POLYNOMIAL 0x8005
/*place your own here*/
#define INITIAL_VALUE 0x0000  

union {
   unsigned long Whole;
   struct 
   {
      unsigned char Data;
      unsigned int  Remainder;
      unsigned char Head;
   }Part;
}CRC_buffer;


//=========================================
// Function Prototypes
//=========================================

void Modem_SetTxFreq(u08 freq);
void Modem_SetRxFreq(u08 freq);
void Modem_Set_BaseTxLevel(u08 level);
void Modem_Reset_TxLevel(void);
void Modem_TxLevel_inc(void);
void Modem_SetTxLevel(u08 level);



void Modem_Init(void);
void Modem_Switch(u08 mode);

void Modem_Receive_NSamples(u16 samples);

void Modem_Set_BaseMode(u08 basemode);
void Modem_Set_RxLNA(u08 lna);
inline void Modem_AddRxMsg(u08 *src_ptr,u08 len,u08 RSSI);
u08  Modem_CheckRxQ(void);
sModemRxMsg *Modem_GetNextRxMsg(void);
inline void Modem_BlastStart(void);
void Modem_ClearRSSIbuf(void);
u08  Modem_CreatePkt(u08 *msg, u08 len,u08 *pkt);
u08  Modem_Send(u08 *msg, u08 len);
void Modem_PutCRC(u08 b);
u16  Modem_CRC (u08* Data,u08 Length);

void Modem_CRC_Reset(void);
void Modem_CRC_Byte(u08 data);
u16 Modem_CRC_GetResult(void);

void Modem_Timer_Wait_ms(u16 delaytime);
void Modem_StartBackground_Timer_ms(u16 delaytime);




//=========================================
// Variables
//=========================================

#ifdef MODEM_C
  volatile	sModem Modem;
	u08 BlastPostProcess_flg;
  sCWG CWG;  
  sModemRxMsgQ ModemRxMsgQ;  
  sSMP  SMP;
	sTX TX;
	sRX RX;
  sBackGroundTimer BackGroundTimer;  
  sRF_MISSION_DETAILS RF_MISSION_DETAILS;
  
#else
  volatile	extern sModem Modem;
	extern u08 BlastPostProcess_flg;
  extern sCWG CWG;
  extern sModemRxMsgQ ModemRxMsgQ;
  extern sSMP SMP;
	extern sTX TX;
	extern sRX RX;
  extern sBackGroundTimer BackGroundTimer;  
  extern sRF_MISSION_DETAILS RF_MISSION_DETAILS;

#endif



#endif //MODEM_H




