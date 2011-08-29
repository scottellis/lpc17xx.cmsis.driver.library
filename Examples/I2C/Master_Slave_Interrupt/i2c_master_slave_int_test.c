/**********************************************************************
* $Id$		i2c_master_slave_int_test.c  				2010-05-21
*//**
* @file		i2c_master_slave_int_test.c
* @brief	This example describes how to uses two I2C peripheral on LPC1768
* 			to communicate together. One set as master and the other set as slave.
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
/** @defgroup I2C_Master_Slave_Interrupt	Master_Slave_Interrupt
 * @ingroup I2C_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device as master definition */
#define USEDI2CDEV_M		2

/** Used I2C device as slave definition */
#define USEDI2CDEV_S		0
/** Own Slave address in Slave I2C device */
#define I2CDEV_S_OWN_ADDR	(0x90>>1)

/** Max buffer length */
#define BUFFER_SIZE			0x10


#if (USEDI2CDEV_M == USEDI2CDEV_S)
#error "Master and Slave I2C device are duplicated!"
#endif

#if (USEDI2CDEV_M == 0)
#define I2CDEV_M LPC_I2C0
#elif (USEDI2CDEV_M == 2)
#define I2CDEV_M LPC_I2C2
#else
#error "Master I2C device not defined!"
#endif


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
"\t - This example uses two I2C peripheral, one set as master and \n\r"
"\t the other set as slave \n\r"
"\t First, the master send 16 bytes of data to slave \n\r"
"\t Then, the master receive 16 bytes of data from slave \n\r"
"\t Both of them used in interrupt mode \n\r"
"********************************************************************************\n\r";

/** These global variables below used in interrupt mode - Master device -----------*/
__IO FlagStatus complete_M;
uint8_t Master_Buf[BUFFER_SIZE];


/** These global variables below used in interrupt mode - Slave device ------------*/
__IO FlagStatus complete_S;
uint8_t Slave_Buf[BUFFER_SIZE];


uint8_t master_test[2];
uint8_t slave_test[2];

/************************** PRIVATE FUNCTIONS *************************/
/* Interrupt service routines */
#if ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0))
void I2C0_IRQHandler(void);
#endif
#if ((USEDI2CDEV_M == 2) || (USEDI2CDEV_S == 2))
void I2C2_IRQHandler(void);
#endif

void print_menu(void);
void Error_Loop_M(uint8_t ErrorCode);
void Error_Loop_S(uint8_t ErrorCode);


#if ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0))
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		Main I2C0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void I2C0_IRQHandler(void)
{
#if (USEDI2CDEV_M == 0)
	I2C_MasterHandler(I2CDEV_M);
	if (I2C_MasterTransferComplete(I2CDEV_M)){
		complete_M = SET;
	}
#endif

#if (USEDI2CDEV_S == 0)
	I2C_SlaveHandler(I2CDEV_S);
	if (I2C_SlaveTransferComplete(I2CDEV_S)){
		complete_S = SET;
	}
#endif
}
#endif /* ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0)) */


#if ((USEDI2CDEV_M == 2) || (USEDI2CDEV_S == 2))

/*********************************************************************//**
 * @brief 		Main I2C2 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void I2C2_IRQHandler(void)
{
#if (USEDI2CDEV_M == 2)
	I2C_MasterHandler(I2CDEV_M);
	if (I2C_MasterTransferComplete(I2CDEV_M)){
		complete_M = SET;
	}
#endif
#if (USEDI2CDEV_S == 2)
	I2C_SlaveHandler(I2CDEV_S);
	if (I2C_SlaveTransferComplete(I2CDEV_S)){
		complete_S = SET;
	}
#endif
}
#endif /* ((USEDI2CDEV_M == 2) || (USEDI2CDEV_S == 2)) */

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
			Master_Buf[i] = 0;
			Slave_Buf[i] = i;
		}
	}
	else
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Master_Buf[i] = i;
			Slave_Buf[i] = 0;
		}
	}
}

