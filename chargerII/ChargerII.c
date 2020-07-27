/*********************************** MODULE INFO ****************************
	
   Device       : ATMega48
   File name    : Charger.c
   Ver nr.      : 1.0
   Description  : Battery Charger with ATMega8
   Compiler     : GCC-AVR
   Author       : Jason van Aardt iTribe Design
 
   Change log   : 2004-05-15  Initial 
                : 2005-06-05  New Code Base
 
****************************************************************************/

#include "ChargerII.h"




/*
typedef struct 
{
  u08 Error;
  u16 initial_battery_voltage;
  
  u16 fast_charge_voltage_mV;
  u16 fast_charge_current_mA;
  u16 trickle_charge_volt_mV;
  u16 trickle_charge_current_mA;
      
  u16 battery_volt_max;
  u16 battery_volt_min;
  u16 charge_volt_max;
  u16 charge_volt_min;
  u16 charge_current_max;
  u16 charge_current_min;
  
  s16 pwmcount;     
}sCharging_info;
*/

typedef struct 
{
  s16 pwmcount;     
}sCharging_info;



enum{
  NO_ERROR, DISCONNECT_ERROR, TRICKLE_CURRENT_MAX_ERROR};

enum{
  GREEN_LED, YELLOW_LED,RED_LED,ALL_LED};
  
enum{
  IDLE,
  CALC_CHARGE_VOLTAGE,
  FAST,
  TRICKLE,
  COMPLETE,
  RESTART,
  ERROR
};

enum{
  VOLTAGE,
  CURRENT,
  BATTERYVOLTAGE
  };

//-----------------------------------------------------------------
// Global Declarations
//-----------------------------------------------------------------

u08 MasterTimePresent;
u08 VirginSlaveSetTime;
volatile sTime time;
volatile u08 ChargeState;
volatile sCharging_info charging_info;
volatile u08 Pwm_flag;
volatile u16 gFAST_CURRENT_MAX;
volatile u16 gFAST_VOLTAGE_TERMINATE;

void Waiter(u16 wt)
{
    SystemTimers.us_tick=0;
    while(SystemTimers.us_tick< wt)
    {
      asm volatile("nop\n\t");
      wdt_reset();
    }
}  


//****************************************
// Pwm_off() 
//****************************************
void Pwm_off(void)                         
{
  TCCR1B = 0x00;
}

//****************************************
// Pwm_on() 
//****************************************
void Pwm_on(void)                         
{
  TCCR1B = (1<< CS10);         // 
}



//****************************************
// Set_Pwm() 
//****************************************
void Set_Pwm(unsigned int data)                         
{
  OCR1A = data;
}

//****************************************
// Pwm_inc() 
//****************************************
void Pwm_inc(void)                         
{
  OCR1A++;
  if(OCR1A == 0)
  {
    OCR1A = 0xFF; // for 8 Bit
//    rprintf("+");
  }
}

//****************************************
// Pwm_dec() 
//****************************************
void Pwm_dec(void)                         
{
  if(OCR1A != 0)
    OCR1A--;
}


//****************************************
// Stop_Pwm() // Stops the PWM in off pos.
//****************************************
void Stop_Pwm(void)                         
{
    if (OCR1A == 1)
    {
      while(TCNT1 > 2);                     // Wait for PWM == 1
      while(TCNT1 < 2);                     // Wait for PWM == 0  
    }
    else
    {
      while(TCNT1 > OCR1A);                  // Wait for PWM == 1
      while(OCR1A > TCNT1);                  // Wait for PWM == 0  
    }
    Pwm_off();
}    
//****************************************
// Restart the PWM
//****************************************
/*
void Start_Pwm(void)                         
{
  Pwm_on();
}
*/


//****************************************
// Read A Stable A/D voltage
//****************************************
/*
void Stable_ADC(void)                     // loop until you have a stable value
{ 
  int V[4];
  unsigned char i;
  int Vmax, Vmin;
  
    //Loop until the ADC value is stable. (Vmax <= (Vmin+1))
    for (Vmax=10,Vmin= 0;Vmax <= (Vmin+1);)
    {
        V[3] = V[2];
        V[2] = V[1];
        V[1] = V[0];
        ADCSRA |= 0x40;                      // Start a new A/D conversion
        while (!(ADCSRA & (1<<ADIF)))        // wait until ADC is ready      
            ;
        V[0] = ADC;
        Vmin = V[0];                          // Vmin is the lower VOLTAGE
        Vmax = V[0];                          // Vmax is the higher VOLTAGE  
        //Save the max and min voltage
        for (i=0;i<=3;i++)
        { 
            if (V[i] > Vmax)
                Vmax=V[i];
            if (V[i] < Vmin)
              Vmin=V[i];  
        }
    }
}
*/

