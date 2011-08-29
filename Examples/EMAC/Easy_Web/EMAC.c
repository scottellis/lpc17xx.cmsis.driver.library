/******************************************************************
 *****                                                        *****
 *****  Name: cs8900.c                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: ethernet packet-driver for use with LAN-        *****
 *****        controller CS8900 from Crystal/Cirrus Logic     *****
 *****                                                        *****
 *****  NXP: Module modified for use with NXP            	  *****
 *****        LPC1768 EMAC Ethernet controller                *****
 *****                                                        *****
 ******************************************************************/

#include "EMAC.h"
#include "tcpip.h"
#include "lpc17xx_emac.h"
#include "lpc17xx_pinsel.h"

/* For debugging... */
#include "debug_frmwrk.h"
#include <stdio.h>
#define DB	_DBG((uint8_t *)db)
char db[64];

static unsigned short *rptr;
static unsigned short *tptr;

/*
 * NXP: Here AHBRAM1 section still not be used, so a mount of this section
 * will be used to store buffer data get from receive packet buffer of EMAC
 */
static unsigned short *pgBuf = (unsigned short *)LPC_AHBRAM1_BASE;

// configure port-pins for use with LAN-controller,
// reset it and send the configuration-sequence
void Init_EMAC(void)
{
	uint32_t delay;

	/* EMAC configuration type */
	EMAC_CFG_Type Emac_Config;
	/* pin configuration */
	PINSEL_CFG_Type PinCfg;

	/* EMAC address */
	uint8_t EMACAddr[] = {MYMAC_1, MYMAC_2, MYMAC_3, MYMAC_4, MYMAC_5, MYMAC_6};

	/*
	 * Enable P1 Ethernet Pins:
	 * P1.0 - ENET_TXD0
	 * P1.1 - ENET_TXD1
	 * P1.4 - ENET_TX_EN
	 * P1.8 - ENET_CRS
	 * P1.9 - ENET_RXD0
	 * P1.10 - ENET_RXD1
	 * P1.14 - ENET_RX_ER
	 * P1.15 - ENET_REF_CLK
	 * P1.16 - ENET_MDC
	 * P1.17 - ENET_MDIO
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;

	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 4;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 14;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);

	_DBG_("Init EMAC module");
	sprintf(db,"MAC addr: %X-%X-%X-%X-%X-%X \n\r", \
			 EMACAddr[0],  EMACAddr[1],  EMACAddr[2], \
			  EMACAddr[3],  EMACAddr[4],  EMACAddr[5]);
	DB;

	Emac_Config.Mode = EMAC_MODE_AUTO;
	Emac_Config.pbEMAC_Addr = EMACAddr;
	// Initialize EMAC module with given parameter
	while (EMAC_Init(&Emac_Config) == ERROR){
		// Delay for a while then continue initializing EMAC module
		_DBG_("Error during initializing EMAC, restart after a while");
		for (delay = 0x100000; delay; delay--);
	}
	_DBG_("Init EMAC complete");
}


// reads a word in little-endian byte order from RX_BUFFER

unsigned short ReadFrame_EMAC(void)
{
  return (*rptr++);
}

// reads a word in big-endian byte order from RX_FRAME_PORT
// (useful to avoid permanent byte-swapping while reading
// TCP/IP-data)

unsigned short ReadFrameBE_EMAC(void)
{
  unsigned short ReturnValue;

  ReturnValue = SwapBytes (*rptr++);
  return (ReturnValue);
}


// copies bytes from frame port to MCU-memory
// NOTES: * an odd number of byte may only be transfered
//          if the frame is read to the end!
//        * MCU-memory MUST start at word-boundary

void CopyFromFrame_EMAC(void *Dest, unsigned short Size)
{
  unsigned short * piDest;                       // Keil: Pointer added to correct expression

  piDest = Dest;                                 // Keil: Line added
  while (Size > 1) {
    *piDest++ = ReadFrame_EMAC();
    Size -= 2;
  }

  if (Size) {                                         // check for leftover byte...
    *(unsigned char *)piDest = (char)ReadFrame_EMAC();// the LAN-Controller will return 0
  }                                                   // for the highbyte
}

// does a dummy read on frame-I/O-port
// NOTE: only an even number of bytes is read!

void DummyReadFrame_EMAC(unsigned short Size)    // discards an EVEN number of bytes
{                                                // from RX-fifo
  while (Size > 1) {
    ReadFrame_EMAC();
    Size -= 2;
  }
}

// Reads the length of the received ethernet frame and checks if the
// destination address is a broadcast message or not
// returns the frame length
unsigned short StartReadFrame(void) {
	unsigned short RxLen;
	EMAC_PACKETBUF_Type RxPack;

	RxLen = EMAC_GetReceiveDataSize() - 3;
	// Copy packet to data buffer
	RxPack.pbDataBuf = (uint32_t *)pgBuf;
	RxPack.ulDataLen = RxLen;
	EMAC_ReadPacketBuffer(&RxPack);
	// Point to the data buffer
	rptr = (unsigned short *)pgBuf;
	return(RxLen);
}

// Release the buffer after reading all the content inside
void EndReadFrame(void) {
	// just call EMAC_UpdateConsumeIndex() in EMAC driver
	EMAC_UpdateRxConsumeIndex();
}

// Check whether if there is a receive packet coming
unsigned int CheckFrameReceived(void) {             // Packet received ?
	// Just call EMAC_CheckReceiveIndex() in EMAC driver
	if (EMAC_CheckReceiveIndex() == TRUE){
		return (1);
	} else {
		return (0);
	}
}

// requests space in EMAC memory for storing an outgoing frame
void RequestSend(unsigned short FrameSize)
{
	// Nothing to do here, just implemented in CopyToFrame_EMAC()
}

// check if ethernet controller is ready to accept the
// frame we want to send

unsigned int Rdy4Tx(void)
{
  return (1);   // the ethernet controller transmits much faster
}               // than the CPU can load its buffers


// writes a word in little-endian byte order to TX_BUFFER
void WriteFrame_EMAC(unsigned short Data)
{
  *tptr++ = Data;
}

// copies bytes from MCU-memory to frame port
// NOTES: * an odd number of byte may only be transfered
//          if the frame is written to the end!
//        * MCU-memory MUST start at word-boundary

void CopyToFrame_EMAC(void *Source, unsigned int Size)
{
	EMAC_PACKETBUF_Type TxPack;

	// Setup Tx Packet buffer
	// NXP: Added for compatibility with old style
	TxPack.ulDataLen = Size;
	TxPack.pbDataBuf = (uint32_t *)Source;
	EMAC_WritePacketBuffer(&TxPack);
	EMAC_UpdateTxProduceIndex();
}

