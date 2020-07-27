#include <pic.h>
#include "roboto.h"

const u8 READY_msg[]="ROBOTO READY";
const u8 OK_HOME_msg[]="OK_HOME-";
const u8 OK_CD_msg[]="OK_CD-";
const u8 E_HOME_msg[]="E_HOME-";
const u8 E_MAX_msg[]="E_MAX-";
const u8 E_MIN_msg[]="E_MIN-";


// Global variable declaration
bank2 struct sMotor motor[NUMBEROFMOTORS];
bank3 struct sCmdQ gCmdQ[NUMBEROFMOTORS];
bank3 struct sSwtimer gSwtimer[2];
bank3 struct sWorld gWorld;
bank3 struct sDup dup;

struct sCharQ RXQ;   // The serial character Q
int i;
u8 gMotorselected =0;
static bit gStepmotors =0;
static bit gStopmotors =0;
u8 gHalfstep = 1;
u8 gTerminal = 1;
u8 printflag =0;
u8 gHwtimer0_cnt =0;
u8 gHomeswitch[3];
u8 gLedcnt;
//--------------------------------------------
// Function Prototypes
//--------------------------------------------

void Init_world(void);
void nextcoil(u8 i,u8 dir);
u8 Q_read(u8 n, struct sCmd *cmd);
void Q_write(u8 n, struct sCmd *cmd);
u8	Q_checkempty(u8 n);

char read_RXQ(void);
char readnoinc_RXQ(void);
void handle_RX(void);
void handle_TMR0();
void handle_steppermotors(void);
void handle_led(void);
void Init(void);
void Init_comms(void);
void Init_hwtimers(void);
void Init_swtimers(void);

void printc(u8 c);
void prints(const u8 *c);
void printnum(int n);
void printnum_4d(int n);
u8 RXQ_empty(void);
u8 checkforbytes_RXQ(u8 cnt);
void WDJ_protocol_handler(void);
void handle_homeswitches(void);

void duplicator_statemachine(void);


void error_max(u8 i);
void error_min(u8 i);
void error_home(u8 i);
void error_cd(void);
void ok_home(u8 i);
void ok_cd(void);





//--------------------------------------------
// Returns a pointer to the next structure
// in the ring buffer Q
//--------------------------------------------
u8 Q_read(u8 n, struct sCmd *cmd)
{
	if (gCmdQ[n].rdptr == gCmdQ[n].wrptr)
	{	
		return(NULL);	
	}
	else	
	{
		if ((++gCmdQ[n].rdptr) > (CMDQ_SIZE)-1)
			gCmdQ[n].rdptr = 0;
		*cmd = gCmdQ[n].ringbuf[gCmdQ[n].rdptr];				
		return(1);
	}
}

//--------------------------------------------
// Writes to the ring buffer Q
//--------------------------------------------
void Q_write(u8 n, struct sCmd *cmd)
{
    if(++gCmdQ[n].wrptr > CMDQ_SIZE-1) 
    	gCmdQ[n].wrptr = 0;

    gCmdQ[n].ringbuf[gCmdQ[n].wrptr] =*cmd;	
}

//--------------------------------------------
// Checks if the ring buffer Q is empty
//--------------------------------------------
u8	Q_checkempty(u8 n)
{
	if(gCmdQ[n].rdptr == gCmdQ[n].wrptr)
		return(1);
	else
		return(0);
}	


//--------------------------------------------
// Returns the Character and increments 
// the read ptr
//--------------------------------------------
u8 read_RXQ(void)
{
	if (RXQ.rdptr == RXQ.wrptr)
		return(NULL);	
	else	
	{
		if ((++RXQ.rdptr) > RXQSIZE-1)
			RXQ.rdptr = 0;
		return(RXQ.buf[RXQ.rdptr]);
	}
}

//--------------------------------------------
// Returns the Character but doesn't increment 
// the read ptr
//--------------------------------------------
char readnoinc_RXQ(void)
{
u8 tmpptr;

	if (RXQ.rdptr == RXQ.wrptr)
		return(NULL);	
	else	
	{
		tmpptr = RXQ.rdptr;	
		if ((++tmpptr) > RXQSIZE-1)
			tmpptr = 0;
		return(RXQ.buf[tmpptr]);
	}
}


//-------------------------------------------
// Interrupt Handling Routines
//-------------------------------------------

