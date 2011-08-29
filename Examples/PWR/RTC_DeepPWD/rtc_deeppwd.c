/**********************************************************************
* $Id$		rtc_deeppwd.c 				2010-06-18
*//**
* @file		rtc_deeppwd.c
* @brief	This example describes how to enter the system in Deep
* 			PowerDown and wake-up by using RTC Interrupt
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
#include "lpc17xx_rtc.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup PWR_RTC_DeepPWD	RTC_DeepPWD
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
	"This example used to enter system in Deep PowerDown mode and wake up it by\n\r "
	"using RTC Interrupt \n\r"
	"********************************************************************************\n\r";

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void RTC_IRQHandler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		RTC(Real-time clock) interrupt handler
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void RTC_IRQHandler(void)
{
	if(RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM))
	{
		RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
		RTC_Cmd(LPC_RTC, DISABLE);
		NVIC_DisableIRQ(RTC_IRQn);
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

	/* Initialize and configure RTC */
	RTC_Init(LPC_RTC);

	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);

	/* Set alarm time = 5s.
	 * So, after each 5s, RTC will generate and wake-up system
	 * out of Deep PowerDown mode.
	 */
	RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, 5);

	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, DISABLE);
	/* Set the AMR for 5s match alarm interrupt */
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);

	_DBG_("Press '1' to enter system in Deep PowerDown mode");
	while(_DG !='1');

	RTC_Cmd(LPC_RTC, ENABLE);
	NVIC_EnableIRQ(RTC_IRQn);

	_DBG_("Enter Deep PowerDown mode...");
	_DBG_("Wait 5s, RTC will wake-up system...\n\r");

	// Enter target power down mode
	CLKPWR_DeepPowerDown();

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
