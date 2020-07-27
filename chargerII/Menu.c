
/*********************************** MODULE INFO ************************************
	
   Device      : ATMega8
   File name   : Menu.c
   Ver nr.     : 1.0
   Description : C file for the Menu system
   Compiler    : GCC
   Author      : Jason van Aardt
 
   Change log  : 2005-08-09 Initial Version
 
*************************************************************************************/

#define MENU_C
#include "Menu.h"

#define MENU_SET_TIME_HOUR_COLON_X  7


extern volatile u08 MasterTimePresent;


//**************************************************
// void Show_Time(void)
// Shows the current time of day on the LCD
//**************************************************
u08 oogle;
void Menu_Show_Time(u08 type)
{
    s08 hours, minutes, seconds;
    wdt_reset();

  lcdClear();  
        
	Time_readtime((u08*)&hours,(u08*)&minutes,(u08*)&seconds);                        
	lcdGotoXY(4,1);	     
	lcdprint_dec_u08(hours,1);
	lcdprint(":");
	lcdprint_dec_u08(minutes,1);    
	lcdprint(":");
	lcdprint_dec_u08(seconds,1);
	lcdprintat(3,0,"-Primedia-");         
	
	
	#ifdef MASTER_COMMS
		MasterCharger_SendTime();
	if(oogle)
	{
		oogle = 0;
		lcdprintat(0,0,"*");         		
	}
	else
	{
		oogle = 1;
		lcdprintat(0,0," ");         				
	}
		
	#endif
		

    
/*	
    switch(type)
    {
      case MENU_TIME:
        lcdprintat(3,0,"-Primedia-");         
        Time_readtime((u08*)&hours,(u08*)&minutes,(u08*)&seconds);                        
        lcdGotoXY(4,1);	     
        lcdprint_dec_u08(hours,1);
        lcdprint(":");
        lcdprint_dec_u08(minutes,1);    
        lcdprint(":");
        lcdprint_dec_u08(seconds,1);
      break;
      
      case MENU_STARTTIME:
        lcdprintat(3,0,"Start Time");     
        // Read the previous values set from EEPROM
        hours = EEPROM_INT_read(STARTTIME_HOUR_ADDR);
        minutes = EEPROM_INT_read(STARTTIME_HOUR_ADDR+1);    
        Menu_Display_TimeDigits(5,1,hours,minutes);
      break;
      
      case MENU_STOPTIME:
        lcdprintat(3,0,"Stop Time");           
        hours = EEPROM_INT_read(STOPTIME_HOUR_ADDR);
        minutes = EEPROM_INT_read(STOPTIME_HOUR_ADDR+1);                    
        Menu_Display_TimeDigits(5,1,hours,minutes);       
      break;
      
    }  
*/                 
}





//**************************************************
// void Menu_Set_Time_Display_Digits(void)
// Allows the user to set the time of day on the LCD
//**************************************************
void Menu_Display_TimeDigits(u08 x, u08 y,u08 hours, u08 minutes)
{
	  lcdGotoXY(x,y);	     
	  lcdprint_dec_u08(hours,1);
	  lcdprint(":");
    lcdprint_dec_u08(minutes,1);
}