void handle_RX(void)
{
	// THis is a ring buffer, check for wraparound
    if(++RXQ.wrptr > RXQSIZE-1) 
    	RXQ.wrptr = 0;
    	
    RXQ.buf[RXQ.wrptr]=RCREG;
}

void handle_TMR0()
{
  	if(++gHwtimer0_cnt == 10)
  	{	
  		gHwtimer0_cnt = 0;
  		// switch the gripper motor off
  		if(gSwtimer[0].enable)
	  		if(++gSwtimer[0].cnt > gSwtimer[0].cmp)
	  		{  		
		  		GRIPPER = 0;  		
		  		gSwtimer[0].enable = 0;
		  	}
  		if(gSwtimer[1].enable)
	  		if(++gSwtimer[1].cnt > gSwtimer[1].cmp)
	  		{  		
				gSwtimer[1].cnt = 0;
	  			gStepmotors = 1;
	  		}		  	
  	}
}
void handle_TMR2()
{
}



static void interrupt isr(void)
{
  
  if(RBIF)      // There was a PortB change interrupt
  {
    RBIF=0;     // Clear the interrupt flag
    RBIE=1;     // Enable interrupt again
  }

  if(INTF)      // PortB external interrupt pin
  {
    INTF=0;
  }

  if(T0IF)      // Timer 0 Interrupt Flag
  {
    handle_TMR0();
    T0IF=0;     // Clear the interrupt flag
  }
  if(TMR2IF)      // Timer 2 Interrupt Flag
  {
    handle_TMR2();
    TMR2IF=0;     // Clear the interrupt flag
  }
    
  if(RCIF)		// Recieve interrupt flag
  {
    RCIE=0; // Disable Receive Interrupt
    handle_RX();
	RCIE=1; // Enable Receive Interrupt    
  }
}

//--------------------------------------------
// void Init(void)
//   Initialize the CPU and development board
//--------------------------------------------
void Init(void)
{
  // Disable the dog when programming

  //  (1 is input, 0 is output)
  TRISA=0b00001111;  //  (ONLY 6 PINS)
  TRISB=0b11111111;
  TRISC=0b10100000;
  TRISD=0b00000000;  // motors are connected here
  TRISE=0b00000000;

  PORTD = 0x0;
  RBPU=1;    // 0 = Enable PORTB pullups
  ADCON1=0x07;   // NO a/d'S
  RBIF=0;    // Clear Port B interrupt flag
  T0IF=0;    // Clear timer0 interrupt flag  
  T0IE=0;    // Timer interrupt disable
  RBIE=1;    // RB Port Change interrupt enable   
  
  // And Finally
  GIE=1;        // Enable global interrupts
  LED0=1;		// Show that board is initialized
}

//--------------------------------------------
// void Init_comms(void)
//   Initialize the RS232 Serial Comms
//--------------------------------------------
void Init_comms(void)
{
	RXQ.rdptr = 0;
	RXQ.wrptr = 0;
	RXQ.forceread = 0;
	RXQ.waiting = 0;
	RXQ.timeout = 0;
	
	BRGH=1; // High BaudRate
	SPBRG=SERIAL_SPEED;
	SPEN=1; // Serial Port Enable
	TXEN=1; // Enable TX
  	SYNC = 0;  // Set to async port
 	CREN = 1;  // Continuous RX enable

    TXIE  = 0;  // Disable Transmit Interrupt
	RCIE  = 1;  // Enable Receive  Interrupt
  	PEIE  = 1;  // Enable peripheral interrupts
}


//--------------------------------------------
// void Init_swtimers(void)
//   Initialize the timers driven off hw TM0
//--------------------------------------------
void Init_swtimers(void)
{
  //Software Timer0 for the gripper
  gSwtimer[0].cmp = 0x4;
  gSwtimer[0].cnt = 0;
  gSwtimer[0].enable = FALSE;

  // Software Timer1 for the OS
  gSwtimer[1].cmp = 1;
  gSwtimer[1].cnt = 0;
  gSwtimer[1].enable = TRUE;  
}

//--------------------------------------------
// void Init_hwtimers(void)
//   Initialize the timers
//--------------------------------------------
void Init_hwtimers(void)
{
  //Hardware Timer0
  T0CS=0;  // Select timer mode
  T0IE=1; // Enable the interrupt

/*  Hardware Timer2
   Set the postscaler to 16
   Set the prescaller to 1 
*/  
  T2CON = (0x0f<<3) & (1<<2);
  TMR2IE=0; // Disable the interrupt
}