//****************************************
// Take Measurements 
//****************************************
/*
int Measure(unsigned char value)
{
  char i;
  int av;

  switch (value)
  {
    case BATTERYVOLTAGE: 
        Stop_Pwm();
        ADMUX = VBAT2;
        //Stable_ADC();
        break;   
    case VOLTAGE: 
        ADMUX = VBAT2;
        break;   
    case CURRENT: 
        ADMUX = IBAT2;
        break;
   }  
    //Calculate a average out of the next 8 A/D conversions
    for(av=0,i=8;i;--i)
    {
        ADCSRA |= 0x40;                      // start new A/D conversion
        while (!(ADCSRA & (1<<ADIF)))        // wait until ADC is ready
            ;      
        av = av+ADC;
    }
    av = av/8;
    CLRBIT(ADCSRA,ADIF);                     // clear ADC interrupt flag

  switch (value)
  {
    case BATTERYVOLTAGE: 
        if(av <= charging_info.battery_volt_min)
              charging_info.battery_volt_min = av;
        if (av > charging_info.battery_volt_max)
              charging_info.battery_volt_max = av;
    break;   

    case VOLTAGE: 
        if (av <= charging_info.charge_volt_min)
  		    charging_info.charge_volt_min = av;
    		if (av > charging_info.charge_volt_max)
	        charging_info.charge_volt_max = av;
    break;   
    
    case CURRENT:
        if (av <= charging_info.charge_current_min)
  		    charging_info.charge_current_min = av;
    		if (av > charging_info.charge_current_max)
	        charging_info.charge_current_max = av;
    break;       
  }  
    return(av);  
}
*/


int Measure(unsigned char value)
{
  char i;
  int av;

  wdt_reset();

  switch (value)
  {
    case VOLTAGE: 
        ADMUX = VBAT2;
        break;   
    case CURRENT: 
        ADMUX = IBAT2;
        break;
   }  
    //Calculate a average out of the next 8 A/D conversions
    for(av=0,i=8;i;--i)
    {
        ADCSRA |= 0x40;                      // start new A/D conversion
        while (!(ADCSRA & (1<<ADIF)))        // wait until ADC is ready
            ;      
        av = av+ADC;
    }
    av = av>>3;  // divide by 8
    CLRBIT(ADCSRA,ADIF);                     // clear ADC interrupt flag

    return(av);  
}



//*************************************************
// A Charging Error has occured
//*************************************************
void Error(void)
{
            Stop_Pwm();
            LED_OFF_ALL;            
}



//***************************************************
// Calc_IBatt
//***************************************************
unsigned int Calc_IBatt(unsigned int data)
{
  unsigned int z;
  
    z = (data*OPAMP_CURRENT_STEP)/10;
    return(z);
}

//***************************************************
// Calc_IBatt
//***************************************************
unsigned int Calc_VBatt(unsigned int data)
{
  unsigned int z;
  
    z = (data*OPAMP_VOLTAGE_STEP) + ((data*OPAMP_VOLTAGE_STEP_MINOR)/100) ;
    return(z);
}

//****************************************
// InitHW()  Initialize Hardware Registers
//****************************************
void InitHW(void)
{
  TCCR1A  = 0x81;         // define timer1 as 8-bit PWM  

  TIFR1 = (1<<TOV1);      // Counter1 overflow Flag
  TIMSK1   = (1<<TOIE1);  // enable counter1 overflow interrupt
  
  TCNT1   = 0x00;         // set counter1 to zero      
  TCCR1B  = 0x00;         // PWM disabLED

  PORTC   = 0x00;         // Prepare Port C as analog input
  DDRC    = 0x00;         // with no pull-ups
  
  PORTB   = 0xFF;         // Turn of Leds on Port B
  DDRB    = 0xFF;         // Set PORTB as output                    
 
  ADCSRA   = 0x96;         // Single A/D conversion, fCK/64 

  // Init the LED's pins
  LED_PORT_DDR = LED_PORT_DDR | (1<<LED_GREEN_PIN) |(1<<LED_YELLOW_PIN);
  
  i2cInit();
  uartInit();
  uartSetBaudRate(19200);
	rprintfInit(uartSendChar);  
    
  Set_Pwm(0);
  Pwm_on();    
  Set_Pwm(0);
  
  LED_OFF_ALL;
  //LED_ON_GREEN;
  
}

//**************************************************
// Read Settings from the internal EEPROM
// and initialize
//**************************************************

