/**********************************************************************
* $Id$		EXTINT_Sleep.c 				2010-05-21
*//**
* @file		EXTINT_Sleep.c
* @brief	This example describes how to enter the system in sleep
* 			mode and wake-up by using external interrupt
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
#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_libcfg.h"
#include "debug_frmwrk.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup PWR_EXTINT_Sleep	EXTINT_Sleep
 * @ingroup PWR_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
//#define MCB_LPC_1768
#define IAR_LPC_1768

#ifdef MCB_LPC_1768
/* LED pin in byte style on P1 */
#define _EXT_IRQ	EINT0_IRQn
#define _EXTINT		EXTI_EINT0
#elif defined (IAR_LPC_1768)
#define _EXT_IRQ	EINT2_IRQn
#define _EXTINT		EXTI_EINT2
#endif

/************************** PRIVATE FUNCTIONS *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"Power control demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example used to enter system in sleep mode and wake up it by using external \n\r"
	"interrupt\n\r"
	"********************************************************************************\n\r";

/* Interrupt service routines */
#ifdef MCB_LPC_1768
void EINT0_IRQHandler(void);
#elif defined (IAR_LPC_1768)
void EINT2_IRQHandler(void);
#endif

void print_menu(void);
void delay (void);
void InitLED(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
#ifdef MCB_LPC_1768
/*********************************************************************//**
 * @brief		External interrupt 0 handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void EINT0_IRQHandler(void)
{
	  //clear the EINT0 flag
	  EXTI_ClearEXTIFlag(0);

}
#elif defined (IAR_LPC_1768)
/*********************************************************************//**
 * @brief		External interrupt 3 handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void EINT2_IRQHandler(void)
{
	  //clear the EINT2 flag
	  EXTI_ClearEXTIFlag(2);
}

#endif

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
/*********************************************************************//**
 * @brief		Delay function
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void delay (void) {
  unsigned int i;

  for (i = 0; i < 0x100000; i++) {
  }
}

/*********************************************************************//**
 * @brief		Initialize LEDs
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void InitLED(void)
{
#ifdef MCB_LPC_1768
/* Use LEDs P1.28 and P1.29*/
	GPIO_SetDir(1, (1<<28)|(1<<29), 1); //set P1.28 and P1.29 is output
	GPIO_ClearValue(1, (1<<28)|(1<<29));//Turn off LEDs
#elif defined (IAR_LPC_1768)
/* Use LED1 (P1.25) and LED2 (P0.4)*/
	GPIO_SetDir(1, (1<<25), 1); //set P1.25 is output
	GPIO_SetDir(0, (1<<4), 1);  //set P0.4 is output
	GPIO_SetValue(1, (1<<25));  //Turn off LED1
	GPIO_SetValue(0, (1<<4));	//Turn off LED2
#endif
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
	EXTI_InitTypeDef EXTICfg;

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

	/* Initialize LEDs
	 * - If using MCB1700 board:
	 * 		LEDs: P1.28 and P1.29 are available
	 * - If using IAR1700 board:
	 * 		LEDs: LED1(P1.25) and LED2(P0.4) are available
	 * Turn off LEDs after initialize
	 */
	InitLED();

	/* Initialize EXT pin and registers
	 * - If using MCB1700 board: EXTI0 is configured
	 * - If using IAR1700 board: EXTI2 is configured
	 */
#ifdef MCB_LPC_1768
	/* P2.10 as /EINT0 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
#elif defined (IAR_LPC_1768)
	/* P2.12 as /EINT2 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 12;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
#endif

	EXTI_Init();

	EXTICfg.EXTI_Line = _EXTINT;
	/* edge sensitive */
	EXTICfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	EXTICfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_ClearEXTIFlag(_EXTINT);
	EXTI_Config(&EXTICfg);

	NVIC_SetPriorityGrouping(4);
	NVIC_SetPriority(_EXT_IRQ, 0);
	NVIC_EnableIRQ(_EXT_IRQ);

	_DBG_("First LED is blinking in normal mode...\n\r"	\
		  "Press '1' to enter system in sleep mode.\n\r"\
		  "If you want to wake-up the system, press INT/WAKE-UP button.");
	while(_DG !='1')
	{
		//Blink first LED
#ifdef MCB_LPC_1768
		//blink LED P1.28
		GPIO_SetValue(1, (1<<28));
		delay();
		GPIO_ClearValue(1, (1<<28));
		delay();
#elif defined (IAR_LPC_1768)
		//blink LED1 (P1.25)
		GPIO_SetValue(1, (1<<25));
		delay();
		GPIO_ClearValue(1, (1<<25));
		delay();
#endif
	}

	_DBG_("Sleeping...");
	// Enter target power down mode
	CLKPWR_Sleep();

	// MCU will be here after waking up
	_DBG_("System wake-up! Second LED is blinking...");
	//turn off first LED
#ifdef MCB_LPC_1768
	GPIO_ClearValue(1, (1<<29));
#elif defined (IAR_LPC_1768)
	GPIO_SetValue(1, (1<<25));
#endif
	while (1)
	{
		//Blink second LED
#ifdef MCB_LPC_1768
		//blink LED P1.29
		GPIO_SetValue(1, (1<<29));
		delay();
		GPIO_ClearValue(1, (1<<29));
		delay();
#elif defined (IAR_LPC_1768)
		//blink LED2 (P0.4)
		GPIO_SetValue(0, (1<<4));
		delay();
		GPIO_ClearValue(0, (1<<4));
		delay();
#endif
	}
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
