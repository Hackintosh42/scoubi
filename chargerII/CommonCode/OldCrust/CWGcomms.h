

/*********************************** MODULE INFO ****************************
	
   File name    : CWGComms.h
   Ver nr.      : 2.0
   Description  : Communications protocols for data transmission 
				   and reception over Melexis RF connection
   Compiler     : AVR GCC
   Company      : 
   Author       : Jason van Aardt 
 
   Change log   : 2004-10-04  Initial 
 
****************************************************************************/

#ifndef COMMS_H
#define COMMS_H
#include "headers.h"

#define COMMS_8SAMPLES
//#define COMMS_4SAMPLES
//#define COMMS_2SAMPLES



//#define NOISEFLOOR_FACTOR 100     // The amount above the Noise floor to initiate a receiving of bits
//#define NOISEFLOOR_FACTOR 50     // The amount above the Noise floor to initiate a receiving of bits
#define NOISEFLOOR_FACTOR 13     // The amount above the Noise floor to initiate a receiving of bits

#define NOISEACCUM_FACTOR 50
#define RSSIPKT_FACTOR 20     // The amount above the Noise floor to initiate a receiving of bits

#define COMMS_RX_SAMPLES_PER_ENCODED_BIT 4
#define COMMS_RX_SAMPLES_PER_BIT 8

/* VTAG_I
#define COMMS_RX_PORT PINC
#define COMMS_RX_PIN 3

#define COMMS_TX_PORT PORTD
#define COMMS_TX_PIN 2
*/

// VTAG_II
#define COMMS_RX_PORT PINC
#define COMMS_RX_PIN 3

#define COMMS_TX_PORT PORTB
#define COMMS_TX_PIN 1

#define DISABLE 0
#define ENABLE 1



//**********************************************
// The Transmit Q structure
//**********************************************
#define MAX_TX_PKT_SIZE 50
#define MAX_TX_ENCODED_PKT_SIZE (MAX_TX_PKT_SIZE*2)
#define CWG_PKT_HEADER_LEN PREAMBLE_LEN+4
#define CWG_MAX_MSG_LEN MAX_TX_PKT_SIZE - CWG_PKT_HEADER_LEN


//#define MAX_RX_SAMPLESBUF_SIZE 0x80
//#define MAX_RX_SAMPLESBUF_SIZE 0x20
//#define MAX_RX_SAMPLESBUF_SIZE 0xE0
//#define MAX_RX_BITBUF_SIZE MAX_RX_SAMPLESBUF_SIZE/COMMS_RX_SAMPLES_PER_BIT
#define MAX_RX_BITBUF_SIZE 40// 0x20

#ifdef COMMS_8SAMPLES    
  #define TAG_PREAMBLE_LEN 4
  //#define TAG_PREAMBLE_LEN 7
  #define READER_PREAMBLE_LEN 8  
#endif
#ifdef COMMS_4SAMPLES    
  #define TAG_PREAMBLE_LEN 7
  #define READER_PREAMBLE_LEN 12  
#endif
#ifdef COMMS_2SAMPLES    
  #define TAG_PREAMBLE_LEN 22
  #define READER_PREAMBLE_LEN 30
#endif


#define PREAMBLE_LEN TAG_PREAMBLE_LEN
/*
#ifdef TAG
  #define PREAMBLE_LEN TAG_PREAMBLE_LEN
#else
  #ifdef READER
    #define PREAMBLE_LEN READER_PREAMBLE_LEN
  #else 
    #define PREAMBLE_LEN TAG_PREAMBLE_LEN
  #endif
#endif
*/  


//#define PREAMBLE_BYTE 0xAA
#define PREAMBLE_BYTE 0xFF
//#define ENDPREAMBLE_BYTE 0x00
#define ENDPREAMBLE_BYTE 0x3C
#define MAGIC_BYTE 0x42



// The RSSI signal buffer to create the average
#define RSSIbuf_SIZE_POW2 3   //16 RSSI samples deep
#define RSSIbuf_SIZE (1 << RSSIbuf_SIZE_POW2)

enum{
CWG_NONE = 0,
CWG_SYNC,
CWG_PREAMBLE,
CWG_COMPLETE
};



typedef struct{ 
  u08 sample_cnt;
  u08 RSSI_measure_cnt;
}sComms;

typedef struct{ 
	u08 txLevel_u08;
	u08 rxLevel_u08;
}sCommsData;




typedef struct{ 
 
  u16 StartRSSI;
//  u08 Sync;
//  u08 PreambleFound;
  u08 Complete;
//  u08 len;
  
  u08 SignalRSSI;
  u08 CRCPass;
  u08 ReceivedMsgReady;
  u08 ReceivedMsgLen;  
  u08 ReceivedMsg_RSSI; 

  //u08 bitbuf_index;
  //u08 bit_bitmask;
  u16 samplescaptured_cnt;
  u08 bytescaptured;

  u08 bitbuf[MAX_RX_BITBUF_SIZE];
}sCWG;