void Init_Settings(void)
{
  
  u08 virgin = EEPROM_INT_read(VIRGIN_ADDR);
  //if(virgin !=0x42)
  {
    EEPROM_INT_write(VIRGIN_ADDR,0x42);
    EEPROM_INT_write(STARTTIME_HOUR_ADDR,9); // 9:00am
    EEPROM_INT_write(STARTTIME_HOUR_ADDR+1,0); 
    
    EEPROM_INT_write(STOPTIME_HOUR_ADDR,18); // 18:00
    EEPROM_INT_write(STOPTIME_HOUR_ADDR+1,0);     
  }
}


  //**************************************************
  // u08 Check_Pack_Connected(void)
  // Determine if the MISO pin is being pulled low
  // Returns 1 : pack connected 0: no pack connected
  //**************************************************  
  u08 Check_Pack_Connected(void)
  {
    if(COMMS_SPI_MISO_READ == 0)
      return(1);
    else
      return(0);
  }
  

  //**************************************************
  // void Comms_SPI_Init(void)
  // Initialize the Port Pins
  //**************************************************
  void Comms_Master_SPI_Init(void)
  {
    // Make input and output pins
    SPI_PORT_DDR = SPI_PORT_DDR | (1<<SPI_SCK) | (1<<SPI_MOSI);
    SPI_PORT_DDR = SPI_PORT_DDR &~(1<<SPI_MISO);  
    // No Enable Pull Up on MISO
    COMMS_SPI_MISO_1;  
    // SCK low level
    COMMS_SPI_SCK_0;
    COMMS_SPI_MOSI_1;
   
  }
  
  
  //**************************************************
  // void Comms_WaitBitTime(void)
  // Waits a bit period
  //**************************************************
  void Comms_Master_WaitBitTime(void)
  {
      SystemTimers.us_tick=0;
      while(SystemTimers.us_tick< MASTER_WAITBITTIME)
      {
        asm volatile("nop\n\t");
        wdt_reset();
      }
  }
  
  void Comms_Master_WaitSetupTime(void)
  {
      SystemTimers.us_tick=0;
      while(SystemTimers.us_tick< MASTER_WAITSETUP)
      {
        asm volatile("nop\n\t");
        wdt_reset();        
      }
  }
    
  
  
  
  //**************************************************
  // void Comms_SendByte(u08 data)
  // Send a Byte to the Battery Pack
  //**************************************************
  void Comms_Master_WriteByte(u08 data)
  {
  u08 i;
      wdt_reset();

    // Clock line is low, and a rising edge latches in data
    for(i=0;i<8;i++)
    {
      u08 bit = data & 0x80;
      
      if(bit)
        COMMS_SPI_MOSI_1;
      else
        COMMS_SPI_MOSI_0;
      
      #ifdef COMMS_LED_DEBUG      
        if(bit)
          LED_ON_GREEN;    
        else
          LED_OFF_GREEN;        
      #endif
          
      data = data<<1;
      Comms_Master_WaitSetupTime();
      COMMS_SPI_SCK_1;
      #ifdef COMMS_LED_DEBUG      
        LED_ON_YELLOW;    
      #endif
      Comms_Master_WaitBitTime();
      
      COMMS_SPI_SCK_0;
      #ifdef COMMS_LED_DEBUG            
        LED_OFF_YELLOW;
      #endif
      Comms_Master_WaitBitTime();            
    }  
  }
  
  
  
  
  
  //**************************************************
  // void Comms_SendPkt(u08 *pkt, u08 len)
  // Sends a packet of information 
  //**************************************************
  void Comms_Master_SendPkt(u08 *pkt, u08 len)
  {
  u08 i;
    wdt_reset();

    for(i=0;i<len;i++)
      Comms_Master_WriteByte(*pkt++);
  }
  
  
  
  //**************************************************
  // void Comms_ReadByte(u08 data)
  // Send a Byte to the Battery Pack
  //**************************************************
  u08 Comms_Master_ReadByte(void)
  {
  u08 i,data,bit;
  
    wdt_reset();

    // Clock line is low, and a rising edge signals time for new data
    // data bit is sampled just before the SCK line is changed to low
    data = 0;
    for(i=0;i<8;i++)
    {
      COMMS_SPI_SCK_1;
      Comms_Master_WaitBitTime();
    
      bit = COMMS_SPI_MISO_READ;
      data = data <<1;    
      if(bit)    
        data |= 0x01;
      
     
  
      #ifdef COMMS_LED_DEBUG
        LED_ON_YELLOW;        
        if(bit)
          LED_ON_GREEN;    
        else
          LED_OFF_GREEN;        
      #endif
          
      
      COMMS_SPI_SCK_0;
      Comms_Master_WaitBitTime();            
      #ifdef COMMS_LED_DEBUG    
        LED_OFF_YELLOW;
      #endif
  
    }
    return(data);
  }
  
  
  
  //**************************************************
  // void Comms_SendPkt(u08 *pkt, u08 len)
  // Sends a packet of information 
  //**************************************************
  void Comms_Master_GetPkt(u08 *pkt, u08 len)
  {
  u08 i;
    wdt_reset();

  #ifdef DEBUG_GETPKT
    lcdClear();
    lcdprintat(0,0,"Bytes received :");             
    lcdGotoXY(0,1);	     
  #endif  
    
    for(i=0;i<len;i++)
    {
      *pkt = Comms_Master_ReadByte();
  #ifdef DEBUG_GETPKT    
      lcdprint_hex_u08(*pkt);
      lcdprint(" ");
  #endif        
      pkt++;
    }
  }
  
  
  //**************************************************
  // u08 Comms_Master_SendandReceive_CmdData(u08 *masterdata, u08 *slavedata)
  // Returns the command code byte returned by the slave
  //**************************************************
  u08 Comms_Master_SendandReceive_CmdData(u08 *masterdata, u08 *slavedata,u08 checkreturndata)
  {
  u08 master_command;
    wdt_reset();
  
    master_command = *masterdata;
    Comms_Master_WriteByte(0xFF);
    Comms_Master_WriteByte(0x00);
    Comms_Master_WriteByte(0x42);    
    u08 i;
    for(i=0;i< COMMS_DATA_BYTES;i++)
      Comms_Master_WriteByte(*masterdata++);    
  
    // Pause here for the slave data
    Waiter(0x3FFF);

    Comms_Master_GetPkt(slavedata,COMMS_DATA_BYTES);
    // Check if the Slave responded
    u08 returndata_error =0;
    if(*slavedata == ((master_command)+0x10))
    {
      // ONLY ADD IN ONCE WE CAN DO A PACK UPDATE
      if(checkreturndata)
      {
        for(i=0;i<COMMS_DATA_BYTES;i++)
          if(*slavedata != *masterdata)
             returndata_error = 1;
      }      
    }
    else
      returndata_error = 1;
      
    return(returndata_error);
  }  
   
  
  //**************************************************
  // u08 Comms_Send_Hello_Pack(void)
  // Determine if the pack is connected.
  //**************************************************
  u08 Comms_Send_Hello_Pack(void)
  {       
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
    
    // Check if the pack responds to a request
    masterdata[0] = CMD_GET_PACKINFO;
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    
    return(Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0));
  }

  //**************************************************
  //  u16 Comms_Get_PackTemperature(void)
  //  Get the Temperature of the battery pack
  //**************************************************
  void Comms_Get_PackTemperature(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
      
    masterdata[0] = CMD_GET_TEMPERATURE;    
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    u16 temp = (slavedata[1]<<8) + slavedata[2];


    #define MAX_TEMPERATURE_ERROR_READING 3
    // if the temperature is returned as 0 ,
    // then use previous value
    if(temp == 0)
    {
      //if(++PackInfo.Temperature_error> MAX_TEMPERATURE_ERROR_READING)  
      if( PackInfo.Temperature == 0)
         PackInfo.Temperature  = SENSING_NTC_25DEGREES_ADC;
    }  
    else
      PackInfo.Temperature = temp;

    PackInfo.Temperature_deg = 25 + ((SENSING_NTC_25DEGREES_ADC - PackInfo.Temperature)/TEMPERATURE_GRADIENT);    
    if(PackInfo.Temperature > SENSING_NTC_25DEGREES_ADC)
      PackInfo.Temperature_deg = 25;          
      
/*	 
	// Previous Attiny26 Led controller
    PackInfo.Temperature_deg = 25+ ((NTC_25degrees - PackInfo.Temperature) >>4);    
    if(PackInfo.Temperature > NTC_25degrees)
      PackInfo.Temperature_deg = 25;          
*/	  
  }

  //**************************************************
  // u16 Comms_Get_PackTime(void)
  //  Get the Temperature of the battery pack
  //**************************************************
  u08 Comms_Get_PackTime(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];

	Time_readtime((u08*)&Time.hours,(u08*)&Time.minutes,(u08*)&Time.seconds);  
    masterdata[0] = CMD_GET_TIME;    
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    PackInfo.Time.hours = slavedata[1];
    PackInfo.Time.minutes = slavedata[2];
    PackInfo.Time.seconds = slavedata[3];  
    return(0);
  }

  //**************************************************
  // u16 Comms_Set_PackTime(void)
  //  Set the Timeof the battery pack
  //**************************************************
  u08 Comms_Set_PackTime(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
      
    masterdata[0] = CMD_SET_TIME;   
    Time_readtime((u08*)&Time.hours,(u08*)&Time.minutes,(u08*)&Time.seconds);  
    masterdata[1] = Time.hours;    
    masterdata[2] = Time.minutes;    
    masterdata[3] = Time.seconds;    
      
    u08 timeset_correct_cnt =4;
    
    // Ensure the pack is set correctly    
    while(--timeset_correct_cnt > 0)    
    {
      if(Comms_Master_SendandReceive_CmdData(masterdata,slavedata,1) == 0)
      {  
          return(0);              
      }
    }
    
    return(1); // FAILED TO SET THE TIME CORRECTLY
    
  }


