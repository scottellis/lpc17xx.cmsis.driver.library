/**********************************************************************
* $Id$		i2c_polling_test.c  				2010-05-21
*//**
* @file		i2c_polling_test.c
* @brief	An example of I2C using polling mode to test the I2C driver.
* 			Using I2C at mode I2C master/8bit on LPC1766 to communicate with
* 			SC16IS750/760 Demo Board
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
/** @defgroup I2C_sc16is750_polling	sc16is750_polling
 * @ingroup I2C_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device definition, should be 0 or 2 */
#define USEDI2CDEV	0

/* Definition of internal register of SC16IS750/760 */
#define IODIR		0x0A
#define IOSTATE		0x0B
#define IOCON		0x0E
#define SLVADDR		(0x90>>1)
#define REGS_ADDR(n) (n<<3)

#if (USEDI2CDEV == 0)
#define I2CDEV LPC_I2C0
#elif (USEDI2CDEV == 2)
#define I2CDEV LPC_I2C2
#else
#error "I2C device not defined!"
#endif

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"I2C demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115.2 kbps \n\r"
" An example of I2C using polling mode to test the I2C driver \n\r"
" Using I2C at mode I2C master/8bit on LPC1766 to communicate with \n\r"
" SC16IS750/760 Demo Board \n\r"
" Press '1' to turn ON, '2' to turn OFF LEDs \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";

/* Define array data with match data to set internal register value of SC16IS740/750/760 */
uint8_t iocon_cfg[2] = {REGS_ADDR(IOCON), 0x00};
uint8_t iodir_cfg[2] = {REGS_ADDR(IODIR), 0xFF};
uint8_t iostate_cfg_0[2] = {REGS_ADDR(IOSTATE), 0x00};
uint8_t iostate_cfg_1[2] = {REGS_ADDR(IOSTATE), 0xFF};


/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void Error_Loop(uint32_t ErrorCode);

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
 * @brief		A subroutine that will be called if there's any error
 * 				on I2C operation
 * @param[in]	ErrorCode Error Code Input
 * @return 		None
 **********************************************************************/
void Error_Loop(uint32_t ErrorCode)
{
	uint32_t test;

	// for testing purpose
	test = ErrorCode;
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
	uint8_t tmpchar[2] = {0, 0};
	__IO FlagStatus exitflag;
	PINSEL_CFG_Type PinCfg;
	I2C_M_SETUP_Type transferCfg;
	uint8_t SC16IS_RegStat;

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
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
#if (USEDI2CDEV == 0)
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
#elif (USEDI2CDEV == 2)
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#endif

	/* I2C block ------------------------------------------------------------------- */
	// Initialize I2C peripheral
	I2C_Init(I2CDEV, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(I2CDEV, ENABLE);

	/* Configure SC16IS750 ---------------------------------------------------------- */
	/* First, send some command to reset SC16IS740 chip via I2C bus interface */
	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iocon_cfg;
	transferCfg.tx_length = sizeof(iocon_cfg);
	transferCfg.rx_data = NULL;
	transferCfg.rx_length = 0;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}

	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iodir_cfg;
	transferCfg.tx_length = sizeof(iodir_cfg);
	transferCfg.rx_data = NULL;
	transferCfg.rx_length = 0;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}

	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iostate_cfg_0;
	transferCfg.tx_length = sizeof(iostate_cfg_0);
	transferCfg.rx_data = NULL;
	transferCfg.rx_length = 0;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}


	/* Validate value of SC16IS750 register ------------------------------------------ */
	/* This section will dump out value of register that set through I2C bus */

	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iocon_cfg;
	transferCfg.tx_length = 1;
	transferCfg.rx_data = (uint8_t *)&SC16IS_RegStat;
	transferCfg.rx_length = 1;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}

	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iodir_cfg;
	transferCfg.tx_length = 1;
	transferCfg.rx_data = (uint8_t *)&SC16IS_RegStat;
	transferCfg.rx_length = 1;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}

	transferCfg.sl_addr7bit = SLVADDR;
	transferCfg.tx_data = (uint8_t *)iostate_cfg_0;
	transferCfg.tx_length = 1;
	transferCfg.rx_data = (uint8_t *)&SC16IS_RegStat;
	transferCfg.rx_length = 1;
	transferCfg.retransmissions_max = 2;
	if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
			== ERROR){
		Error_Loop(transferCfg.status);
	}

	// Reset exit flag
	exitflag = RESET;

    /* Read some data from the buffer */
    while (exitflag == RESET){

    	while((tmpchar[0] = _DG) == 0);

    	if (tmpchar[0] == 27){
			/* ESC key, set exit flag */
			_DBG_(menu2);
			exitflag = SET;
		}
		else if (tmpchar[0] == 'r'){
			print_menu();
		}
		else{
			if (tmpchar[0] == '1'){
				// LEDs are ON now...
				transferCfg.sl_addr7bit = SLVADDR;
				transferCfg.tx_data = (uint8_t *)iostate_cfg_0;
				transferCfg.tx_length = sizeof(iostate_cfg_0);
				transferCfg.rx_data = NULL;
				transferCfg.rx_length = 0;
				transferCfg.retransmissions_max = 2;
				if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
						== ERROR){
					Error_Loop(transferCfg.status);
				}
			}
			else if (tmpchar[0] == '2')
			{
				// LEDs are OFF now...
				transferCfg.sl_addr7bit = SLVADDR;
				transferCfg.tx_data = (uint8_t *)iostate_cfg_1;
				transferCfg.tx_length = sizeof(iostate_cfg_1);
				transferCfg.rx_data = NULL;
				transferCfg.rx_length = 0;
				transferCfg.retransmissions_max = 2;
				if (I2C_MasterTransferData(I2CDEV, &transferCfg, I2C_TRANSFER_POLLING) \
						== ERROR){
					Error_Loop(transferCfg.status);
				}
			}
			/* Then Echo it back */
			_DBG_(tmpchar);
		}
    }

    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy(LPC_UART0)==SET);

    // DeInitialize UART0 peripheral
    UART_DeInit(LPC_UART0);
    I2C_DeInit(I2CDEV);
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