/*
#define MAX_CWG_Q_SIZE 4
typedef struct{
 u08  wr;
 u08 completedwr;
 u08  rd;
 sCWG CWG[MAX_CWG_Q_SIZE];
}sCWGQ;
*/


typedef struct{ 
  u08 Sample;
  u08 Ones_cnt;
  
  u08 Ones_cmp;  
  u08 Ones_cmp_previous;    
  u08 Ones_cmp_sample;
  u08 Ones_cmp_sample_previous;
  
  u08 Sync;
  u08 SyncPos;
  u08 Sync_cnt;
//  u08 Synccomplete;
  u08 ShiftReg;
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
 u08 buf [MAX_RX_BITBUF_SIZE];
 u08 len;
 u08 rssi;
}sRxMsg;

typedef struct
{
  u08 rd;
  u08 wr;
  sRxMsg Msg[MAX_MSG_Q_SIZE];
}sRxMsgQ;



typedef struct{ 
  u08 Busy;
  u08 SignalFound;
  u16 RSSI;
//  u16 StartRSSI;
  u08 RSSIbuf_tailptr;
  u16 NoiseAccum;
  u16 NoiseFloor; 
  u16 RSSIInitRx;  
  u16 RSSIbuf[RSSIbuf_SIZE];  
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


/*
typedef struct{ 
u08 bitmask;
u08 full;
//u16 samplescaptured;
u08 bitscaptured;
u16 samplebuf_index;

u16 bitbuf_index;
u08 bit_bitmask;
u16 samplescaptured_cnt;

u08 samplebuf[MAX_RX_SAMPLESBUF_SIZE];
u08 bitbuf[MAX_RX_BITBUF_SIZE];
u08 alignedbitbuf[MAX_RX_BITBUF_SIZE];
}sRXSamples;
*/

/*
typedef struct{ 
u08 MagicPass;
u08 CRCPass;
u08 msgwaiting;
}sRXDecode;
*/

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


void Comms_BitTimer(void);
u08 Comms_TxBit_interrupt(void);
void Comms_RxBit_interrupt(void);


// Signal Functions
void Comms_ClearRSSIbuf(void);
u08 Comms_CheckSignal(void);
u08 Comms_Rxbit(void);
void Comms_Txbit(u08 bit);
void Comms_Resync(void);


void Comms_AddBitBuf(u08 data);

// Fast Blasting
u08 Comms_Sync(void);
void Comms_BlastSample(u08 data);
void Comms_BlastStart(void);
void Comms_BlastPostProcess(void);


// Based on the Chipcon type sample method
void Comms_DecodeSamples(void);
void Comms_ChipconSample(u08 data);
void Comms_CWGSample(u08 data);


// Receiver Routines
void Comms_ClearSamples(void);

// Transmitter Routines
void Comms_SendMsg(u08 *msg, u08 len);
//u08 Comms_SendPkt(u08 *pkt, u08 len);
u08 Comms_SendPkt(void);
u08 Comms_ManchesterEncodePkt(u08 *src,u08 len, u08* dest);
u08 Comms_CreatePkt(u08 *msg, u08 len,u08 *pkt);


// CRC and Payload Routines
void Comms_PutCRC(u08 b);
u16  Comms_CRC (u08 *Data,u08 Length);

// DSP functions
u08 Comms_Correlate_u08(u08 signal,u08 seq);
//#ifdef BLASTERCWG
void Comms_FindPreamble(sCWG *CWG);
void Comms_CheckReceivedMessage(sCWG *CWG);
//#else
//void Comms_CheckReceivedMessage(void);
//void Comms_FindPreamble(u08 *src_ptr,u08 len);
//#endif

//#ifdef BLASTERCWG
// CWG Q functions
sCWG *Comms_GetNextWrCWG(void);
sCWG *Comms_GetNextRdCWG(void);
u08 Comms_CheckCWGQ(void);
//#endif

// Msg Q functions
u08 Comms_CheckRxMsg(void);
void Comms_AddRxMsg(u08 *src_ptr,u08 len,u08 RSSI);
sRxMsg* Comms_GetNextRxMsg(void);


u08 Comms_SendWakeMsg(void);


#ifdef CWGCOMMS_C
  //sCWG CWG[COMMS_CWG_Q_SIZE];
  //sCWGQ CWGQ;
  sCWG CWG;
  
  sRxMsgQ RxMsgQ;  
//  sCWG CWG;
  sSMP  SMP;
	sTX TX;
	sRX RX;
	sComms Comms;
	sCommsData CommsData;
	//sRXSamples RXSamples;
  //unsigned char NewValue;
  
#else
//  extern sCWG CWG[COMMS_CWG_Q_SIZE];
//#ifdef BLASTERCWG
//  extern sCWGQ CWGQ;
  extern sCWG CWG;

//#endif  
  extern sRxMsgQ RxMsgQ;
//  extern sCWG CWG;
  extern sSMP SMP;
	extern sTX TX;
	extern sRX RX;
	extern sComms Comms;
	extern sCommsData CommsData;	
	//extern sRXSamples RXSamples;	
  //extern unsigned char NewValue;
  
#endif




#endif //COMMS_H



