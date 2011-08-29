#ifndef __EMAC_H
#define __EMAC_H

#include "lpc_types.h"
#include "uip-conf.h"
#include "uipopt.h"
#include "lpc17xx_emac.h"

#define EMAC_MAX_PACKET_SIZE (UIP_CONF_BUFFER_SIZE + 16)	// 1536 bytes

#define ENET_DMA_DESC_NUMB   	3
#define AUTO_NEGOTIATION_ENA 	1  		// Enable PHY Auto-negotiation
#define PHY_TO               	200000  // ~10sec
#define RMII					1		// If zero, it's a MII interface

/* This is the MAC address of LPC17xx */
/* 0C 1D 12 E0 1F 10*/
#ifdef MCB_LPC_1768
#define EMAC_ADDR0		0x10
#define EMAC_ADDR1		0x1F
#define EMAC_ADDR2		0xE0
#define EMAC_ADDR3		0x12
#define EMAC_ADDR4		0x1D
#define EMAC_ADDR5		0x0C
#elif defined(IAR_LPC_1768)
#define EMAC_ADDR0		0x00
#define EMAC_ADDR1		0xFF
#define EMAC_ADDR2		0xFF
#define EMAC_ADDR3		0xFF
#define EMAC_ADDR4		0xFF
#define EMAC_ADDR5		0xFF
#endif


/* Configurable macro ---------------------- */
#define SPEED_100               1
#define SPEED_10                0
#define FULL_DUPLEX             1
#define HALF_DUPLEX             0

#define FIX_SPEED               SPEED_100
#define FIX_DUPLEX              FULL_DUPLEX


BOOL_8 tapdev_init(void);
UNS_32 tapdev_read(void * pPacket);
BOOL_8 tapdev_send (void *pPacket, UNS_32 size);

#endif