#ifdef SERIAL_NUMBERGEN
  //**************************************************
  // u16 Comms_Set_PackInfo(void)
  //  Set the Serial Number of the Pack
  //**************************************************
  u08 Comms_Set_PackInfo(u16 serialnum)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
      
    masterdata[0] = CMD_SET_PACKINFO;   
    masterdata[1] = PACKTYPE_ST;    
    masterdata[2] = (serialnum >> 8);    
    masterdata[3] = 0xFF & serialnum;    
      
    if(Comms_Master_SendandReceive_CmdData(masterdata,slavedata,1) == 0)
          return(0);                  
    return(1); // FAILED TO SET THE serial number CORRECTLY    
  }

  //**************************************************
  // void Serial_Number_Generatator(void)
  //  State machine for the serial number generator
  //**************************************************  
  void Serial_Number_Generator(void)
  {
	u16 SerialNo;
	// Provide voltage for very flat packs
	Set_Pwm(2);
	Pwm_on();
	
	while(1)
	{
		// Read the last programmed serial number from eeprom
		SerialNo = EEPROM_INT_read(LAST_SERIAL_ADDR) << 8;
		SerialNo |= EEPROM_INT_read(LAST_SERIAL_ADDR+1);
		SerialNo++;

		
	  u08 program = 0;
	  u08 refresh  = 1;
	  u08 editpos  = 1;
	  while(program == 0)
	  {
		if(refresh)
		{
			refresh = 0;
			lcdClear();
			lcdHome();			
			lcdprint("SerialNo Prog");
			lcdprintat(0,1,"SN ");            
			lcdprint_dec_u16(SerialNo);    
			lcdGotoXY(3 +(4 - editpos),1);
			// Blink
			lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_CURSOR);			
		}
		
		// Now check for changes to the keys
		switch(Kbd_Scan())
		{
			case UP_KEY:
				refresh = 1;
				switch(editpos)
				{
				 case 1: SerialNo++; break;
				 case 2: SerialNo+=10; break;
				 case 3: SerialNo+=100; break;
				 case 4: SerialNo+=1000; break;				 				 
				}
				delay(15000);				// Allow key to settle				
			break;
			case DOWN_KEY:			
				refresh = 1;			
				switch(editpos)
				{
				 case 1: SerialNo--; break;
				 case 2: SerialNo-=10; break;
				 case 3: SerialNo-=100; break;
				 case 4: SerialNo-=1000; break;				 				 
				}
				delay(15000);				// Allow key to settle				
			break;
			case LEFT_KEY:
				refresh = 1;			
				if(++editpos >4)
					editpos = 4;
			break;
			case RIGHT_KEY:
				refresh = 1;			
				if(--editpos <1)
					editpos = 1;
			break;
			
			
			case ENTER_KEY:
				program = 1;
				delay(15000);				// Allow key to settle
			break;
		}
	  }
		
        lcdClear();
        lcdprint("Programming");		
		Comms_Set_PackInfo(SerialNo);
		Comms_Set_PackInfo(SerialNo);
		//Comms_Set_PackInfo(SerialNo);		
		// Store the Serial Number
		EEPROM_INT_write(LAST_SERIAL_ADDR,(SerialNo >>8));
		EEPROM_INT_write(LAST_SERIAL_ADDR+1,(SerialNo&0xFF));		  
        lcdClear();				
        lcdprint("Next Pack");				
		while(Kbd_Scan() != ENTER_KEY);		
	}
  }
  
  
