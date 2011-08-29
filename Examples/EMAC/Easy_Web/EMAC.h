/*----------------------------------------------------------------------------
 *      LPC1768 Ethernet Definitions
 *----------------------------------------------------------------------------
 *      Name:    EMAC.H
 *      Purpose: LPC1768 EMAC hardware implementation definitions
 *----------------------------------------------------------------------------
 *      Copyright (c) 2006 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __EMAC_H
#define __EMAC_H

#include "lpc17xx_emac.h"

#ifdef MCB_LPC_1768
#define MYMAC_1         0x1E            /* our ethernet (MAC) address        */
#define MYMAC_2         0x30            /* (MUST be unique in LAN!)          */
#define MYMAC_3         0x6c
#define MYMAC_4         0xa2
#define MYMAC_5         0x45
#define MYMAC_6         0x5e
#elif defined(IAR_LPC_1768)
/* Using IAR LPC1768 KickStart board */
#define MYMAC_1         0x00            /* our ethernet (MAC) address        */
#define MYMAC_2         0xFF            /* (MUST be unique in LAN!)          */
#define MYMAC_3         0xFF
#define MYMAC_4         0xFF
#define MYMAC_5         0xFF
#define MYMAC_6         0xFF
#endif

// prototypes
void           Init_EMAC(void);
unsigned short ReadFrameBE_EMAC(void);
void           CopyToFrame_EMAC(void *Source, unsigned int Size);
void           CopyFromFrame_EMAC(void *Dest, unsigned short Size);
void           DummyReadFrame_EMAC(unsigned short Size);
unsigned short StartReadFrame(void);
void           EndReadFrame(void);
unsigned int   CheckFrameReceived(void);
void           RequestSend(unsigned short FrameSize);
unsigned int   Rdy4Tx(void);

#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

