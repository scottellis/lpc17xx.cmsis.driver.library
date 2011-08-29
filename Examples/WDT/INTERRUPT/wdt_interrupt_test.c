/**********************************************************************
* $Id$		wdt_interrupt_test.c			2010-05-21
*//**
* @file		wdt_interrupt_test.c
* @brief	This example describes how to use Watch-dog timer application
*           in interrupt mode
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
/** @defgroup WDT_INTERRUPT	INTERRUPT
 * @ingroup WDT_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

//Watchodog time out in 5 seconds
#define WDT_TIMEOUT 	5000000


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" Watch dog timer interrupt (test or debug mode) demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: Cortex M3 \n\r"
"\t - Communicate via: UART0 -  115200 bps \n\r"
" Use WDT with Internal RC OSC, interrupt mode (test only), timeout = 1 seconds \n\r"
" To generate an interrupt, after interrupt WDT interrupt is disabled immediately! \n\r"
"********************************************************************************\n\r";
uint8_t info1[] = "BEFORE WDT interrupt!\n\r";
uint8_t info2[] = "AFTER WDT interrupt\n\r";

__IO Bool wdt_flag = FALSE;
__IO Bool LED_toggle = FALSE;

/************************** PRIVATE FUNCTION *************************/
void WDT_IRQHandler(void);

void print_menu(void);
void LED_Init (void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		WDT interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void WDT_IRQHandler(void)
{
	// Disable WDT interrupt
	NVIC_DisableIRQ(WDT_IRQn);
	// Set WDT flag according
	if (wdt_flag == TRUE)
		wdt_flag = FALSE;
	else
		wdt_flag = TRUE;
	// Clear TimeOut flag
	WDT_ClrTimeOutFlag();
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print menu
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}

/*********************************************************************//**
 * @brief		Initialize LEDs
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void LED_Init (void)
{
#ifdef MCB_LPC_1768 /* Using LED2.2 for testing */
	//turn on LED2.2
	FIO_SetDir(2,(1<<2),1);
	FIO_SetValue(2,(1<<2));
#elif defined(IAR_LPC_1768) /* Using LED1 (P1.25 for testing */
	FIO_SetDir(1,(1<<25),1);
	FIO_ClearValue(1,(1<<25));
#endif
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main WDT program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	uint32_t delay;
	// Init LED port
	LED_Init();

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

	/* Install interrupt for WDT interrupt */
	NVIC_SetPriority(WDT_IRQn, 0x10);
	// Set Watchdog use internal RC, just generate interrupt only in 5ms if Watchdog is not feed

	// Init WDT, IRC OSC, interrupt mode, timeout = 5000000 us = 5s
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY);

	/* Enable the Watch dog interrupt*/
	NVIC_EnableIRQ(WDT_IRQn);

	while (1)
	{
		if (wdt_flag == FALSE){ //before WDT interrupt
			_DBG_(info1);
			_DBG_("Press '1' to enable Watchdog timer...");
			while(_DG !='1');
			// Start watchdog with timeout given
			WDT_Start(WDT_TIMEOUT);
			while(wdt_flag == FALSE)
			{
				_DBD32(WDT_GetCurrentCount()); _DBG_("");
			}
		} else { // after WDT interrupt
			_DBG_(info2);
			_DBG_("LED is blinking...");
			while(wdt_flag == TRUE)
			{
				if (LED_toggle == FALSE)
				{
#ifdef MCB_LPC_1768
					//turn on LED
					GPIO_SetValue(2,(1<<2));
#elif defined(IAR_LPC_1768)
					GPIO_ClearValue(1,(1<<25));
#endif
					LED_toggle = TRUE;
				}
				else
				{
#ifdef MCB_LPC_1768
					//turn off LED
					GPIO_ClearValue(2,(1<<2));
#elif defined(IAR_LPC_1768)
					GPIO_SetValue(1,(1<<25));
#endif
					LED_toggle = FALSE;
				}
				//delay
				for(delay = 0; delay<1000000; delay ++);
			}
		}
	}
	return 1;
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
