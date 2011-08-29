/**********************************************************************
* $Id$		rit_interrupt.c						2010-05-21
*//**
* @file		rit_interrupt.c
* @brief	This example used RIT to generate interrupt each 1s
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
#include "lpc17xx_rit.h"
#include "lpc17xx_libcfg.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup RIT_Interrupt	Interrupt
 * @ingroup RIT_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

#define TIME_INTERVAL 	1000

/************************** PRIVATE VARIABLE ***********************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	" RIT demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	" Use RIT as a timer to generate interrupt to turn on/off LED each 1s \n\r"
	"********************************************************************************\n\r";
FunctionalState LEDStatus = ENABLE;

/************************** PRIVATE FUNCTION *************************/
void RIT_IRQHandler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		RIT interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void RIT_IRQHandler(void)
{
	RIT_GetIntStatus(LPC_RIT); //call this to clear interrupt flag
	if(LEDStatus == ENABLE)
	{
		LEDStatus = DISABLE;
#ifdef MCB_LPC_1768
		//turn off LED
		GPIO_ClearValue(2,(1<<2));
#elif defined(IAR_LPC_1768)
		GPIO_SetValue(1,(1<<25));
#endif
	}
	else
	{
		LEDStatus = ENABLE;
#ifdef MCB_LPC_1768
		//turn off LED
		GPIO_SetValue(2,(1<<2));
#elif defined(IAR_LPC_1768)
		GPIO_ClearValue(1,(1<<25));
#endif
	}
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main RIT program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void) {

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	 _DBG(menu);

	RIT_Init(LPC_RIT);
	/* Configure time_interval for RIT
	 * In this case: time_interval = 1000 ms = 1s
	 * So, RIT will generate interrupt each 1s
	 */
	RIT_TimerConfig(LPC_RIT,TIME_INTERVAL);

	_DBG("The time interval is: ");
	_DBD32(TIME_INTERVAL); _DBG_(" millisecond..");

#ifdef MCB_LPC_1768 /* Using LED2.2 for testing */
	//turn on LED2.2
	FIO_SetDir(2,(1<<2),1);
	FIO_SetValue(2,(1<<2));
#elif defined(IAR_LPC_1768) /* Using LED1 (P1.25 for testing */
	FIO_SetDir(1,(1<<25),1);
	FIO_ClearValue(1,(1<<25));
#endif
	NVIC_EnableIRQ(RIT_IRQn);

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
