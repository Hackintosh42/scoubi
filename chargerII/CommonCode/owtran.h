

#ifndef OWTRAN_H
#define OWTRAN_H
#include "avrheaders.h"
#include "global.h"
#include "ow.h"
#include "ownet.h"


u08 owBlock(u08 portnum, u08 do_reset, u08 *tran_buf, u08 tran_len);

#endif


