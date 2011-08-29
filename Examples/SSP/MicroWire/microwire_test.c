/**********************************************************************
* $Id$		microwire_test.c					2010-05-21
*//**
* @file		microwire_test.c
* @brief	This example describes how to use SPP peripheral in
* 			MicroWire frame format
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "lpc17xx_ssp.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SSP_MicroWire	MicroWire
 * @ingroup SSP_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS *************************/
/* Idle char */
#define IDLE_CHAR	0xFF

/** Used SSP device as master definition */
#define USEDSSPDEV_M		0

/** Used SSP device as slave definition */
#define USEDSSPDEV_S		1

/** Max buffer length */
#define BUFFER_SIZE			0x40

/* These macro below is used in MiroWire Frame Format.
 * Since master in MicroWire Frame Format must send a Operation Command
 * to slave before each transmission between master and slave
 */
/* Write command */
#define MicroWire_WR_CMD	((uint8_t)(0x00))
/* Read command */
#define MicroWire_RD_CMD	((uint8_t)(0x01))


#if (USEDSSPDEV_M == USEDSSPDEV_S)
#error "Master and Slave SSP device are duplicated!"
#endif

#if (USEDSSPDEV_M == 0)
#define SSPDEV_M LPC_SSP0
#elif (USEDSSPDEV_M == 1)
#define SSPDEV_M LPC_SSP1
#elif
#error "Master SSP device not defined!"
#endif

#if (USEDSSPDEV_S == 0)
#define SSPDEV_S LPC_SSP0
#elif (USEDSSPDEV_S == 1)
#define SSPDEV_S LPC_SSP1
#elif
#error "Slave SSP device not defined!"
#endif

/* These variable below are used in Master SSP -------------------- */
/* Read data pointer */
uint8_t *pRdBuf_M;
/* Index of read data mode */
uint32_t RdIdx_M;
/* Length of data */
uint32_t DatLen_M;
/* Master Rx Buffer */
uint8_t Master_Rx_Buf[BUFFER_SIZE];


/* These variable below are used in Slave SSP -------------------- */
/* Write data pointer */
uint8_t *pWrBuf_S;
/* Index of write data mode */
uint32_t WrIdx_S;
/* Length of data */
uint32_t DatLen_S;
/* Slave Tx Buffer */
uint8_t Slave_Tx_Buf[BUFFER_SIZE];
/* Last command */
uint8_t Last_cmd;

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"SSP demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" This example uses two SSP peripherals in MicroWire frame format \n\r"
" \t one is set as master mode and the other is set as slave mode. \n\r"
"\t The master and slave transfer a number of data bytes together \n\r"
"\t in polling mode \n\r"
"********************************************************************************\n\r";

// SSP Configuration structure variable
SSP_CFG_Type SSP_ConfigStruct;

/************************** PRIVATE FUNCTIONS *************************/
void ssp_MW_SendCMD(LPC_SSP_TypeDef *SSPx, uint8_t cmd);
uint16_t ssp_MW_GetRSP(LPC_SSP_TypeDef *SSPx);
uint8_t ssp_MW_GetCMD(LPC_SSP_TypeDef *SSPx);
void ssp_MW_SendRSP(LPC_SSP_TypeDef *SSPx, uint16_t Rsp);
void print_menu(void);
void Buffer_Init(void);
void Buffer_Verify(void);
void Error_Loop(void);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Send command to slave in master mode
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void ssp_MW_SendCMD(LPC_SSP_TypeDef *SSPx, uint8_t cmd)
{
	// wait for current SSP activity complete
	while (SSP_GetStatus(SSPx, SSP_STAT_BUSY) ==  SET);

	SSP_SendData(SSPx, (uint16_t) cmd);
}

/*********************************************************************//**
 * @brief		Get respond from slave after sending a command in master mode
 * @param[in]	None
 * @return 		None
 **********************************************************************/
uint16_t ssp_MW_GetRSP(LPC_SSP_TypeDef *SSPx)
{
	while (SSP_GetStatus(SSPx, SSP_STAT_RXFIFO_NOTEMPTY) == RESET);
	return (SSP_ReceiveData(SSPx));
}

/*********************************************************************//**
 * @brief		Get command from master in slave mode
 * @param[in]	None
 * @return 		None
 **********************************************************************/
uint8_t ssp_MW_GetCMD(LPC_SSP_TypeDef *SSPx)
{
	// Wait for coming CMD
	while (SSP_GetStatus(SSPx, SSP_STAT_RXFIFO_NOTEMPTY) == RESET);

	return ((uint8_t)(SSP_ReceiveData(SSPx)));
}

