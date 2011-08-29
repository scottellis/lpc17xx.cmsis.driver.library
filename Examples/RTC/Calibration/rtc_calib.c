/**********************************************************************
* $Id$		rtc_calib.c						2010-06-18
*//**
* @file		rtc_calib.c
* @brief	This example describes how to calibrate real time clock
* @version	2.0
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
#include "lpc17xx_rtc.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup RTC_Calibration	Calibration
 * @ingroup RTC_Examples
 * @{
 */


/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"RTC Calibration demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example describes how to calibrate RTC \n\r"
	"********************************************************************************\n\r";

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void RTC_IRQHandle(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		RTC interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void RTC_IRQHandler(void)
{
	uint32_t secval;

	/* This is increment counter interrupt*/
	if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
	{
		secval = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_SECOND);

		/* Send debug information */
		_DBG ("Second: "); _DBD(secval);
		_DBG_("");

		// Clear pending interrupt
		RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}
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

	/* In this example:
	 * Suppose that the RTC need periodically adjust after each 5 second.
	 * And the time counter need by incrementing the counter by 2 instead of 1
	 * We will observe timer counter after calibration via serial display
	 */
	// Init RTC module
	RTC_Init(LPC_RTC);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);

	//Set current time = 0
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);

	/* Setting Timer calibration
	 * Calibration value =  5s;
	 * Direction = Forward calibration
	 * So after each 5s, calibration logic can periodically adjust the time counter by
	 * incrementing the counter by 2 instead of 1
	 */
	RTC_CalibConfig(LPC_RTC, 5, RTC_CALIB_DIR_FORWARD);
	RTC_CalibCounterCmd(LPC_RTC, ENABLE);

	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

    /* Enable RTC interrupt */
    NVIC_EnableIRQ(RTC_IRQn);

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