/*********************************************************************//**
 * @brief		A subroutine that will be called if there's any error
 * 				on I2C operation (master)
 * @param[in]	ErrorCode Error Code Input
 * @return 		None
 **********************************************************************/
void Error_Loop_M(uint8_t ErrorCode)
{
	/*
	 * Insert your code here...
	 */
	while(1);
}

/*********************************************************************//**
 * @brief		A subroutine that will be called if there's any error
 * 				on I2C operation (slave)
 * @param[in]	ErrorCode Error Code Input
 * @return 		None
 **********************************************************************/
void Error_Loop_S(uint8_t ErrorCode)
{
	/*
	 * Insert your code here...
	 */
	while(1);
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
	I2C_M_SETUP_Type transferMCfg;
	I2C_S_SETUP_Type transferSCfg;
	uint32_t tempp;
	uint8_t *sp, *dp;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	//print menu screen
	print_menu();


	/* I2C block ------------------------------------------------------------------- */

	/*
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
#if ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0))
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
#endif
#if ((USEDI2CDEV_M == 2) || (USEDI2CDEV_S == 2))
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#endif

	// Initialize Master I2C peripheral
	I2C_Init(I2CDEV_M, 100000);
	// Initialize Slave I2C peripheral
	I2C_Init(I2CDEV_S, 100000);

	/* Set  Own slave address for I2C device */
	OwnSlavAdr.GeneralCallState = ENABLE;
	OwnSlavAdr.SlaveAddrChannel= 0;
	OwnSlavAdr.SlaveAddrMaskValue = 0xFF;
	OwnSlavAdr.SlaveAddr_7bit = I2CDEV_S_OWN_ADDR;
	I2C_SetOwnSlaveAddr(I2CDEV_S, &OwnSlavAdr);

	/*
	 * Note: The master should be set higher priority than
	 * the slave that let interrupt in master can appear
	 * in slave's timeout condition.
	 * A higher interrupt priority has lower number level!!!
	 */

	/* Configure interrupt for I2C in NVIC of ARM core */
#if ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0))
    /* Disable I2C0 interrupt */
    NVIC_DisableIRQ(I2C0_IRQn);
#if (USEDI2CDEV_M == 0)
    /* preemption = 1, sub-priority = 0 */
    NVIC_SetPriority(I2C0_IRQn, ((0x00<<3)|0x01));
#else
    /* preemption = 1, sub-priority = 2 */
    NVIC_SetPriority(I2C0_IRQn, ((0x02<<3)|0x01));
#endif
#endif /* ((USEDI2CDEV_M == 0) || (USEDI2CDEV_S == 0)) */

#if ((USEDI2CDEV_M == 2) || (USEDI2CDEV_S == 2))
    /* Disable I2C2 interrupt */
    NVIC_DisableIRQ(I2C2_IRQn);
#if (USEDI2CDEV_M == 2)
    /* preemption = 1, sub-priority = 0 */
    NVIC_SetPriority(I2C2_IRQn, ((0x00<<3)|0x01));
#else
    /* preemption = 1, sub-priority = 2 */
    NVIC_SetPriority(I2C2_IRQn, ((0x02<<3)|0x01));
#endif
#endif


	/* Enable Master I2C operation */
	I2C_Cmd(I2CDEV_M, ENABLE);
	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_S, ENABLE);


