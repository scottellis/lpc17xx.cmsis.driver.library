/**********************************************************************
* $Id$		spi_loopback_test.c					2010-05-21
*//**
* @file		spi_loopback_test.c
* @brief	An example of SPI using polling mode with loop-back
* 			(MISO <-> MOSI)connection to test SPI driver
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
#include "lpc17xx_spi.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SPI_LoopBack	LoopBack
 * @ingroup SPI_Examples
 * @{
 */


/************************** PRIVATE DEFINITIONS *********************/
// PORT number that /CS pin assigned on
#define CS_PORT_NUM		0
// PIN number that  /CS pin assigned on
#define CS_PIN_NUM		16
#define SPI_DATABIT_SIZE 		10
/** Max buffer length */
#define BUFFER_SIZE			0x40

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"SPI demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
"\t An example of SPI using polling mode with loop-back (MISO <-> MOSI) \n\r"
"\t connection to test SPI driver \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";

// SPI Configuration structure variable
SPI_CFG_Type SPI_ConfigStruct;

#if (SPI_DATABIT_SIZE == 8)
uint8_t Tx_Buf[BUFFER_SIZE];
uint8_t Rx_Buf[BUFFER_SIZE];
#else
uint16_t Tx_Buf[BUFFER_SIZE];
uint16_t Rx_Buf[BUFFER_SIZE];
#endif

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void Buffer_Init(void);
void Error_Loop(void);
void Buffer_Verify(void);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void Buffer_Init(void)
{
	uint8_t i;
#if (SPI_DATABIT_SIZE == 8)
	for (i = 0; i < BUFFER_SIZE; i++) {
		Tx_Buf[i] = i;
		Rx_Buf[i] = 0;
	}
#else
	for (i = 0; i < BUFFER_SIZE/2; i++) {
		Tx_Buf[i] = i;
		Rx_Buf[i] = 0;
	}
#endif
}

/*********************************************************************//**
 * @brief		Error Loop (called by Buffer_Verify() if any error)
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Error_Loop(void)
{
	/* Loop forever */
	while (1);
}

/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Verify(void)
{
	uint8_t i;
#if (SPI_DATABIT_SIZE==8)
	uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
	uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];
	for ( i = 0; i < SPI_DATABIT_SIZE; i++ )
	{
		if ( *src_addr++ != *dest_addr++ )
		{
			/* Call Error Loop */
			_DBG_("Verify error");
			Error_Loop();
		}
	}
#else
	uint16_t *src_addr = (uint16_t *) &Tx_Buf[0];
	uint16_t *dest_addr = (uint16_t *) &Rx_Buf[0];
	for ( i = 0; i < SPI_DATABIT_SIZE/2; i++ )
	{
		if ( *src_addr++ != *dest_addr++ )
		{
			/* Call Error Loop */
			_DBG_("Verify error");
			Error_Loop();
		}
	}
#endif

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
 * @brief		c_entry: Main SPI program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	SPI_DATA_SETUP_Type xferConfig;

	/*
	 * Initialize SPI pin connect
	 * P0.15 - SCK;
	 * P0.16 - SSEL - used as GPIO
	 * P0.17 - MISO
	 * P0.18 - MOSI
	 */
	PinCfg.Funcnum = 3;
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
	PinCfg.Funcnum = 0;
	PINSEL_ConfigPin(&PinCfg);

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

	SPI_ConfigStruct.CPHA = SPI_CPHA_SECOND;
	SPI_ConfigStruct.CPOL = SPI_CPOL_LO;
	SPI_ConfigStruct.ClockRate = 2000000;
	SPI_ConfigStruct.DataOrder = SPI_DATA_MSB_FIRST;
	SPI_ConfigStruct.Databit = SPI_DATABIT_SIZE;
	SPI_ConfigStruct.Mode = SPI_MASTER_MODE;
	// Initialize SPI peripheral with parameter given in structure above
	SPI_Init(LPC_SPI, &SPI_ConfigStruct);

	/* Initialize Buffer */
	Buffer_Init();

	xferConfig.tx_data = Tx_Buf;
	xferConfig.rx_data = Rx_Buf;
	xferConfig.length = BUFFER_SIZE;
	SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);

	// Verify buffer after transferring
	Buffer_Verify();

	_DBG_("Verify complete");

	SPI_DeInit(LPC_SPI);
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
