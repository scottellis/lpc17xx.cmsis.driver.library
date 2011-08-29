/**********************************************************************
* $Id$		slave.c  				2010-05-21
*//**
* @file		slave.c
* @brief	This example describes how to use I2C peripheral as a slave
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
/** @defgroup I2C_slave	slave
 * @ingroup I2C_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device as slave definition */
#define USEDI2CDEV_S		0
/** Own Slave address in Slave I2C device */
#define I2CDEV_S_OWN_ADDR	(0x90>>1)

/** Max buffer length */
#define BUFFER_SIZE			0x10

#if (USEDI2CDEV_S == 0)
#define I2CDEV_S LPC_I2C0
#elif (USEDI2CDEV_S == 2)
#define I2CDEV_S LPC_I2C2
#else
#error "Slave I2C device not defined!"
#endif

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"I2C demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - This example uses I2C as slave device to transfer data \n\r"
" to/from I2C master device \n\r"
"********************************************************************************\n\r";

/** These global variables below used in interrupt mode - Slave device -----------*/
uint8_t Slave_Buf[BUFFER_SIZE];
uint8_t slave_test[2];

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void Buffer_Init(uint8_t type);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
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
 *
 * @return 		None
 **********************************************************************/
void Buffer_Init(uint8_t type)
{
	uint8_t i;

	if (type)
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Slave_Buf[i] = i;
		}
	}
	else
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Slave_Buf[i] = 0;
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
	I2C_OWNSLAVEADDR_CFG_Type OwnSlavAdr;
	I2C_S_SETUP_Type transferSCfg;
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
#if ((USEDI2CDEV_S == 0))
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
#endif
#if ((USEDI2CDEV_S == 2))
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#endif

	// Initialize Slave I2C peripheral
	I2C_Init(I2CDEV_S, 100000);

	/* Set  Own slave address for I2C device */
	OwnSlavAdr.GeneralCallState = ENABLE;
	OwnSlavAdr.SlaveAddrChannel= 0;
	OwnSlavAdr.SlaveAddrMaskValue = 0xFF;
	OwnSlavAdr.SlaveAddr_7bit = I2CDEV_S_OWN_ADDR;
	I2C_SetOwnSlaveAddr(I2CDEV_S, &OwnSlavAdr);

	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_S, ENABLE);

	_DBG_("Press '1' to start");
	while (_DG != '1');


	/* Reading -------------------------------------------------------- */
	_DBG_("Start Reading...");

	/* Initialize buffer */
	Buffer_Init(0);

	/* Start I2C slave device first */
	transferSCfg.tx_data = NULL;
	transferSCfg.tx_length = 0;
	transferSCfg.rx_data = Slave_Buf;
	transferSCfg.rx_length = sizeof(Slave_Buf);
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_POLLING);

	pdat = Slave_Buf;
	// Verify
	for (tempp = 0; tempp < sizeof(Slave_Buf); tempp++){
		if (*pdat++ != tempp){
			_DBG_("Verify error");
			break;
		}
	}
	if (tempp == sizeof(Slave_Buf)){
		_DBG_("Verify successfully");
	}

	/* Transmit -------------------------------------------------------- */
	_DBG_("Start Transmit...");

	/* Initialize buffer */
	Buffer_Init(1);

	/* Start I2C slave device first */
	transferSCfg.tx_data = Slave_Buf;
	transferSCfg.tx_length = sizeof(Slave_Buf);
	transferSCfg.rx_data = NULL;
	transferSCfg.rx_length = 0;
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_POLLING);

	_DBG_("Complete!");

#if 1
	/* Receive and transmit -------------------------------------------------------- */
	_DBG_("Start Receive, wait for repeat start and transmit...");

	/* Initialize buffer */
	Buffer_Init(1);
	slave_test[0] = 0xAA;
	slave_test[1] = 0x55;

	/* Start I2C slave device first */
	transferSCfg.tx_data = Slave_Buf;
	transferSCfg.tx_length = sizeof(Slave_Buf);
	transferSCfg.rx_data = slave_test;
	transferSCfg.rx_length = sizeof(slave_test);
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_POLLING);
	_DBG_("Receive Data:");
	_DBH(slave_test[0]); _DBG_("");
	_DBH(slave_test[1]); _DBG_("");
	_DBG_("Complete!");
#endif


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
