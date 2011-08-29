/**********************************************************************
* $Id$		wdt_reset_test.c			2010-05-21
*//**
* @file		wdt_reset_test.c
* @brief	This example describes how to use WDT in reset mode
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
#include "lpc17xx_wdt.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup WDT_RESET	RESET
 * @ingroup WDT_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
//#define MCB_LPC_1768
#define IAR_LPC_1768

//Watchodog time out in 5 seconds
#define WDT_TIMEOUT 	5000000


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"This Welcome Screen below will executive after reset event \n\r"
"Hello NXP Semiconductors \n\r"
" Watch dog timer reset when timeout demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: Cortex M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use WDT with Internal RC OSC, reset mode, timeout = 5 seconds \n\r"
" To reset MCU when time out. After reset, program will determine what cause of "
" last reset time (external reset or WDT time-out)\n\r"
"********************************************************************************\n\r";
uint8_t info1[] = "Last MCU reset caused by WDT TimeOut!\n\r";
uint8_t info2[] = "Last MCU reset caused by External!\n\r";

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main WDT program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	// print welcome screen
	_DBG(menu1);

	// Read back TimeOut flag to determine previous timeout reset
	if (WDT_ReadTimeOutFlag()){
		_DBG_(info1);
		// Clear WDT TimeOut
		WDT_ClrTimeOutFlag();
	} else{
		_DBG_(info2);
	}

	// Initialize WDT, IRC OSC, interrupt mode, timeout = 5000000us = 5s
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_RESET);
	// Start watchdog with timeout given
	WDT_Start(WDT_TIMEOUT);

	//infinite loop to wait chip reset from WDT
	while(1);

	return 0;
}

/* Support required entry point for other toolchain */
int main (void)
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