#endif



#ifdef READTIME_TEST
  //**************************************************
  // ReadTime_Test()
  //  Read and display the Time of a Pack
  //**************************************************  
  void ReadTime_Test(void)
  {
	// Provide voltage for very flat packs
	Set_Pwm(2);
	Pwm_on();
	
	while(1)
	{
		lcdClear();
		lcdHome();			
		lcdprint(" *PackTimeTest*");

		while(!Check_Pack_Connected())
		{
			wdt_reset();
		}
		
		{
			lcdClear();
			lcdprintat(2,0,"-Connected-");         
			if(Comms_Send_Hello_Pack() == 0)
			{ 
				lcdClear();
				lcdprintat(1,0,"-Reading Time-");     
				// Get All the Pack info
				if(Comms_Get_PackTime() == 0)
				{				
					u08 masterdata[COMMS_DATA_BYTES];
					u08 slavedata[COMMS_DATA_BYTES];  
					  
					masterdata[0] = CMD_GET_PACKINFO;
					Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
					PackInfo.PackType = slavedata[1];
					PackInfo.SerialNo = (slavedata[2]<<8) + slavedata[3];				
					Comms_Get_PackTime();      
					Comms_Get_PackStartStopTime();      	
					  lcdClear();      
					  lcdprintat(0,0,"SN");            
					  lcdprint_dec_u16(PackInfo.SerialNo);       
					  
					  Menu_Display_TimeDigits(8,0,PackInfo.Time.hours,PackInfo.Time.minutes);   
					  lcdprint(":");      
					  lcdprint_dec_u08(PackInfo.Time.seconds,1);
					  
					  Menu_Display_TimeDigits(1,1,PackInfo.StartTime.hours,PackInfo.StartTime.minutes);                
					  lcdGotoXY(7,1);  
					  lcdprint("->");
					  Menu_Display_TimeDigits(10,1,PackInfo.StopTime.hours,PackInfo.StopTime.minutes);       
					
//					Display_PackTimes();  				
					while(Kbd_Scan() != ENTER_KEY);							
				}			
			}
		}
	}   
  }
#endif



#ifdef FEEVERBEEPER_TEST
//**************************************************  
void FeeverBeeper_Test(void)
{	
	lcdClear();
	lcdHome();			
	lcdprint(" -FeeverBeeper-");
	while(1)
	{
		wdt_reset();
		if(RxSerial.MessageReady_flag)
		{
			lcdClear();
			lcdHome();			
			lcdprint(" -FeeverBeeper-");
			lcdprintat(1,1,"*");		
			RxSerial.MessageReady_flag = 0;
			u08 i;
			for(i=0;i<RxSerial.MessageSize;i++)
			{
				lcdprintChar(RxSerial.Message[i]);
			}
    }				
	}
}
#endif






  //**************************************************
  // u16 Comms_Get_PackTime(void)
  //  Get the Temperature of the battery pack
  //**************************************************
  u08 Comms_Get_PackStartStopTime(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
    masterdata[0] = CMD_GET_STARTSTOPTIME;    
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    PackInfo.StartTime.hours = slavedata[1];
    PackInfo.StartTime.minutes = slavedata[2];
    PackInfo.StopTime.hours = slavedata[3];
    PackInfo.StopTime.minutes = slavedata[4];         
     return(0);
  }


  //**************************************************
  // u16 Comms_Set_PackStartTime(void)
  //  Set the Start Time of the Pack
  //**************************************************
  u08 Comms_Set_PackStartStopTime(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];

    masterdata[0] = CMD_SET_STARTSTOPTIME;   
    masterdata[1] = EEPROM_INT_read(STARTTIME_HOUR_ADDR);    
    masterdata[2] = EEPROM_INT_read(STARTTIME_HOUR_ADDR+1);    
    masterdata[3] = EEPROM_INT_read(STOPTIME_HOUR_ADDR);    
    masterdata[4] = EEPROM_INT_read(STOPTIME_HOUR_ADDR+1);  
    
    u08 timeset_correct_cnt =4;    
    while((--timeset_correct_cnt > 0))
    {
      if(Comms_Master_SendandReceive_CmdData(masterdata,slavedata,1) == 0)
          return(0);
    }    
        
    return(1);
  }


  //**************************************************
  // u08 Comms_Get_PackStartTime(void)
  //  Set the Start Time of the Pack
  //**************************************************
  u08 Comms_Get_RechargeCount(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];  
    
    masterdata[0] = CMD_GET_RECHARGENO;    
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    PackInfo.RechargeNo = (slavedata[1]<<8) + slavedata[2];
    return(0);        
  }

  //**************************************************
  // u16 Comms_Set_PackStartTime(void)
  //  Set the Start Time of the Pack
  //**************************************************
  u08 Comms_Inc_RechargeCount(void)
  {
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];
      
    masterdata[0] = CMD_INC_RECHARGECOUNT;               
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);      
    return(0);    
  }

//**************************************************
// Read and Display the Pack Times
//**************************************************

