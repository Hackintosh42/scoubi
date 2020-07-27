/*********************************** MODULE INFO ************************************
	
   Device      : ATMega48
   File name   : Charger.h
   Ver nr.     : 1.0
   Description : Header file for Charger.C                
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 
 
*************************************************************************************/

//#include "global.h"
#include <avr/wdt.h>
#include <avr/io.h>
//#include <avr/signal.h>	
#include <avr/interrupt.h>	

#include <stdlib.h>


#include ".\CommonCode\global.h"
#include ".\CommonCode\uart.h"
#include ".\CommonCode\time.h"
#include ".\CommonCode\i2c.h"
#include ".\CommonCode\rprintf.h"
#include ".\CommonCode\lcd.h"
#include ".\CommonCode\lcdrprintf.h"
#include ".\CommonCode\SystemTimers.h"
#include ".\CommonCode\rprintf.h"



#include "kbd.h"
#include "Menu.h"
#include "led.h"
#include "BatteryPack_Comms.h"

#define SENSING_NTC_25DEGREES_ADC 827
#define TEMPERATURE_GRADIENT 21
/*
#define NTC_20degrees 
#define NTC_25degrees 880
#define NTC_30degrees 
#define NTC_35degrees 
#define NTC_40degrees 640
#define NTC_45degrees 
#define NTC_50degrees 
#define NTC_55degrees 472
#define NTC_60degrees 
*/

#define CHARGECOMPLETE_TEMPERATURE                0x40
#define CHARGECOMPLETE_TIME                       0x41
#define CHARGECOMPLETE_PACKDISCONNECTED_CURRENT   0x42
#define CHARGECOMPLETE_PACKDISCONNECTED_COMMS     0x43
#define CHARGECOMPLETE_COMMS_FAILURE              0x44
#define UNKNOWN_PACKTYPE                          0x45
      

// Bit handling macros
//************************************************************************************
/*
  #define SETBIT(x,y) (x |= (y))  // Set bit y in byte x
  #define CLRBIT(x,y) (x &= (~(y)))// Clear bit y in byte x
  #define CHKBIT(x,y) (x & (y))   // Check bit y in byte x
*/


// Function prototypes
//************************************************************************************
//void print_char(u08 uart_data);
//void Put_char(unsigned char data);
unsigned char Get_char(void);
int  Measure(unsigned char);
void Setup(void);
void Start_Pwm(void);
void Stop_Pwm(void);
void Calc_Charge_Voltage(void);
u08 Comms_Get_PackTime(void);      
u08 Comms_Get_PackStartStopTime(void);     
void Display_PackTimes(void); 	



// Bit handling macros
//************************************************************************************
#define SETBIT(x,y) (x |= (y))  // Set bit y in byte x
#define CLRBIT(x,y) (x &= (~(y)))// Clear bit y in byte x
#define CHKBIT(x,y) (x & (y))   // Check bit y in byte x


#define IBAT2   1     			//CURRENT measurement on ADC channel #4
#define VBAT2   0     			//VOLTAGE measurement on ADC channel #5
#define TBAT2   3     			//TEMPERATURE measurement on ADC channel #3

#define LED0    0x04     		//LED0 -> pin 0 on portD
#define LED1    0x08     		//LED1 -> pin 2 on portD
#define LED2    0x10     		//LED2 -> pin 3 on portD
                         
//#define OPAMP_CURRENT_STEP 25  // 1Bit A/D resolution = 2.4415mA

#define SENSE_RES_0R1

#ifdef SENSE_RES_0R22
  #define OPAMP_CURRENT_STEP 39  // 1Bit A/D resolution = 3.894mA
#endif

#ifdef SENSE_RES_0R11
  #define OPAMP_CURRENT_STEP 78  // 1Bit A/D resolution = 7.788mA
#endif

#ifdef SENSE_RES_0R1 
  #define OPAMP_CURRENT_STEP 84  // 1Bit A/D resolution = 8.566mA
#endif



//#define OPAMP_VOLTAGE_STEP 16 // 1 Bit A/D resolution = 16.1mV
//#define OPAMP_VOLTAGE_STEP 21   // 1 bit A/D res = 22.29mV
//#define OPAMP_VOLTAGE_STEP_MINOR 36

#ifdef SENSE_RES_0R22
  #define OPAMP_VOLTAGE_STEP 27   // 1 bit A/D res = 22.29mV
  #define OPAMP_VOLTAGE_STEP_MINOR 20
#endif

#ifdef SENSE_RES_0R1 
  #define OPAMP_VOLTAGE_STEP 27   // 1 bit A/D res = 22.29mV
  #define OPAMP_VOLTAGE_STEP_MINOR 00
#endif



#define SLA_CAPACITY  12000     // in mAH

#define BATTERYCONNECT_VOLTAGE_MIN 100
#define FAST_CURRENT_MAX_12V 200   // in mA/10
#define FAST_CURRENT_MIN 1

#define FAST_VOLTAGE_TERMINATE_6V 7000
#define FAST_VOLTAGE_TERMINATE_12V 13000 // in mV

#define TRICKLE_VOLTAGE_6V 405
#define TRICKLE_VOLTAGE_12V 780

#define TRICKLE_CURRENT_MAX 300
#define TRICKLE_CURRENT_MIN 5

#define MAXPWMCOUNT  2   

//*******************************************************
// BATTERY INFO
//*******************************************************
