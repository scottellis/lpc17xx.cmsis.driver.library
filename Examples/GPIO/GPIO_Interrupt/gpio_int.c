/**********************************************************************
* $Id$		gpio_int.c  				2010-05-21
*//**
* @file		gpio_int.c
* @brief	This example used to test GPIO interrupt function
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
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup GPIO_Interrupt	GPIO_Interrupt
 * @ingroup GPIO_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

#ifdef MCB_LPC_1768
/* LED pin in byte style on P1 */
#define POLL_LED	(1<<4)		// P1.28
#define INT3_LED	(1<<5)		// P1.29
#define GPIO_INT	(1<<25)		// test GPIO interrupt on P0.25
#elif defined(IAR_LPC_1768)
#define POLL_LED	(1<<1)		//P1.25 (LED2)
#define INT3_LED	(1<<4)		//P0.4 	(LED1)
#define GPIO_INT	(1<<23) 	// test GPIO interrupt on P0.23
#endif


/************************** PRIVATE FUNCTIONS *************************/
void EINT3_IRQHandler(void);

void delay (void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		External interrupt 3 handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void EINT3_IRQHandler(void)
{
	      int j;
	      if(GPIO_GetIntStatus(0, 25, 1))
		  {
	    	  GPIO_ClearInt(0,(1<<25));
			  for (j= 0; j<8; j++)
			  {
#ifdef MCB_LPC_1768
				  /* Use MCB1700 board:
				   * blink LED P1.29 when EINT3 occur
				   */
				    FIO_ByteSetValue(1, 3, INT3_LED);
					delay();
					FIO_ByteClearValue(1, 3, INT3_LED);
				    delay();
#elif defined(IAR_LPC_1768)
				/* Use IAR LPC1768 KS board:
				 * blink LED2 P0.4 when EINT3 occur
				 */
				    FIO_ByteSetValue(0, 0, INT3_LED);
					delay();
					FIO_ByteClearValue(0, 0, INT3_LED);
				    delay();
#endif
			  }
          }
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
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


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void)
{
	/** Use: LEDs for polling when idle and when GPIO interrupt occurs
	 *  If using MCB1700 board: Polling led: P1.28; Interrupt led: P1.29
	 *  If using IAR KS board:  Polling led: P1.25 (LED1); Interrupt led: P0.4 (LED2)
	 */
#ifdef MCB_LPC_1768
	FIO_ByteSetDir(1, 3, POLL_LED, 1);
	FIO_ByteSetDir(1, 3, INT3_LED, 1);
	// Turn off all LEDs
	FIO_ByteClearValue(1, 3, POLL_LED);
	FIO_ByteClearValue(1, 3, INT3_LED);
#elif defined(IAR_LPC_1768)
	FIO_ByteSetDir(1, 3, POLL_LED, 1);
	FIO_ByteSetDir(0, 0, INT3_LED, 1);
	// Turn off all LEDs
	FIO_ByteSetValue(1, 3, POLL_LED);
	FIO_ByteSetValue(0, 0, INT3_LED);
#endif

	// Enable GPIO interrupt
	/* Use MCB1700: test GPIO interrupt on P0.25->connects with ADC potentiometer
	 * Use IAR KS : test GPIO interrupt on P0.23->connects with BUT1 button
	 */
	GPIO_IntCmd(0,(1<<25),1);
	NVIC_EnableIRQ(EINT3_IRQn);

	while (1)
	{
		//polling led while idle
		FIO_ByteSetValue(1, 3, POLL_LED);
		delay();
		FIO_ByteClearValue(1, 3, POLL_LED);
		delay();
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
