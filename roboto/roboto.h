//------------------------------------
// Porting types
//------------------------------------
#define u8 unsigned char

//------------------------------------
// Some Macro's
//------------------------------------
#define NOP asm("nop")
#define NULL 0
#define TRUE 1
#define FALSE 0
#define PORTBIT(p,b) ((unsigned)&(p)*8+(b))

/*          PORTC bits               */
static bit LED0 @ PORTBIT(PORTC,0);
static bit LED1 @ PORTBIT(PORTC,1);
static bit GRIPPER @ PORTBIT(PORTC,3);
static bit SW0 @ PORTBIT(PORTA,0);
static bit SW1 @ PORTBIT(PORTA,1);
static bit SW2 @ PORTBIT(PORTA,2);
static bit _CRDAT3 @ PORTBIT(PORTC,2);
static bit IICSCL  @ PORTBIT(PORTC,3);
static bit IICSDA  @ PORTBIT(PORTC,4);
static bit _CRDAT4 @ PORTBIT(PORTC,5);
static bit TXD     @ PORTBIT(PORTC,6);
static bit RXD     @ PORTBIT(PORTC,7);


 

//--------------------------------------
// Serial RS232 defines
//--------------------------------------
#define SERIAL_SPEED 12     // 4Mhz 19200bd SBRGH = 1 
#define RXQSIZE 8
#define RX_TIMEOUT 1242


//--------------------------------------
// Structure for the receive character Q
//--------------------------------------
typedef struct sCharQ{
u8 waiting;
u8 timeout;
u8 forceread;
u8 rdptr;
u8 wrptr;
u8 buf[RXQSIZE];
};

#define CMDQ_SIZE 3

//--------------------------------------
// Structure for Command
//--------------------------------------
typedef struct sCmd{
u8 	dir;
int steps;
};

//--------------------------------------
// Structure for Q
//--------------------------------------
typedef struct sCmdQ{
u8 rdptr;
u8 wrptr;
struct sCmd ringbuf[CMDQ_SIZE];
};


//----------------------------------------
// Motors
//----------------------------------------

#define MH 0
#define MV 1
#define MG 2

#define MOTORSPEED 0
#define NUMBEROFMOTORS 3
#define IDLE 0
#define HOLD 1
#define FWD 2
#define REV 3
#define WAIT 4
#define HOME 5
#define NOHOME 6
#define CD 7 
#define NOCD 8
#define STOP 9
#define FHOME 10
#define MOVETO 11

const int MOTORMAXPOSITION[3] = {850,800,10000};
const int MOTORMINPOSITION[3] = {-200,-2,-10000};


// The stepper motor coil pattern to halfstep and 1.4 times torque
const u8 coilpattern[8] ={0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08};

typedef struct sMotorcmd{
int 		steps;
u8			direction;
u8 			speed;
};

typedef struct sMotor{
struct sMotorcmd cmdQ[CMDQ_SIZE];
u8 			state;    // forward backwards or stopped
u8 			nextstate;
int 		position;
int 		moveto;
int			steps;
int 		step_cnt;
u8 			coils;
u8 			speed;
u8			speed_cnt;
u8			coilpattern_cnt;
};

typedef struct sLinearMotor{
u8 			state;
u8 			timeon;
}

//----------------------------------------
// Timers
//----------------------------------------

typedef struct sSwtimer{
u8 cmp;
u8 cnt;
u8 enable;
};

typedef struct sCoords{
int h;
int v;
};

typedef struct sWorld{
struct sCoords tray;
struct sCoords instack;
struct sCoords outstack;
};

typedef struct sDup{
u8 state;
u8 nextstate;
u8 ok_cd;
u8 ok_grab;
u8 ok_drop;
};
#define INSTACK 1
#define WAIT_CD_INSTACK 2
#define WAIT_GRAB_INSTACK 3
#define HALT 99