//**************************************************
// void Menu_Set_Time(void)
// Allows the user to set the time of day on the LCD
//**************************************************
void Menu_Set_Time(u08 type)
{
  s08 hours, minutes, seconds;
  
    lcdClear();  

	lcdprintat(3,0,"*Set Time*");     
	Time_readtime((u08*)&hours,(u08*)&minutes,(u08*)&seconds);                  
	
/*	
    switch(type)
    {
      case MENU_SET_TIME:
        lcdprintat(3,0,"*Set Time*");     
        // Get the time
        Time_readtime((u08*)&hours,(u08*)&minutes,(u08*)&seconds);                  
      break;
      
      case MENU_SET_STARTTIME:
        lcdprintat(1,0,"*Set Start Time*");     
        // Read the previous values set from EEPROM
        hours = EEPROM_INT_read(STARTTIME_HOUR_ADDR);
        minutes = EEPROM_INT_read(STARTTIME_HOUR_ADDR+1);              
      break;
      
      case MENU_SET_STOPTIME:
        lcdprintat(1,0,"*Set Stop Time*");           
        hours = EEPROM_INT_read(STOPTIME_HOUR_ADDR);
        minutes = EEPROM_INT_read(STOPTIME_HOUR_ADDR+1);                    
      break;      
    }
*/	
    seconds = 0;
    

    
    Menu.CursorPosX = MENU_SET_TIME_HOUR_COLON_X -1;
    Menu.CursorPosY = 1;
    lcdGotoXY(Menu.CursorPosX, Menu.CursorPosY);
    // Blink
    lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_CURSOR);

    Menu_Display_TimeDigits(MENU_SET_TIME_HOUR_COLON_X-2, Menu.CursorPosY, hours,minutes);

    wdt_reset();

    // Pause to allow the Enter Key to resettle
    delay(15000);
    

    u08 Complete =0;
    while(!Complete)
    {   
      wdt_reset();
      lcdGotoXY(Menu.CursorPosX, Menu.CursorPosY);    
      // Scan the keyboard 
      u08 key = Kbd_Scan();
      if ( key != NO_KEY)
      {
        switch(key)
        {
          case RIGHT_KEY:
            Menu.CursorPosX++;
            if(Menu.CursorPosX == MENU_SET_TIME_HOUR_COLON_X )
              Menu.CursorPosX = MENU_SET_TIME_HOUR_COLON_X+2;
            if(Menu.CursorPosX >MENU_SET_TIME_HOUR_COLON_X+2)
              Menu.CursorPosX = MENU_SET_TIME_HOUR_COLON_X+2;              
          break;
          
          case LEFT_KEY:
            Menu.CursorPosX--;
            if(Menu.CursorPosX == MENU_SET_TIME_HOUR_COLON_X+1 )            
              Menu.CursorPosX = MENU_SET_TIME_HOUR_COLON_X-1;
            if(Menu.CursorPosX <MENU_SET_TIME_HOUR_COLON_X-1)              
            if(Menu.CursorPosX <6)
              Menu.CursorPosX = MENU_SET_TIME_HOUR_COLON_X-1;
          break;
          
          case UP_KEY:
            if(Menu.CursorPosX <7)
            {            
              hours++;
              if (hours > 23)
                hours = 0;            
            }
            else 
            {
            
              minutes++;
              if (minutes > 59)
                minutes = 0;                        
            }
            Menu_Display_TimeDigits(MENU_SET_TIME_HOUR_COLON_X-2, Menu.CursorPosY, hours,minutes);
          break;
          
          case DOWN_KEY:
            if(Menu.CursorPosX <7)
            {
              hours--;
              if (hours < 0)
                hours = 23;                
            }
            else 
            {
              minutes--;
              if (minutes < 0)                
                minutes = 59;                        
            }         
            Menu_Display_TimeDigits(MENU_SET_TIME_HOUR_COLON_X-2, Menu.CursorPosY, hours,minutes);
          break;
          
          case ENTER_KEY:
            switch(type)
            {
              case MENU_SET_TIME:
                Time_settime(hours,minutes,seconds);
				#ifdef MASTER_COMMS
					MasterCharger_SendTime();				
				#endif
                Menu.Screen = MENU_TIME;
              break;
              /*
              case MENU_SET_STARTTIME:
                EEPROM_INT_write(STARTTIME_HOUR_ADDR,hours);
                EEPROM_INT_write(STARTTIME_HOUR_ADDR+1,minutes);
                Menu.Screen = MENU_STARTTIME;              
              break;
              
              case MENU_SET_STOPTIME:
                EEPROM_INT_write(STOPTIME_HOUR_ADDR,hours);
                EEPROM_INT_write(STOPTIME_HOUR_ADDR+1,minutes);                             
                Menu.Screen = MENU_STOPTIME;                            
              break;              
			  */
            }                     
            Complete  = 1;
            lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY);            
          break;             
        }
      }
      
     } 
    
}

//***************************************************
// void Menu_Change(u08 screen)
// Change to the select screen
//***************************************************
void Menu_Change(u08 screen)
{
  Menu.Screen = screen;
}



