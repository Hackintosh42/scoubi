
#ifndef UART_H
#define UART_H

#include "avrheaders.h"
#include "global.h"		

#define TX_SERIAL_BUFFER_SIZE 30
#define RX_SERIAL_BUFFER_SIZE TX_SERIAL_BUFFER_SIZE // just 4 luck
#define UART_STRAIGHT_TX




typedef struct{ 
u08 rd_index;
u08 wr_index;
u08 transmitting;
u08 charbuffer[TX_SERIAL_BUFFER_SIZE];
}sTxSerial;


void uart_ClearRXbuffers(void);
void uartInit(void);
void uartSetBaudRate(u32 baudrate);
void uartSendChar(u08 data);
u08 uartGetChar(void);
/*
void print_decimal_u16(u16 uart_data);
void print_decimal_u08(u08 uart_data);
void rprintfChar(u08 data);
u08 Get_char(void);
void rprintfu08(u08 data);
void rprintfu16(u16 data);
void rprintfu32(u32 data);
void rprintfBits_u08(u08 data);
void rprintfBits_u32(u32 data);

void rprintf(u08* string);
//void rprintf_rom(PGM_P string);
void rprintf_rom(const u08 *string);
void rprintfBits_u16(u16 data);
*/
typedef struct{ 
u08 rd_index;
u08 wr_index;
u08 CarriageReturn_flag;
u08 StartDelim_flag;
u08 EndDelim_flag;
u08 Wakeup_flag;
u08 LastRxSerial_cnt;
u08 MessageReady_flag;
u08 MessageSize;
u08 Message[RX_SERIAL_BUFFER_SIZE];
u08 charbuffer[RX_SERIAL_BUFFER_SIZE];
}sRxSerial;


#ifdef UART_C
volatile	sRxSerial RxSerial;
volatile  sTxSerial TxSerial;
  static char __attribute__ ((progmem)) HexChars[] = "0123456789ABCDEF";
  
#else
	extern volatile	sRxSerial 	    	RxSerial;
  extern volatile	sTxSerial 	    	TxSerial;
  
#endif 

#endif // UART_H






