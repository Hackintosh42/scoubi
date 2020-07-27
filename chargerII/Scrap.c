
/*

  while(1)
  {
	  wdt_reset();
    //Menu_Show_Time();
    u08 key = Kbd_Scan();
    if ( key != NO_KEY)
    {
      switch(key)
      {
        case LEFT_KEY:
          lcdprintat(4,0,"LEFT_KEY");      
          rprintf("LEFT_KEY\r\n");          
        break;
        case RIGHT_KEY:
          lcdprintat(4,0,"RIGHT_KEY");      
          rprintf("RIGHT_KEY\r\n");                    
        break;
        case UP_KEY:
          lcdprintat(5,0,"UP_KEY");      
          rprintf("UP_KEY\r\n");                    
        break;
        case DOWN_KEY:
          lcdprintat(4,0,"DOWN_KEY");     
           rprintf("DOWN_KEY\r\n");          
        break;
        
        case ENTER_KEY:
          lcdprintat(4,0,"ENTER_KEY");      
          rprintf("ENTER_KEY\r\n");                    
        break;             
      }
    }
  }
*/
//***************************************************
// Constant_Voltage(u16)
//***************************************************
/*
void Constant_Voltage(u16 voltage)
{
  Set_Pwm(128);
  Start_Pwm();
  charging_info.pwmcount = MAXPWMCOUNT/2;
  
  u16 count = 0;

  while(1)
	{	            
    charging_info.volta = Measure(VOLTAGE);
    charging_info.volta_mV = Calc_VBatt(charging_info.volta);
    charging_info.currenta = Measure(CURRENT);
    charging_info.currenta_mA = Calc_IBatt(charging_info.currenta);

    if(++count == 10000)
    {
      count = 0;
      //vt100SetCursorPos(5,5);
      rprintf("Volta = ");
      rprintfu16(charging_info.volta_mV);      
      rprintf("mV   ");
      rprintfu16(charging_info.volta);
	  
      //vt100SetCursorPos(6,5);
      rprintf("Currenta = ");
      rprintfu16(charging_info.currenta_mA);      
      rprintf("mA   ");
      rprintfu16(charging_info.currenta);

      //vt100SetCursorPos(7,5);	  
      rprintf("OCR1A = ");
      rprintfu08(OCR1A);	  
      
      //rprintf(" \r\n");
      //uartSendTxBuffer();	  
    }

    //*****************************************************
    // What must the Pulse width do
    //*****************************************************
    if (charging_info.volta_mV < (voltage))    
      charging_info.pwmcount++;
    else
    {
     if (charging_info.volta_mV > (voltage))  
      charging_info.pwmcount--;     
    }     

   
    if((charging_info.pwmcount) >= MAXPWMCOUNT)
    {
       charging_info.pwmcount = MAXPWMCOUNT/2;
        Pwm_flag = 1;
		//Pwm_inc();		
    }           
    else     
      if((charging_info.pwmcount) <= 0)
      {
        Pwm_flag = 0;
        charging_info.pwmcount = MAXPWMCOUNT/2;      
		//Pwm_dec();
      }    
  } 
}

 
    ptr[0] =ptr[1] = ptr[2]= ptr[3]= 0;
    
    // Send the Preamble
    Comms_Master_WriteByte(0xFF);
    Comms_Master_WriteByte(0x00);
    Comms_Master_WriteByte(0x42);    
    
    Comms_Master_WriteByte(CMD_GET_SERIALNO);
    Comms_Master_WriteByte(0x00);    
    Comms_Master_WriteByte(0x00);    
    Comms_Master_WriteByte(0x00);    
    Comms_Master_WriteByte(0x00);    
    
    Comms_Master_GetPkt(ptr,5);

    lcdClear();
    lcdprintat(0,0,"Serial No:");             
    lcdGotoXY(0,1);	 
    //lcdprint_dec_u16(PackInfo.SerialNo);
    lcdprint_hex_u08(ptr[0]);
    lcdprint_hex_u08(ptr[1]);
    lcdprint_hex_u08(ptr[2]);
    lcdprint_hex_u08(ptr[3]);
    
  
  
    // Get the Recharge Count number
    Comms_SendCmd(CMD_GET_RECHARGENO);
    Comms_GetPkt(ptr,2)
    PackInfo.RechargeNo = (ptr[0] << 8) + ptr[1];
  
    // Get the Temperature
    Comms_SendCmd(CMD_GET_TEMPERATURE);
    Comms_GetPkt(ptr,2)
    PackInfo.Temperature = (ptr[0] << 8) + ptr[1];
    
    // Get the Operational Time in minutes of the Pack
    Comms_SendCmd(CMD_GET_OPTIME_MINS);
    Comms_GetPkt(ptr,2)
    PackInfo.OpTime_mins = (ptr[0] << 8) + ptr[1];  

*/