
#define OWCRC_C
#include "owcrc.h"


// Local global variables
u16 utilcrc16[MAX_PORTNUM];
u08 utilcrc8[MAX_PORTNUM];
//const static u08 dscrc_table[] = {
//const static u08 dscrc_table[] = {

static char PROGMEM dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//--------------------------------------------------------------------------
// Reset crc16 to the value passed in
//
// 'reset' - data to set crc16 to.
//
void setcrc16(u08 portnum, u16 reset)
{
   utilcrc16[portnum] = reset;
   return;
}

//--------------------------------------------------------------------------
// Reset crc8 to the value passed in
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'reset'    - data to set crc8 to
//
void setcrc8(u08 portnum, u08 reset)
{
   utilcrc8[portnum] = reset;
   return;
}

//--------------------------------------------------------------------------
// Calculate a new CRC16 from the input data short.  Return the current
// CRC16 and also update the global variable CRC16.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'data'     - data to perform a CRC16 on
//
// Returns: the current CRC16
//
/*
const u08 oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

u16 docrc16(u08 portnum, u16 cdata)
{
   cdata = (cdata ^ (utilcrc16[portnum] & 0xff)) & 0xff;
   utilcrc16[portnum] >>= 8;

   if (oddparity[cdata & 0xf] ^ oddparity[cdata >> 4])
     utilcrc16[portnum] ^= 0xc001;

   cdata <<= 6;
   utilcrc16[portnum]   ^= cdata;
   cdata <<= 1;
   utilcrc16[portnum]   ^= cdata;

   return utilcrc16[portnum];
}
*/
//--------------------------------------------------------------------------
// Update the Dallas Semiconductor One Wire CRC (utilcrc8) from the global
// variable utilcrc8 and the argument.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
// 'x'        - data byte to calculate the 8 bit crc from
//
// Returns: the updated utilcrc8.
//
u08 docrc8(u08 portnum, u08 x)
{
const u08 *ptr;
   //utilcrc8[portnum] = dscrc_table[utilcrc8[portnum] ^ x];
    ptr = dscrc_table + (utilcrc8[portnum] ^ x);
    utilcrc8[portnum] = pgm_read_byte(ptr);
   return utilcrc8[portnum];
}