void Display_PackTimes(void)
{
      lcdClear();      
      Menu_Display_TimeDigits(6,0,PackInfo.Time.hours,PackInfo.Time.minutes);   
      lcdprint(":");      
      lcdprint_dec_u08(PackInfo.Time.seconds,1);
	  
      Menu_Display_TimeDigits(1,1,PackInfo.StartTime.hours,PackInfo.StartTime.minutes);                
      lcdGotoXY(7,1);  
      lcdprint("->");
      Menu_Display_TimeDigits(10,1,PackInfo.StopTime.hours,PackInfo.StopTime.minutes);       
}    


  //**************************************************
  // 
  // 
  //**************************************************
  u08 Comms_Get_PackInfo(void)
  {
    //u08 Comms_Failure;
    u08 masterdata[COMMS_DATA_BYTES];
    u08 slavedata[COMMS_DATA_BYTES];  
	
  
    masterdata[0] = CMD_GET_PACKINFO;
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    PackInfo.PackType = slavedata[1];
    PackInfo.SerialNo = (slavedata[2]<<8) + slavedata[3];
        
/*        
    masterdata[0] = CMD_GET_RECHARGENO;    
    Comms_Master_SendandReceive_CmdData(masterdata,slavedata,0);
    PackInfo.RechargeNo = (slavedata[1]<<8) + slavedata[2];   
*/    
    
    Comms_Get_RechargeCount();
    Comms_Set_PackTime();      	
    Comms_Get_PackTime();      
    Comms_Get_PackStartStopTime();      	
    Display_PackTimes();  
	u08 cnt = 0;
	// Check that the times are correct.
	while(((PackInfo.StartTime.hours != 9) ||(PackInfo.StartTime.minutes != 0)
		|| (PackInfo.StopTime.hours != 18) || (PackInfo.StopTime.minutes != 0)
		|| (PackInfo.Time.hours != Time.hours) || (PackInfo.Time.minutes !=Time.minutes))
		&& (++cnt <3))
	{
		/*
		lcdClear();      
		lcdGotoXY(3,0);  		
		lcdprint("Pack Time");  	
		lcdGotoXY(3,1);  
		lcdprint("Incorrect");		
		while(Kbd_Scan() == NO_KEY);
		*/
		Comms_Set_PackStartStopTime();	
		Comms_Get_PackStartStopTime();  
		Comms_Set_PackTime();   
  	    Comms_Get_PackTime();      
	}
/*
    if(Comms_Set_PackTime())
    {
        lcdClear();
        lcdprintat(3,0,"*Not Set*");
        lcdprintat(6,1,"Time"); 
        //while(Kbd_Scan() == NO_KEY);  
    }    
    if(Comms_Set_PackStartStopTime())
    {
        lcdClear();
        lcdprintat(3,0,"*Not Set*");
        lcdprintat(1,1,"StartStop Time");
        //while(Kbd_Scan() == NO_KEY);          
    } 
    
     
    Comms_Get_PackStartStopTime();      
    //Display_PackTimes();    
*/	
       
    return(0);
  }


//**************************************************
// Sends the Time to the Slave units.
//**************************************************  
void MasterCharger_SendTime(void)
{
	Time_readtime((u08*)&Menu.hours,(u08*)&Menu.minutes,(u08*)&Menu.seconds);                              
	rprintfChar(Menu.hours);
	rprintfChar(Menu.minutes);
	rprintfChar(Menu.seconds);	
	rprintfChar(' ');
	rprintfChar(100);	
}  

//**************************************************
// Sends the Time to the Slave units.
//**************************************************  
u08 google;
void SlaveCharger_UpdateTime(void)
{
	// Using the message sent from the Master CHarger, 
	// update the RTC.
    //lcdprintat(0,0,"*****************");         
	
	u08 hours,minutes,seconds;
	hours = RxSerial.Message[0];
	minutes = RxSerial.Message[1];
	seconds = RxSerial.Message[2];
		
	Time_settime(hours,minutes,seconds);
	#ifdef SLAVE_COMMS	
	if(google)
	{
		google = 0;
		//lcdprintat(0,0,"$");         	
	}
	else
	{
		google = 1;
		//lcdprintat(0,0," ");         			
	}
	#endif
}  