//***************************************************
// void Menu_Show(void)
// Show the Current Menu
//***************************************************
void Menu_Show(void)
{
  u08 key = Kbd_Scan();
  wdt_reset();

  switch(Menu.Screen)
  {
    case MENU_TIME:
      switch(key)
      {
        case ENTER_KEY:
		  #ifdef MASTER_COMMS
			Menu.Screen = MENU_SET_TIME; 
		  #else
			if(MasterTimePresent == 0)
				Menu.Screen = MENU_SET_TIME; 			
		  #endif
        break;
		#ifdef CHANGE_STARTSTOP_TIME		
        case UP_KEY:
          Menu.Change = 1;                
          Menu.Screen = MENU_STOPTIME;        
        break;
        case DOWN_KEY:
          Menu.Change = 1;                
          Menu.Screen = MENU_STARTTIME;        
        break;                
		#endif
        case NO_KEY:
            // Check if the seconds have changed
            Time_readtime((u08*)&Menu.hours,(u08*)&Menu.minutes,(u08*)&Menu.seconds);                              
            if(Menu.seconds != Menu.Prevseconds)
            {
              Menu.Prevseconds = Menu.seconds;
              Menu_Show_Time(MENU_TIME);
            }
        break;        
      }
    break;
    
    case MENU_SET_TIME:
      Menu_Set_Time(MENU_SET_TIME);    
    break;
    
	/*
    case MENU_STARTTIME:
      switch(key)
      {
		#ifdef CHANGE_STARTSTOP_TIME
        case ENTER_KEY:
          Menu.Change = 1;        
          Menu.Screen = MENU_SET_STARTTIME;        
        break;
		#endif
        case UP_KEY:
          Menu.Change = 1;        
          Menu.Screen = MENU_TIME;        
        break;
        case DOWN_KEY:
          Menu.Change = 1;        
          Menu.Screen = MENU_STOPTIME;        
        break;                
        case NO_KEY:
            if(Menu.Change)
            {
              Menu.Change = 0;
              Menu_Show_Time(MENU_STARTTIME);
            }
        break;        
      }    
    break;
    
    
    case MENU_SET_STARTTIME:
      Menu_Set_Time(MENU_SET_STARTTIME);    
    break;

    case MENU_STOPTIME:
      switch(key)
      {
        case ENTER_KEY:
          Menu.Screen = MENU_SET_STOPTIME;        
          Menu.Change = 1;
        break;
        case UP_KEY:
          Menu.Screen = MENU_STARTTIME;        
          Menu.Change = 1;          
        break;
        case DOWN_KEY:
          Menu.Screen = MENU_TIME;        
          Menu.Change = 1;          
        break;                
        case NO_KEY:
            if(Menu.Change)
            {
              Menu.Change = 0;
              Menu_Show_Time(MENU_STOPTIME);
            }
        break;        
      }    
    break;

    case MENU_SET_STOPTIME:
      Menu_Set_Time(MENU_SET_STOPTIME);    
    break; 
*/  

/*
    case MENU_PACKINFO:
    
      lcdClear();
      lcdprintat(0,0,"S/N:");            
      lcdprint_dec_u16(PackInfo.SerialNo); 
      lcdprintat(14,0,"");            
      switch(PackInfo.PackType)
      {       
        case 0x01:
          lcdprint("ST");
        break;
        case  0x02:
          lcdprint("MB");
        break;
        default:
          lcdprint("??");
        break;
      }
                      
      lcdprintat(0,1,"R/C:");      
      lcdprint_dec_u16(PackInfo.RechargeNo);
      //lcdprint("  OT:");
      //lcdprint_dec_u16(PackInfo.OpTime_mins);
      
    break; 
*/    

     
  }





    
}


u08 Charging_Menu =0;
u08 chargingdots = 0;
//***************************************************
// void Menu_Show_Charging_VI(void)
// Show the Charging information on the LCD
//***************************************************
void Menu_Show_Charging_VI(u08 key)
{      

  wdt_reset();
  
  key = Kbd_Scan();
    
  if(key==UP_KEY)
  {
    if(++Charging_Menu > 2)
      Charging_Menu= 0;
  }
  if(key==DOWN_KEY)
  {
    if(Charging_Menu== 0)
      Charging_Menu= 2;
    else
      Charging_Menu--;
  }
  
    
    
  switch(Charging_Menu)
  {
    case 2:
      Comms_Get_PackTime();
      lcdClear();   
      lcdGotoXY(0,0);  
      switch(PackInfo.PackType)
      {       
        case 0x01:
          lcdprint("ST");
        break;
        case  0x02:
          lcdprint("MB");
        break;
        default:
          lcdprint("??");
        break;
      }
      
      Menu_Display_TimeDigits(4,0,PackInfo.StartTime.hours,PackInfo.StartTime.minutes);                
      lcdGotoXY(9,0);  
      lcdprint("->");
      Menu_Display_TimeDigits(11,0,PackInfo.StopTime.hours,PackInfo.StopTime.minutes);                        
      
            
      lcdprintat(0,1,"RC");      
      lcdprint_dec_u16(PackInfo.RechargeNo);
      Menu_Display_TimeDigits(8,1,PackInfo.Time.hours,PackInfo.Time.minutes); 
      lcdprint(":");      
      lcdprint_dec_u08(PackInfo.Time.seconds,1);

    break;
  
    case 1:
        lcdClear();
        //lcdHome();
        
        lcdprint_dec_u16(PackInfo.volta_mV); 
        lcdprint("mV ");
        
        lcdprint_dec_u08(PackInfo.Temperature_deg,1); 
        lcdprint("ß");
        
        lcdprint_dec_u16(PackInfo.currenta_mA);      
        lcdprint("mA");
  
        Menu_Display_TimeDigits(0,1,PackInfo.ChargeTime.hours,PackInfo.ChargeTime.minutes);

        lcdprint(" ");
        lcdprint_dec_u16(PackInfo.Temperature);      
        
        lcdprint(" ");
        lcdprint_dec_u08(OCR1A,0);	
    break;
    
    case 0:
        lcdClear();
        //lcdHome();
        lcdprint("Charging ");
        u08 i;
        for(i=0;i<chargingdots;i++)
          lcdprint(".");
        if(++chargingdots > 6)
          chargingdots =0;

        lcdprintat(0,1,"SN");            
        lcdprint_dec_u16(PackInfo.SerialNo);       
                
        Menu_Display_TimeDigits(7,1,PackInfo.ChargeTime.hours,PackInfo.ChargeTime.minutes);                
        
        lcdGotoXY(13,1);
        lcdprint_dec_u08(PackInfo.Temperature_deg,1); 
        lcdprint("ß");

         
        //Menu_Display_TimeDigits(11,1,hour,min);                
    break;
  }
  
}       
