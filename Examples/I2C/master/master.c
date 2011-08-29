/**********************************************************************
* $Id$		master.c  				2010-05-21
*//**
* @file		master.c
* @brief	This example describes how to configure I2C as master device
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
#include "lpc17xx_i2c.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup I2C_master	master
 * @ingroup I2C_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device as slave definition */
#define USEDI2CDEV_M		0
/** Own Slave address in Slave I2C device */
#define I2CDEV_S_ADDR	(0x90>>1)

/** Max buffer length */
#define BUFFER_SIZE			0x10

#if (USEDI2CDEV_M == 0)
#define I2CDEV_M LPC_I2C0
#elif (USEDI2CDEV_M == 2)
#define I2CDEV_M LPC_I2C2
#else
#error "Master I2C device not defined!"
#endif

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"I2C demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - This example uses two I2C peripheral, one set as master and \n\r"
"\t the other set as slave \n\r"
"\t Test Master mode as alone device \n\r"
"********************************************************************************\n\r";

/** These global variables below used in interrupt mode - Slave device ----------------*/
uint8_t Master_Buf[BUFFER_SIZE];
uint8_t master_test[2];

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void Buffer_Init(uint8_t type);

/*-------------------------PRIVATE FUNCTIONS-----------------------------*/
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG_(menu1);
}

/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	type:
 * 				- 0: Initialize Master_Buf with increment value from 0
 * 					Fill all member in Slave_Buf with 0
 * 				- 1: Initialize Slave_Buf with increment value from 0
 * 					Fill all member in Master_Buf with 0
 * @return 		None
 **********************************************************************/
void Buffer_Init(uint8_t type)
{
	uint8_t i;

	if (type)
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Master_Buf[i] = i;
		}
	}
	else
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Master_Buf[i] = 0;
		}
	}
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	I2C_M_SETUP_Type transferMCfg;
	uint32_t tempp;
	uint8_t *pdat;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	print_menu();


	/* I2C block ------------------------------------------------------------------- */

	/*
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
#if ((USEDI2CDEV_M == 0))
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
#endif
#if ((USEDI2CDEV_M == 2))
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#endif

	// Initialize Slave I2C peripheral
	I2C_Init(I2CDEV_M, 100000);

	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_M, ENABLE);

	/* Transmit -------------------------------------------------------- */
	_DBG_("Press '1' to transmit");
	while (_DG != '1');
	_DBG_("Start Transmit...");

	/* Initialize buffer */
	Buffer_Init(1);

	/* Start I2C slave device first */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = sizeof(Master_Buf);
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


	/* Receive -------------------------------------------------------- */
	_DBG_("Press '2' to receive");
	while(_DG != '2');
	_DBG_("Receive...");

	/* Initialize buffer */
	Buffer_Init(0);

	/* Start I2C slave device first */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = NULL ;
	transferMCfg.tx_length = 0;
	transferMCfg.rx_data = Master_Buf;
	transferMCfg.rx_length = sizeof(Master_Buf);
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);

	pdat = Master_Buf;
	// Verify
	for (tempp = 0; tempp < sizeof(Master_Buf); tempp++){
		if (*pdat++ != tempp){
			_DBG_("Verify error");
			break;
		}
	}
	if (tempp == sizeof(Master_Buf)){
		_DBG_("Verify successfully");
	}

#if 1
	/* Transmit and receive -------------------------------------------------------- */
	_DBG_("Press '3' to Transmit, then repeat start and receive...");
	while (_DG != '3');

	/* Initialize buffer */
	Buffer_Init(0);
	master_test[0] = 0xAA;
	master_test[1] = 0x55;

	/* Start I2C slave device first */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = master_test ;
	transferMCfg.tx_length = sizeof(master_test);
	transferMCfg.rx_data = Master_Buf;
	transferMCfg.rx_length = sizeof(Master_Buf);
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
#endif

	pdat = Master_Buf;
	// Verify
	for (tempp = 0; tempp < sizeof(Master_Buf); tempp++){
		if (*pdat++ != tempp){
			_DBG_("Verify error");
			break;
		}
	}
	if (tempp == sizeof(Master_Buf)){
		_DBG_("Verify successfully");
	}
	I2C_DeInit(I2CDEV_M);
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