//--------------------------------------------
// Print a character to the Rs232 port
//--------------------------------------------
void printc(u8 c)
{
  // Wait until Uart is ready.
  while(TXIF==0) NOP;  
  TXREG=c;  
}

//--------------------------------------------
// Print a string to the Rs232 port
//--------------------------------------------
void prints(const u8 c[])
{
u8 i;
	i=0;
	while(c[i] != NULL)
	{
		printc(c[i]);
		i++;
	}
}

//--------------------------------------------
// Void newline(void)
//--------------------------------------------
void newline(void)
{
	if(gTerminal)
	{
		printc(0x0a);printc(0x0d);
	}
}


//--------------------------------------------
// Print an integer to the Rs232 port
//--------------------------------------------
void printnum(int n)
{
u8 tmp;
	tmp = 0;
	while(n>=100)
	{
		n = n -100;
		tmp +=1;
	}
	printc('0'+tmp);
	tmp = 0;;
	while(n>=10)
	{
		n = n -10;
		tmp +=1;
	}
	printc('0'+tmp);
	printc('0'+n);
}

//--------------------------------------------
// Print an integer to the Rs232 port
//--------------------------------------------
void printnum_4d(int n)
{
u8 tmp;
	tmp = 0;
	if(n<0)
	{
		printc('-');
		n=~((unsigned int)n);
	}
	while(n>=1000)
	{
		n = n -1000;
		tmp +=1;
	}	
	printc('0'+tmp);	
	while(n>=100)
	{
		n = n -100;
		tmp +=1;
	}
	printc('0'+tmp);
	tmp = 0;;
	while(n>=10)
	{
		n = n -10;
		tmp +=1;
	}
	printc('0'+tmp);
	printc('0'+n);
}


//--------------------------------------------
// Check if the receive queue is empty
//--------------------------------------------
u8 RXQ_empty(void)
{
	if(RXQ.rdptr == RXQ.wrptr)
		return(1);
	else
		return(0);
}

//--------------------------------------------
// Returns TRUE if the receive queue has cnt byte
// available for reading
//--------------------------------------------
u8 checkforbytes_RXQ(u8 cnt)
{
	if (RXQ.wrptr >= RXQ.rdptr)
	{
		if((RXQ.wrptr - RXQ.rdptr) >= cnt)
			return(1);
	}
	else
	{
		if (((RXQSIZE - RXQ.rdptr) + RXQ.wrptr) >= cnt)
			return(1);
	}	
	return(0);
}

