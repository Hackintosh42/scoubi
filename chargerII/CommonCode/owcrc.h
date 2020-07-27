

#ifndef OWCRC_H
#define OWCRC_H

#include "avrheaders.h"
#include "global.h"	
#include "ow.h"	

void setcrc16(u08 portnum, u16 reset);
u16 docrc16(u08 portnum, u16 cdata);
void setcrc8(u08 portnum, u08 reset);
u08 docrc8(u08 portnum, u08 x);

#endif