#if 1
	/* MASTER SEND DATA TO SLAVE -------------------------------------------------------- */
	/* Force complete flag for the first time of running */
	complete_S =  RESET;
	complete_M =  RESET;

	_DBG_("Master transmit data to slave...");

	/* Initialize buffer */
	Buffer_Init(0);

	/* Start I2C slave device first */
	transferSCfg.tx_data = NULL;
	transferSCfg.tx_length = 0;
	transferSCfg.rx_data = Slave_Buf;
	transferSCfg.rx_length = sizeof(Slave_Buf);
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_INTERRUPT);

	/* Then start I2C master device */
	transferMCfg.sl_addr7bit = I2CDEV_S_OWN_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = sizeof(Master_Buf);
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_INTERRUPT);

	/* Wait until both of them complete */
	while ((complete_M == RESET) || (complete_S == RESET));
	// verify data
	sp = Master_Buf;
	dp = Slave_Buf;
	for (tempp = sizeof(Master_Buf); tempp; tempp--){
		if(*sp++ != *dp++){
			_DBG_("Verify data error!");
		}
	}
	// Success!
	if (tempp == 0){
		_DBG_("Verify data successfully!");
	}


	/* MASTER RECEIVE DATA FROM SLAVE -------------------------------------------------------- */
	/* Force complete flag for the first time of running */
	complete_S =  RESET;
	complete_M =  RESET;

	_DBG_("Master read data from slave...");

	/* Initialize buffer */
	Buffer_Init(1);

	/* Start I2C slave device first */
	transferSCfg.tx_data = Slave_Buf;
	transferSCfg.tx_length = sizeof(Slave_Buf);
	transferSCfg.rx_data = NULL;
	transferSCfg.rx_length = 0;
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_INTERRUPT);

	/* Then start I2C master device */
	transferMCfg.sl_addr7bit = I2CDEV_S_OWN_ADDR;
	transferMCfg.tx_data = NULL;
	transferMCfg.tx_length = 0;
	transferMCfg.rx_data = Master_Buf;
	transferMCfg.rx_length = sizeof(Master_Buf);
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_INTERRUPT);

	/* Wait until both of them complete */
	while ((complete_M == RESET) || (complete_S == RESET));
	// verify data
	sp = Master_Buf;
	dp = Slave_Buf;
	for (tempp = sizeof(Master_Buf); tempp; tempp--){
		if(*sp++ != *dp++){
			_DBG_("Verify data error!");
		}
	}
	// Success!
	if (tempp == 0){
		_DBG_("Verify data successfully!");
	}
#endif


#if 1
	// TEST-----------------------------------------------------------
	// Master transmit two bytes, then repeat start and reading from slave
	// a number of byte
	/* Force complete flag for the first time of running */
	complete_S =  RESET;
	complete_M =  RESET;

	_DBG_("Master transmit data to slave first, then repeat start and read data from slave...");
	/* Initialize buffer */
	Buffer_Init(1);
	master_test[0] = 0xAA;
	master_test[1] = 0x55;
	slave_test[0] = 0x00;
	slave_test[1] = 0x00;

	/* Start I2C slave device first */
	transferSCfg.tx_data = Slave_Buf;
	transferSCfg.tx_length = sizeof(Slave_Buf);
	transferSCfg.rx_data = slave_test;
	transferSCfg.rx_length = sizeof(slave_test);
	I2C_SlaveTransferData(I2CDEV_S, &transferSCfg, I2C_TRANSFER_INTERRUPT);

	/* Then start I2C master device */
	transferMCfg.sl_addr7bit = I2CDEV_S_OWN_ADDR;
	transferMCfg.tx_data = master_test;
	transferMCfg.tx_length = sizeof(master_test);
	transferMCfg.rx_data = Master_Buf;
	transferMCfg.rx_length = sizeof(Master_Buf);
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_INTERRUPT);

	/* Wait until both of them complete */
	while ((complete_M == RESET) || (complete_S == RESET));
	// verify data
	sp = Master_Buf;
	dp = Slave_Buf;
	for (tempp = sizeof(Master_Buf); tempp; tempp--){
		if(*sp++ != *dp++){
			_DBG_("Verify data error!");
		}
	}
	// Success!
	if (tempp == 0){
		_DBG_("Verify data successfully!");
	}
	sp = master_test;
	dp = slave_test;
	for (tempp = sizeof(master_test); tempp; tempp--){
		if(*sp++ != *dp++){
			_DBG_("Verify data error!");
		}
	}
	// Success!
	if (tempp == 0){
		_DBG_("Verify data successfully!");
	}
#endif

	while (1){
		tempp++;
	}

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
