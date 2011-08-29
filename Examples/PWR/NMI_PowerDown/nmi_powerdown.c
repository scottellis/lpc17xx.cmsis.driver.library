/**********************************************************************
* $Id$		nmi_powerdown.c 				2010-06-18
*//**
* @file		nmi_powerdown.c
* @brief	This example describes how to enter the system in Power
* 			Down mode and wake-up by using NMI (Non-maskable Interrupt)
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
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup PWR_NMI_PowerDown	NMI_PowerDown
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
	"This example used to enter system in PowerDown mode and wake up it by using \n\r "
	"NMI (Non-Maskable Interrupt) \n\r"
	"********************************************************************************\n\r";
void print_menu(void);
void NMI_Handler(void);


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		Non-Maskable interrupt handler
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void NMI_Handler(void)
{
	//do nothing
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

	/* Pin selection:
	 * P2.10 as NMI
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);

	SCB->ICSR |=(1<<31); 	//changes NMI exception state to pending

	_DBG_("Press '1' to enter system in PowerDown mode");
	while(_DG !='1');

	_DBG_("Enter PowerDown mode...");
	_DBG_("Press INT0 button to wake-up system");

	// Enter target power down mode

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

	CLKPWR_PowerDown();
	SystemInit();
	debug_frmwrk_init();
	_DBG_("\n\rSystem waked-up!");
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