//--------------------------------------------
// void WDJ_protocolstack(void)
//   William Danie Jason Roboto Protocol!
//--------------------------------------------
void WDJ_protocol_handler(void)
{
u8 data[4];
static struct sCmd tmpcmd;

  	if (!checkforbytes_RXQ(RXQ.waiting))
  	{  	
/*  	
  		if(++RXQ.timeout > RX_TIMEOUT)
	  	{
	  		printc('!');
	  		RXQ.timeout = 0;
	  		RXQ.forceread = 1;
	  		RXQ.waiting = 0;
  		}		
*/  		
	}
	else
	{	
		RXQ.waiting =0;
		RXQ.timeout =0;
				
		// Try regain sync by moving onto the next character
		if(RXQ.forceread)
		{	
			RXQ.forceread = 0;
			read_RXQ();
		}
					
		switch(readnoinc_RXQ())
			{			
				// repeat the last motor command
				case 'a':
					read_RXQ();
					printc('a');
					Q_write(gMotorselected,&tmpcmd);
				break;
					
				// Command : Select motor;				
				case 'm':				
					RXQ.waiting = 2;
					if(checkforbytes_RXQ(RXQ.waiting))
					{
						read_RXQ();
						data[0] = read_RXQ();
						printc('m');
						switch(data[0])
						{
							case 'h':
								gMotorselected =0;
								printc('h');
							break;
							case 'v':
								gMotorselected = 1;
								printc('v');
							break;
							case 'g':
								gMotorselected = 2;
								printc('g');
							break;
							default: 
								gMotorselected = 0xff;
								printc('!');
							break;
						}
							
						RXQ.waiting = 0;					
					}
				break;				

				// Command : Home
				case 'c':
					RXQ.waiting = 2;
					if(checkforbytes_RXQ(RXQ.waiting))
					{	
						read_RXQ();	
						printc('c');										
						if (read_RXQ() == 'd')
						{
							if (gMotorselected == 1)
							{
								// Set the maximum number of steps so the motor doesn't burn out if stuck
								tmpcmd.dir = CD; tmpcmd.steps = 0;
								Q_write(gMotorselected,&tmpcmd);							
								printc('d');
							}
							else printc('!');
						}
						else
						{
							printc('!');							
						}
						RXQ.waiting = 0;
					}
				break;				



				// Command : Home
				case 'h':
					RXQ.waiting = 4;
					if(checkforbytes_RXQ(RXQ.waiting))
					{	
						read_RXQ();	
						printc('h');										
						if ((read_RXQ() == 'o') && (read_RXQ() == 'm') )
							if (read_RXQ() =='f') 
							{
								// this is a Forced home
								tmpcmd.dir = FHOME; tmpcmd.steps = MOTORMAXPOSITION[gMotorselected];								
								Q_write(gMotorselected,&tmpcmd);							
								printc('o');printc('m');printc('f');
							}
							else
							{
								// Soft home
								tmpcmd.dir = HOME; tmpcmd.steps = 0;								
								Q_write(gMotorselected,&tmpcmd);							
								printc('o');printc('m');printc('e');
							}						
						else
						{
							read_RXQ();
							printc('!');printc('!');printc('!');
						}
						RXQ.waiting = 0;
					}
				break;				
				// goto
				case 'g':
					RXQ.waiting = 4;
					if(checkforbytes_RXQ(RXQ.waiting))
					{
						read_RXQ();
						tmpcmd.dir = MOVETO;						
						tmpcmd.steps = (read_RXQ()- '0')*100 + (read_RXQ()-'0')*10 + (read_RXQ()-'0');
						Q_write(gMotorselected,&tmpcmd);

						printc('g'); 
						printnum(tmpcmd.steps);
						RXQ.waiting = 0;
					}
				break;				


				
				// Command : Forward
				case 'f':
					RXQ.waiting = 4;
					if(checkforbytes_RXQ(RXQ.waiting))
					{
						read_RXQ();
						tmpcmd.dir = FWD;						
						tmpcmd.steps = (read_RXQ()- '0')*100 + (read_RXQ()-'0')*10 + (read_RXQ()-'0');
						Q_write(gMotorselected,&tmpcmd);

						printc('f'); 
						printnum(tmpcmd.steps);
						RXQ.waiting = 0;
					}
				break;				
				case 'r':
					RXQ.waiting = 4;
					if(checkforbytes_RXQ(RXQ.waiting))
					{
						read_RXQ();
						tmpcmd.dir = REV;
						tmpcmd.steps = (read_RXQ()- '0')*100 + (read_RXQ()-'0')*10 + (read_RXQ()-'0');
						Q_write(gMotorselected,&tmpcmd);
						printc('r'); printnum(tmpcmd.steps);
						RXQ.waiting = 0;
					}
				break;				


				case 'v':
					RXQ.waiting = 4;
					if(checkforbytes_RXQ(RXQ.waiting))
					{
						read_RXQ();
						motor[gMotorselected].speed = (read_RXQ()- '0')*100 + (read_RXQ()- '0')*10 + (read_RXQ()-'0');
						printc('v'); printnum(motor[gMotorselected].speed);						
						RXQ.waiting = 0;
					}
				break;				
								
				case 's':				
						read_RXQ();
						printc('s');
						gStopmotors = 1;						
						RXQ.waiting = 0;					
				break;				
				
				case 'p':
						read_RXQ();
						printc('p');
						printnum_4d(motor[gMotorselected].position);
						RXQ.waiting = 0;
				break;
				
				case 't':
						read_RXQ();
						printc('t');
						if(gTerminal) gTerminal =0;
						else gTerminal = 1;
				
				case NULL: // There was no character returned
				break;
				
				default:	// unknown command
					read_RXQ();
					RXQ.waiting = 0;
					printc('?');	
				break;
			}
			
			newline();
	}
}

//--------------------------------------------
// Stepper motor routines
//--------------------------------------------