//***************************************************
// void Constant_Current(u16 current,u16 maxvoltage)
//***************************************************
u08 Constant_Current(u16 maxcurrent,u16 maxvoltage,u16 maxtime, u08 maxtemperature)
{
  u16 current;
  u08 Consequetive_overtemp;
//  u08 key;
  
  LED_OFF_GREEN;
  LED_ON_YELLOW;  

  Set_Pwm(2);
  Pwm_on();
  charging_info.pwmcount = MAXPWMCOUNT/2;
  
  u16 count = 0;
  u08 ChargeComplete = 0;
  PackInfo.MaxChargeTime  = maxtime;
  
  u08 hours,minutes,seconds,oldminutes;
  u08 prevseconds;
  Time_readtime(&PackInfo.ChargeStart.hours,&PackInfo.ChargeStart.minutes,&seconds);                              
  PackInfo.ChargeTime_mins = 0;
  PackInfo.Previous_inc_current_mins = 0;
  //current = 200; // Start at 200mA and start incrementing
  current = maxcurrent;
  Consequetive_overtemp  = 0;
  oldminutes = 100;
  while(!ChargeComplete)
	{	      
      wdt_reset();
    
      PackInfo.volta = Measure(VOLTAGE);
      PackInfo.volta_mV = Calc_VBatt(PackInfo.volta);
      PackInfo.currenta = Measure(CURRENT);
      PackInfo.currenta_mA = Calc_IBatt(PackInfo.currenta);
      
      /*
      key = Kbd_Scan();
      if((key==UP_KEY) || (key==DOWN_KEY))
        Menu_Show_Charging_VI(key);
      */
       
      
    if(++count > 5000)	
	{
	  Menu_Show_Charging_VI(0);
      count = 0;    	
      Time_readtime(&hours,&minutes,&seconds); 
	  #ifdef MASTER_COMMS
           if(seconds != prevseconds)
		   {
			seconds = prevseconds;
			MasterCharger_SendTime();						
		   }
	  #endif
	  // Only once a minute
      if(minutes != oldminutes)
      {
		oldminutes = minutes;
        Comms_Get_PackTemperature();
		// Assume a wrap around at 12 midnight
		if(PackInfo.ChargeStart.hours > hours)
			PackInfo.ChargeTime_mins = (hours*60 + minutes) + (((23 - PackInfo.ChargeStart.hours)*60) + (60 - PackInfo.ChargeStart.minutes));		
		else		
			PackInfo.ChargeTime_mins = (hours*60 + minutes) - ((PackInfo.ChargeStart.hours*60) + (PackInfo.ChargeStart.minutes));
		
        PackInfo.ChargeTime.hours = PackInfo.ChargeTime_mins/60;  
        PackInfo.ChargeTime.minutes = PackInfo.ChargeTime_mins - (PackInfo.ChargeTime.hours*60);        
        
        //*******************************************************
        // EVERY N Minutes increase the current by MaxCurrent/10        
        //*******************************************************
        #define INCREASE_MINUTES 10
        if(PackInfo.ChargeTime_mins >= (PackInfo.Previous_inc_current_mins + INCREASE_MINUTES))
        {
          PackInfo.Previous_inc_current_mins = PackInfo.ChargeTime_mins;
          current = current + (maxcurrent/10);
          if(current > maxcurrent)
            current = maxcurrent;
        }
        
        
        // Check that we are not over temperature or Time NTC remember!
		#ifdef SLA_CHARGER		
		#else
        if(PackInfo.Temperature_deg >= maxtemperature)
        {
          #define NUM_CONSEQUETIVE_OVERTEMP 5
          if(++Consequetive_overtemp > NUM_CONSEQUETIVE_OVERTEMP)
            ChargeComplete = CHARGECOMPLETE_TEMPERATURE;          
        }
        else
          Consequetive_overtemp = 0;
		#endif
          
        if(PackInfo.ChargeTime_mins > maxtime)
        {        
          ChargeComplete = CHARGECOMPLETE_TIME;          
        }   
        if(PackInfo.currenta_mA < 50)
        {        
          ChargeComplete = CHARGECOMPLETE_PACKDISCONNECTED_CURRENT;          
        }   
        /*
        if(Check_Pack_Connected() == 0)
        {
          ChargeComplete = CHARGECOMPLETE_PACKDISCONNECTED_COMMS;                  
        }
        */     
      }
	}

      if ((PackInfo.volta_mV > maxvoltage)||(PackInfo.currenta_mA > (current)))
      {
        charging_info.pwmcount--;     
      }
      else
      {
       if((PackInfo.volta_mV < maxvoltage)||(PackInfo.currenta_mA < (current)))
       {
          charging_info.pwmcount++;
       }
      }

      
            
      //************************************
      // Set the desired direction for the
      // PWM modulation
      //************************************
      if((charging_info.pwmcount) >= MAXPWMCOUNT)
      {
       charging_info.pwmcount = MAXPWMCOUNT/2;
       Pwm_flag = 1;
      }           
      else     
      if((charging_info.pwmcount) <= 0)
      {
        Pwm_flag = 0;
        charging_info.pwmcount = MAXPWMCOUNT/2;      
      }    
      else
        Pwm_flag = 2;
    } 
    
  return(ChargeComplete); // Return the Code
}