/*********************************************************************//**
 * @brief		Send respond to master in slave mode
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void ssp_MW_SendRSP(LPC_SSP_TypeDef *SSPx, uint16_t Rsp)
{
	// wait for current SSP activity complete
	while (SSP_GetStatus(SSPx, SSP_STAT_BUSY) ==  SET);

	SSP_SendData(SSPx, Rsp);
}

/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void Buffer_Init(void)
{
	uint8_t i;

	for (i = 0; i < BUFFER_SIZE; i++) {
		Slave_Tx_Buf[i] = i;
		Master_Rx_Buf[i] = 0;
	}

}

/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Verify(void)
{
	uint8_t i;
	uint8_t *pSTx = (uint8_t *) &Slave_Tx_Buf[0];
	uint8_t *pMRx = (uint8_t *) &Master_Rx_Buf[0];

	for ( i = 0; i < BUFFER_SIZE; i++ )
	{
		if (*pMRx++ != *pSTx++)
		{
			/* Call Error Loop */
			Error_Loop();
		}
	}
}

/*********************************************************************//**
 * @brief		Error Loop (called by Buffer_Verify() if any error)
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Error_Loop(void)
{
	/* Loop forever */
	_DBG_("Verify fail!\n\r");
	while (1);
}

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main MICROWIRE program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	uint32_t cnt;
	PINSEL_CFG_Type PinCfg;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/*
	 * Initialize SSP pin connect
	 * P0.6 - SSEL1
	 * P0.7 - SCK1
	 * P0.8 - MISO1
	 * P0.9 - MOSI1
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);


	/*
	 * Initialize SSP pin connect
	 * P0.15 - SCK
	 * P0.16 - SSEL
	 * P0.17 - MISO
	 * P0.18 - MOSI
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);


	/* Initializing Master SSP device section ------------------------------------------- */
	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);
	// Re-configure SSP to MicroWire frame format
	SSP_ConfigStruct.FrameFormat = SSP_FRAME_MICROWIRE;
	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(SSPDEV_M, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(SSPDEV_M, ENABLE);


    /* Initializing Slave SSP device section ------------------------------------------- */
	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);
	/* Re-configure mode for SSP device */
	SSP_ConfigStruct.Mode = SSP_SLAVE_MODE;
	// Re-configure SSP to MicroWire frame format
	SSP_ConfigStruct.FrameFormat = SSP_FRAME_MICROWIRE;
	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(SSPDEV_S, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(SSPDEV_S, ENABLE);


	/* Initializing Buffer section ------------------------------------------------- */
	Buffer_Init();

	/* Start Transmit/Receive between Master and Slave ----------------------------- */
	pRdBuf_M = (uint8_t *)&Master_Rx_Buf[0];
	RdIdx_M = 0;
	DatLen_M = BUFFER_SIZE;
	pWrBuf_S = (uint8_t *)&Slave_Tx_Buf[0];
	WrIdx_S = 0;
	DatLen_S = BUFFER_SIZE;
	/* Force Last command to Read command as default */
	Last_cmd = MicroWire_RD_CMD;

	/* Clear all remaining data in RX FIFO */
	while (SSP_GetStatus(SSPDEV_M, SSP_STAT_RXFIFO_NOTEMPTY))
	{
		SSP_ReceiveData(SSPDEV_M);
	}
	while (SSP_GetStatus(SSPDEV_S, SSP_STAT_RXFIFO_NOTEMPTY))
	{
		SSP_ReceiveData(SSPDEV_S);
	}

	for (cnt = 0; cnt < BUFFER_SIZE; cnt++)
	{
		/* The slave must initialize data in FIFO for immediately transfer from master
		 * due to last received command
		 */
		if (Last_cmd == MicroWire_RD_CMD)
		{
			// Then send the respond to master, this contains data
			ssp_MW_SendRSP(SSPDEV_S, (uint16_t) *(pWrBuf_S + WrIdx_S++));
		}
		else
		{
			// Then send the respond to master, this contains data
			ssp_MW_SendRSP(SSPDEV_S, 0xFF);
		}
		/* Master must send a read command to slave,
		 * the slave then respond with its data in FIFO
		 */
		ssp_MW_SendCMD(SSPDEV_M, MicroWire_RD_CMD);

		// Master receive respond
		*(pRdBuf_M + RdIdx_M++) = (uint8_t) ssp_MW_GetRSP(SSPDEV_M);

		// Re-assign Last command
		Last_cmd = ssp_MW_GetCMD(SSPDEV_S);
	}

	/* Verify buffer */
	Buffer_Verify();

	_DBG_("Verify success!\n\r");

    /* Loop forever */
    while(1);
    return 1;
}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/*
 * @}
 */