//-------------------------------------------------
// Intialize the stepper motors
//-------------------------------------------------
void Init_motors(void)
{
u8 i;
	gMotorselected = 0;
	for(i=0;i<NUMBEROFMOTORS;i++)
	{
		motor[i].state 		= IDLE;
		motor[i].steps	 	= 0;
		motor[i].step_cnt	= 0;
		motor[i].coils 		= 0b0000;
		motor[i].speed 		= MOTORSPEED;
		motor[i].speed_cnt 	= 0;
		motor[i].coilpattern_cnt =0;
		motor[i].coils = coilpattern[0];
		motor[i].position = 0;
	}
	
	motor[0].speed = 0;
	motor[1].speed = 2;

	// 
	motor[0].position = MOTORMAXPOSITION[0]; 
	motor[1].position = MOTORMAXPOSITION[1]; 
	
}


//-------------------------------------------------
// Switch off all Coils
//-------------------------------------------------
void motoroff(u8 num)
{
u8 tmp;
	switch(num)
	{
		case 0:
			tmp = PORTD & 0b11110000;
			PORTD = tmp;
		break;
		case 1:
			tmp = PORTD & 0b00001111;
			PORTD = tmp;			
		break;
		case 2:
			GRIPPER = 0;
		break;
		default:
		break;			
	}
}


//-------------------------------------------------
// Update the physical signals of the coils	
//-------------------------------------------------
void output_motor(u8 num)
{
u8 tmp;
	switch(num)
	{
		case 0:
			tmp = PORTD & 0b11110000;
			tmp |= motor[0].coils;
			PORTD = tmp;
		break;
		case 1:
			tmp = PORTD & 0b00001111;
			tmp |= (motor[1].coils)<<4;
			PORTD = tmp;			
		break;
		case 2:
			// Start the timer
			gSwtimer[0].cnt = 0;
			gSwtimer[0].enable = 1;
			GRIPPER = 1; // switch on the gripper motor;
		default:
		break;			
	}
}	

//-------------------------------------------------
// Motor Home Switch handler 
//-------------------------------------------------	
void handle_homeswitches(void)
{
	// The flag is cleared in the IDLE routine of the motor
	if((PORTA & 0x02)>>1)
	{		
		gHomeswitch[0] = 1;
	}
	if(!((PORTA&0x04)>>2))
	{
		gHomeswitch[1] = 1;
	}
	if(PORTA & 0x01)
	{		
		gHomeswitch[2] = 1;
	}
}

//-------------------------------------------------
// Print error
//-------------------------------------------------	
void error_max(u8 i)
{
//	printc('E');printc('_');printc('M');printc('A');printc('X');printc('-');
	prints(E_MAX_msg);
	switch(i){ case 0: printc('H');break;
			   case 1: printc('V');break;
			   case 2: printc('G');break;
			 }
//	printc(' ');
	newline();
}
void error_min(u8 i)
{
//	printc('E');printc('_');printc('M');printc('I');printc('N');printc('-');
	prints(E_MIN_msg);
	switch(i){ case 0: printc('H');break;
			   case 1: printc('V');break;
			   case 2: printc('G');break;
			 }
//	printc(' ');
	newline();
}



void error_home(u8 i)
{
//	printc('E');printc('_');printc('H');printc('O');printc('M');printc('E');printc('-');
	prints(E_HOME_msg);
	switch(i){	case 0: printc('H'); break;
				case 1:	printc('V'); break;
				case 2: printc('G'); break;}		
	newline();
}

void error_cd(void)
{
	printc('E');printc('_');printc('C');printc('D');printc(' ');printc(' ');printc(' ');printc(' ');
	newline();
}


void ok_home(u8 i)
{
//printc('O');printc('K');printc('_');
prints(OK_HOME_msg);
switch(i){	case 0: printc('H'); break;
			case 1:	printc('V'); break;
			case 2: printc('G'); break;}		
newline();
}

void ok_cd(void)
{
//printc('O');printc('K');printc('_');printc('C');printc('D');
prints(OK_CD_msg);
newline();
dup.ok_cd = 1;
}



//-------------------------------------------------
// Select the next coil to step the motor.
//-------------------------------------------------	
void nextcoil(u8 i,u8 dir)
{
// The vertical hardware is reversed. Dang
if (i==1)
{
	if (dir ==FWD)
		dir = REV;
	else
		dir = FWD;
}
		
switch(dir)
{
	case FWD:	// Horizontal fwd
		if(++motor[i].coilpattern_cnt > 7)
			motor[i].coilpattern_cnt = 0;
		break;
		case REV: // horizontal rev
			if(motor[i].coilpattern_cnt == 0)
				motor[i].coilpattern_cnt = 7;
			else
				motor[i].coilpattern_cnt--;										
		break;
}

motor[i].coils = coilpattern[motor[i].coilpattern_cnt];																		
}



