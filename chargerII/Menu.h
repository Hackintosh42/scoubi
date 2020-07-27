
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : Menu.h
   Ver nr.     : 1.0
   Description : Header file for the Menu's 
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#ifndef MENU_H
#define MENU_H

#include <avr/wdt.h>

#include ".\CommonCode\global.h"
#include ".\CommonCode\lcd.h"
#include ".\CommonCode\lcdrprintf.h"
#include ".\CommonCode\time.h"
#include ".\CommonCode\eeprom.h"
#include "BatteryPack_comms.h"




#include "kbd.h"


#define VIRGIN_ADDR                 0
#define STARTTIME_HOUR_ADDR         1
#define STOPTIME_HOUR_ADDR          3
#define LAST_SERIAL_ADDR            10



  enum{
    MENU_TIME = 0,
    MENU_SET_TIME,
    MENU_STARTTIME,
    MENU_SET_STARTTIME,
    MENU_STOPTIME,
    MENU_SET_STOPTIME,
    //MENU_PACKINFO, 
    MENU_CHARGINGINFO
  };  
  


/* Typedefs */
typedef struct{ 
  u08 CursorPosX;
  u08 CursorPosY;
  
  u08 Cursor_Blink_enable;
  u08 Screen;
  u08 Change;

  u08 hours;
  u08 minutes;
  u08 seconds;
  u08 Prevseconds;  
}sMenu;

void Menu_Change(u08 screen);
void Menu_Show(void);
void Menu_Show_Time(u08 Type);
void Menu_Display_TimeDigits(u08 x, u08 y,u08 hours, u08 minutes);

//void Menu_Show_Charging_VI(u16 volta_mV, u16 currenta_mA);
//void Menu_Show_Charging_VI(u16 volta_mV, u16 currenta_mA,u16 temperature,u08 oc,u16 ChargeTime);
void Menu_Show_Charging_VI(u08 oc);



#ifdef MENU_C
  sMenu Menu;
#else
  extern sMenu Menu;
#endif

#endif