extern u08 Charging_Menu;


  
//**************************************************
// The Main Entry Point into the program
//**************************************************
int main(void)
{
	wdt_disable();  		
  InitHW(); 
  LED_ON_GREEN;
  LED_ON_YELLOW;
  Init_Settings();  
  //sei();    

	lcdInit();
	lcdHome();
	//lcdClear();  
    
  Comms_Master_SPI_Init();
  LED_OFF_GREEN;
  LED_OFF_YELLOW;
	wdt_disable();  		

//	lcdprintat(2,0,"-Test-");         
//	lcdprintat(2,1,"-line2-");         
//	while(1);
	
  // REad the TIme from EEPROM
	#ifndef FEEVERBEEPER_TEST
		wdt_enable(WDTO_2S);  		
  #endif
    

        
  sei();
  //lcdprintat(1,0,"-COMMS-");   
  //lcdprint("$");  

	#ifdef SERIAL_NUMBERGEN
	Serial_Number_Generator();
   #endif

	#ifdef READTIME_TEST
	ReadTime_Test();
   #endif

	#ifdef FEEVERBEEPER_TEST
		FeeverBeeper_Test();
  #endif

  #ifdef SLAVE_COMMS	
	VirginSlaveSetTime = 1;
	//Time_settime(00,00,00);                              
	// Wait here for the time to be received from the master
	u08 reboottime_cnt = 0;
	u08 loopcnt = 0;
	while ((loopcnt <50) && (reboottime_cnt <3))
	{
	 if(RxSerial.MessageReady_flag)
	 {
		RxSerial.MessageReady_flag = 0;
		SlaveCharger_UpdateTime();
		reboottime_cnt++;		
     }		
	 // Wait for the Master here
	 Wait(500);
	 loopcnt++;
	}
	// There is no Master in the System, 
	// go to manual time setting mode
	if(reboottime_cnt <3)
	{
		VirginSlaveSetTime = 1;
		MasterTimePresent = 0;
	}
	else
	{
		VirginSlaveSetTime = 0;
		MasterTimePresent = 1;
	}

  #endif

  #ifdef MASTER_COMMS
    u08 i;
	for(i =0;i<42;i++)
	{
		MasterCharger_SendTime();
		Wait(500);
	}
  #endif 
  
  while(1)
  {   
    wdt_reset();
	
	// To just provide some voltage for very flat packs
	Set_Pwm(2);
	Pwm_on();	
	
    Comms_Master_SPI_Init(); 
	#ifdef SLAVE_COMMS
	 if(RxSerial.MessageReady_flag)
	 {
		RxSerial.MessageReady_flag = 0;
		SlaveCharger_UpdateTime();
     }		
	#endif
	
	#ifdef SLA_CHARGER
		Constant_Current(450,7614,840,41); // 300mA 7.4V Upper voltage limit                                
	#else
		
    if(Check_Pack_Connected())
    {
        lcdClear();
        lcdprintat(2,0,"-Connected-");         
        if(Comms_Send_Hello_Pack() == 0)
        { 
          lcdClear();
          lcdprintat(0,0,"-Communicating-");     
         // Get All the Pack info
         if(Comms_Get_PackInfo() == 0)
         {
            u08 complete = 0;
            //Comms_Update_PackInfo_Time();
            //Menu_Change(MENU_PACKINFO);
            //Menu_Show(); 
            Charging_Menu = 0;
            //while(Kbd_Scan() == NO_KEY);  
            switch(PackInfo.PackType)
            {
              case PACKTYPE_ST:
                complete = Constant_Current(900,7614,720,41); // 300mA 7.4V Upper voltage limit                                
              break;

              case PACKTYPE_MB:
                complete = Constant_Current(1500,7614 ,720,41); // 300mA per Pack 7.4V Upper voltage limit                  
              break;     
            
              default :
                complete = UNKNOWN_PACKTYPE;
              break;
            }
                
              Set_Pwm(0);                
              LED_OFF_GREEN;
              LED_OFF_YELLOW;          
              lcdClear();
              switch(complete)
              {
                case UNKNOWN_PACKTYPE:
                  lcdprintat(2,0,"Unknown Pack"); 
                break;
                case CHARGECOMPLETE_TEMPERATURE:
                  LED_ON_GREEN;                
                  lcdprintat(1,0,"Fully Charged"); 
                  Comms_Inc_RechargeCount();
                break;
                case CHARGECOMPLETE_TIME:
                  //lcdprintat(1,0,"Fully Charged"); 
                  Comms_Inc_RechargeCount();                  
                break;
                case CHARGECOMPLETE_PACKDISCONNECTED_CURRENT:
                  lcdprintat(0,0,"Pack Unplugged");                 
                 break;
                //case CHARGECOMPLETE_PACKDISCONNECTED_COMMS:                
                  //lcdprintat(0,0,"Pack Unplugged C"); 
                //break;  
                case CHARGECOMPLETE_COMMS_FAILURE:
                  lcdprintat(1,0,"*Comms Failure*"); 
                break;                  
                              
              }
            Comms_Get_RechargeCount();
            lcdprintat(0,1,"RC");                  
            lcdprint_dec_u16(PackInfo.RechargeNo);
              
            Menu_Display_TimeDigits(7,1,PackInfo.ChargeTime.hours,PackInfo.ChargeTime.minutes);              
            // Battery Pack Temperature
                  
            lcdGotoXY(13,1);
            lcdprint_dec_u08(PackInfo.Temperature_deg,1); 
            lcdprint("ß");                  // Degree Sign
            // Charge Time
            
            
            // Wait for the Pack to be UnPLugged
            while(Kbd_Scan() == NO_KEY);
            LED_OFF_GREEN;           
			RxSerial.MessageReady_flag = 0; // CLear any Time messages that were waiting
            Menu_Change(MENU_TIME);            
          }
        }  
    }
	#endif // SLA_CdHARGER
    //Menu_Change(MENU_TIME);
	#ifdef SLAVE_COMMS		
		if((MasterTimePresent == 0) && (VirginSlaveSetTime == 1))
		{
			Menu_Change(MENU_SET_TIME);
			VirginSlaveSetTime = 0;
		}
		else
			Menu_Change(MENU_TIME);
	#endif
	
    Menu_Show();
    Waiter(0x1FF);
  }
 }


//***************************************************
// Timer1 Interrupt Overflow vector
//***************************************************
SIGNAL(SIG_OVERFLOW1)
{  
  cli();
    
  if(Pwm_flag == 1)
  {
    Pwm_inc();
    //Led_on(YELLOW_LED);
   //Led_off(RED_LED);          
  }
  else {
    if (Pwm_flag == 0)
    {
      Pwm_dec();    
      //Led_on(RED_LED);
      //Led_off(YELLOW_LED);      
    }
    else
    {
      //Led_off(YELLOW_LED);      
      //Led_off(RED_LED);          
    }
   }

  Pwm_flag = 2;
   
  SystemTimers.us_tick++;
   
  sei();
}

// ************************[ End of BC.C ]*******************************






 