//-------------------------------------------------
// The main handler to step the motors
//-------------------------------------------------	
void handle_steppermotors(void)
{
u8 i;
struct sCmd tmpcmd;

	for(i=0;i<NUMBEROFMOTORS; i++)
	{
		motor[i].state = motor[i].nextstate;		
		switch(motor[i].state)
		{		
		// Check if the motor is IDLE, and if there
		// are any more commands for it in the commandQ				
			case IDLE:
				if(Q_checkempty(i)== 0)
				{
					Q_read(i, &tmpcmd);					
					motor[i].nextstate = tmpcmd.dir;					
					motor[i].step_cnt = 0;
					switch(motor[i].nextstate)
					{ 
						case HOME:						
							if(motor[i].position <0){
								motor[i].nextstate = FWD;
								motor[i].steps = -(motor[i].position);
							}
							else{
								motor[i].nextstate = REV;
								motor[i].steps = motor[i].position;
							}						
						break;
						
						case CD:
							motor[i].steps = MOTORMAXPOSITION[i] - motor[i].position;
						break;
						
						case MOVETO:
							motor[i].moveto = tmpcmd.steps;
						break;
						
						default:
							motor[i].steps = tmpcmd.steps;					
						break;
					}

					gHomeswitch[0]=0; gHomeswitch[1]=0; gHomeswitch[2]=0;																		
				}
				else
					motor[i].nextstate = IDLE;
			break;

			case STOP:
				motor[i].nextstate= HOLD;			
				// now read all the commands out of the Q
				while(!Q_checkempty(i))
					Q_read(i,&tmpcmd);
			break;

			case HOLD:
				motoroff(i);
				if (gStopmotors)
					while(!Q_checkempty(i))
						Q_read(i,&tmpcmd);
						
				motor[i].nextstate = IDLE;
			break;

			case NOCD:
				error_cd();
				if (gStopmotors)
					motor[i].nextstate = STOP;
				else	
					motor[i].nextstate = HOLD;
			break;

			
			case NOHOME:
				if (gStopmotors)
					motor[i].nextstate = STOP;
				else					
					motor[i].nextstate = HOLD;
			break;

			case MOVETO:
				if(motor[i].position <= motor[i].moveto)
				{
					motor[i].nextstate = FWD;
					motor[i].steps = motor[i].moveto - motor[i].position;					
				}
				else
				{
					motor[i].nextstate = REV;
					motor[i].steps = motor[i].position - motor[i].moveto;					
				}
			break;

			case CD:
			case FHOME:
			case HOME:			
			case FWD:
			case REV:
				if(gStopmotors)
					motor[i].nextstate = STOP;
				else
				{
					// Check if we should step or not yet
					if (++motor[i].speed_cnt > motor[i].speed)			
					{									
						if (++motor[i].step_cnt > motor[i].steps)
						{	
							switch(motor[i].state){
								case FHOME:
								case HOME: motor[i].nextstate = NOHOME; break;
								case CD : motor[i].nextstate = NOCD; break;
								default : motor[i].nextstate = HOLD; break; }
						}
						else
						{	
							motor[i].nextstate = motor[i].state;
							motor[i].speed_cnt = 0;
							switch(motor[i].state)
							{
								case CD:
								case FWD:
									if(++motor[i].position > MOTORMAXPOSITION[i])
									{
										motor[i].nextstate = STOP;
										error_max(i);
									}
									// load the next pattern
									nextcoil(i,FWD);
								break;
								
								case REV:
									if(--motor[i].position < MOTORMINPOSITION[i])						
									{
										motor[i].nextstate = STOP;									
 										error_min(i);
									}
									nextcoil(i,REV);
								break;
									
								case FHOME:							
									motor[i].position--;								
									nextcoil(i,REV);
								break;
								case HOME:	
									if(motor[i].position < 0)
									{
										motor[i].position++;
										nextcoil(i,FWD);
									}
									else
									{
										motor[i].position--;
										nextcoil(i,REV);
									}
								break;
																																	
								default : 
								break;
							}				
							output_motor(i);
						}
					}
				
				if ((motor[i].state == FHOME) && (gHomeswitch[i]))
				{
					ok_home(i);
					motor[i].nextstate = HOLD;
					motor[i].position = 0;
				}
				if ((motor[i].state == CD) && (gHomeswitch[2]))
				{
					ok_cd();
					motor[i].nextstate = HOLD;
				}
				
			}
			break;
		
			default:
			break;	
			}
		}			
	gStopmotors = 0;		
}



