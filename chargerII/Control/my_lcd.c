
//**************************************************
// LCD_Wr(unsigned char data)
//**************************************************

// PB4
void LCD_RS(unsigned char data)
{
  if(data)
    PORTB = PORTB |(1<<4);
  else
    PORTB = PORTB & 0xEF;    
}

// PD3
void LCD_ENABLE(unsigned char data)
{
  if(data)
  {
    PORTD = PORTD |(1<<3);
//    Led_on();
  }
  else
  {
    PORTD = PORTD & 0xF7;    
//    Led_off();    
  }
}

//PB5
void LCD_RW(unsigned char data)
{
  if(data)
    PORTB = PORTB |(1 <<5);
  else
    PORTB = PORTB & 0xDF;    
}

void LCD_DELAY(void)
{
/*
  unsigned int i;
 
  for(i=0;i<10;i++)
  {
  }
*/  
}

void LCD_DATA(unsigned char data)
{
  PORTD= (PORTD & 0x0F) | ((data & 0x0F)<<4);
}

void LCD_Wr(unsigned char data)
{
  LCD_RW(0);
  LCD_ENABLE(0);
  LCD_DATA(data>>4);
  LCD_DELAY();
  LCD_ENABLE(1);
  LCD_DELAY();
  LCD_ENABLE(0);
  
  LCD_DATA(0x0F &data);
  LCD_DELAY();
  LCD_ENABLE(1);
  LCD_DELAY();  
  LCD_ENABLE(0);
}

void LCD_WrData(unsigned char data)
{
  LCD_RS(1);
  LCD_Wr(data);    
}

void LCD_WrCmd(unsigned char data)
{  
  LCD_RS(0);
  LCD_Wr(data);
}

void LCD_ClrScr(void)
{
  LCD_WrCmd(0x02);
  LCD_WrCmd(0x01);
}

void LCD_Addr(unsigned char data)
{
  LCD_WrCmd(0x80 | (data & 0x7F));
}

void LCD_Goto(unsigned char y, unsigned char x)
{
  LCD_Addr(y*LCD_SCREEN_WIDTH + x);
}

//****************************************
// LCD_Put_char
//****************************************
LCD_Put_char(unsigned char data)
{
  LCD_WrData(data);
}

LCD_print(unsigned char *string)
{
  while(*(string) != 0x00)
  {
    LCD_Put_char(*string++);    
  }  
}


//****************************************
// Print decimal value of int
//****************************************
void LCD_print_int(int uart_data)
{
    int temp,middle;
    temp = uart_data;
    middle = (temp/1000);                   // first digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*1000));
    middle = (temp/100);                    // second digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*100));
    middle = (temp/10);                     // third digit of int uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*10));
    middle = temp;                          
    LCD_Put_char(middle + 0x30);                    // fourth digit of int uart_data
}

//****************************************
// print_char() prints the decimal value of 
//****************************************
void LCD_print_char(u08 uart_data)
{
    u08 temp,middle;
    temp = uart_data;
    middle = (temp/10);                     // first digit of char uart_data
    LCD_Put_char(middle + 0x30);
    temp = (temp-(middle*10));
    middle = temp;                          // second digit of char uart_data
    LCD_Put_char(middle + 0x30);
}


void LCD_Init(void)
{
unsigned int i;

  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(0);
  for (i=0;i<100;i++)
    LCD_DELAY();
  
  // Select 4 bit mode
  LCD_RS(0);
  LCD_RW(0);
  LCD_ENABLE(1);
  LCD_DATA(0x02);
  LCD_DELAY();
  LCD_ENABLE(0);
  LCD_DELAY();

  LCD_WrCmd(0x0D);  // Display on No Underline  
  LCD_WrCmd(0x06);  //Character Entry Mode Increment/ Display shift off
  LCD_WrCmd(0x2C);  // Function Set 4 bit/2 line mode 5x7 dot format
//  LCD_WrCmd(0x20);  // Function Set 4 bit/2 line mode 5x7 dot format

//  LCD_WrCmd(0x10);
  LCD_ClrScr();
}