/**********************************************************************
* $Id$		exti_deepsleep.c 				2010-06-18
*//**
* @file		exti_deepsleep.c
* @brief	This example describes how to enter the system in deep
* 			sleep mode and wake-up by using Watchdog timer interrupt
* @version	1.0
* @date		18. June. 2010
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
#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_libcfg.h"
#include "debug_frmwrk.h"
#include "lpc17xx_wdt.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup PWR_WDT_DeepSleep	WDT_DeepSleep
 * @ingroup PWR_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"Power control demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example used to enter system in deep sleep mode and wake up it by using \n\r "
	"Watchdog timer interrupt \n\r"
	"********************************************************************************\n\r";
//Watchodog time out in 2 seconds
#define WDT_TIMEOUT 	2000000

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void WDT_IRQHandler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
void WDT_IRQHandler(void)
{
	// Disable WDT interrupt
	NVIC_DisableIRQ(WDT_IRQn);
	WDT_ClrTimeOutFlag();
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void)
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
	print_menu();

	// Init WDT, IRC OSC, interrupt mode, timeout = 2000000 us = 2s
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY);

	_DBG_("Press '1' to enter system in Deep Sleep mode");
	while(_DG !='1');

	NVIC_EnableIRQ(WDT_IRQn);
	WDT_Start(WDT_TIMEOUT);

	_DBG_("Enter Deep Sleep mode!");
	_DBG_("Wait 2s for WDT wake-up system...");

	/*---------- Disable and disconnect the main PLL0 before enter into Deep-Sleep
	 * or Power-Down mode <according to errata.lpc1768-16.March.2010> ------------
	 */
	LPC_SC->PLL0CON &= ~(1<<1); /* Disconnect the main PLL (PLL0) */
	LPC_SC->PLL0FEED = 0xAA; /* Feed */
	LPC_SC->PLL0FEED = 0x55; /* Feed */
	while ((LPC_SC->PLL0STAT & (1<<25)) != 0x00); /* Wait for main PLL (PLL0) to disconnect */
	LPC_SC->PLL0CON &= ~(1<<0); /* Turn off the main PLL (PLL0) */
	LPC_SC->PLL0FEED = 0xAA; /* Feed */
	LPC_SC->PLL0FEED = 0x55; /* Feed */
	while ((LPC_SC->PLL0STAT & (1<<24)) != 0x00); /* Wait for main PLL (PLL0) to shut down */
	/*------------Then enter into PowerDown mode ----------------------------------*/

	// Enter target power down mode
	CLKPWR_DeepSleep();
	SystemInit();
	debug_frmwrk_init();
	_DBG_("\n\rSystem wake-up!\n\r");
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