//--------------------------------------------
// handle_led()
//--------------------------------------------
void handle_led(void)
{
	if(++gLedcnt > 200)
	{
		LED0 = !LED0;
		gLedcnt = 0;
	}
}


//--------------------------------------------
// Init_cmdQ(void)
//--------------------------------------------
void Init_cmdQ(void)
{
	for(i=0;i<NUMBEROFMOTORS; i++)
	{
		gCmdQ[i].rdptr = 0;
		gCmdQ[i].wrptr = 0;	
	}
}

//--------------------------------------------
// Init_world() load the positions of the stacks
// and tray
//--------------------------------------------
void Init_world(void)
{
	gWorld.tray.h = 0; gWorld.tray.v = 300;
	gWorld.instack.h = 200; gWorld.instack.v = 200;
	gWorld.outstack.h = 400; gWorld.instack.v = 600;	
}

//--------------------------------------------
// Init_homemotors()
//--------------------------------------------
void Init_homemotors(void)
{
struct sCmd tmpcmd;
	tmpcmd.dir = FHOME; tmpcmd.steps = MOTORMAXPOSITION[0];
	Q_write(MH,&tmpcmd);							
	tmpcmd.dir = FHOME; tmpcmd.steps = MOTORMAXPOSITION[1];
	Q_write(MV,&tmpcmd);									
	tmpcmd.dir = FWD; tmpcmd.steps = 2;
	Q_write(MG,&tmpcmd);										
}


//--------------------------------------------
// Duplicator_statemachine()
// Controls the machine to pick up grab drop cd's
//--------------------------------------------
void duplicator_statemachine(void)
{
struct sCmd cmd;

dup.state = dup.nextstate;

switch(dup.state)
{
	case IDLE:
		dup.nextstate = INSTACK;
	break;
	case INSTACK:
		dup.nextstate = WAIT_CD_INSTACK;		
		cmd.dir = MOVETO;cmd.steps = gWorld.instack.h;
		Q_write(MH,&cmd);
		cmd.dir = MOVETO;cmd.steps = gWorld.instack.v;
		Q_write(MV,&cmd);
		// move down to the cd now
		cmd.dir = CD;cmd.steps = 0;
		Q_write(MV,&cmd);
		
	break;
	case WAIT_CD_INSTACK:
		if (dup.ok_cd)
		{
			dup.ok_cd = 0;
			dup.nextstate = WAIT_GRAB_INSTACK;
			cmd.dir=FWD;cmd.steps=20;
			motor[MG].speed = 020;
			Q_write(MG,&cmd);
		}			
	break;
	case WAIT_GRAB_INSTACK:
		if (dup.ok_grab)
		{
			dup.ok_grab = 0;
			cmd.dir=MOVETO; cmd.steps = gWorld.tray.h;
			Q_write(MH,&cmd);
			dup.nextstate = HALT;
		}
	
	case HALT:
		dup.nextstate = HALT;
	break;
	
	default:
	break;
}
}


//--------------------------------------------
// Main(void)
//--------------------------------------------
void main(void)
{
  Init();       
  Init_comms();
  Init_cmdQ();
  Init_motors();
  Init_swtimers();
  Init_hwtimers();
  Init_world();
	
  gHomeswitch[0] = 0; gHomeswitch[1] = 0; gHomeswitch[2] = 0;  
  Init_homemotors();
  prints(READY_msg);newline();
  dup.state = dup.nextstate = IDLE;
  while(1)
  {
  	    	  
	  // Check if there are any characters in the Q just waiting to be read
	  if(!RXQ_empty()) 
	  	WDJ_protocol_handler();
	  	 	
	  // Check if the timer has timed out to step the motors
	  if(gStepmotors) 
	  {	
	  	  gStepmotors =0;
	  	  handle_homeswitches();
		  handle_steppermotors();
		  handle_led();	  			
		  duplicator_statemachine();
	   }
	
  }
}
