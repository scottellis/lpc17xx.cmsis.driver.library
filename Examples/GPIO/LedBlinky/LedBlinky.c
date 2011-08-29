/**********************************************************************
* $Id$		LedBlinky.c  				2010-05-21
*//**
* @file		LedBlinky.c
* @brief	This example describes how to use GPIO interrupt to drive LEDs
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
#include "lpc17xx_gpio.h"
#include "lpc17xx_libcfg.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup GPIO_LedBlinky	LedBlinky
 * @ingroup GPIO_Examples
 * @{
 */


/************************** PRIVATE DEFINITIONS *************************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

#ifdef MCB_LPC_1768
 /* Number of user LEDs */
#define LED_NUM     8
const unsigned long led_mask[] = { 1<<28, 1<<29, 1UL<<31, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6 };
#endif

/************************** PRIVATE VARIABLES *************************/
/* SysTick Counter */
volatile unsigned long SysTickCnt;

/************************** PRIVATE FUNCTIONS *************************/
void SysTick_Handler (void);

void Delay (unsigned long tick);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		SysTick handler sub-routine (1ms)
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void SysTick_Handler (void) {
  SysTickCnt++;
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Delay function
 * @param[in]	tick - number milisecond of delay time
 * @return 		None
 **********************************************************************/
void Delay (unsigned long tick) {
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
  while ((SysTickCnt - systickcnt) < tick);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void) {                       /* Main Program                       */
  int num = -1;
  int dir =  1;

  SysTick_Config(SystemCoreClock/1000 - 1); /* Generate interrupt each 1 ms   */

#ifdef MCB_LPC_1768
  GPIO_SetDir(1, 0xB0000000, 1);           /* LEDs on PORT1 defined as Output    */
  GPIO_SetDir(2, 0x0000007C, 1);           /* LEDs on PORT2 defined as Output    */

  GPIO_ClearValue(1, 0xB0000000);
  GPIO_ClearValue(2, 0x0000007C);
#elif defined(IAR_LPC_1768)
  GPIO_SetDir(1, (1<<25), 1);
  GPIO_ClearValue(1, (1<<25));
#endif


  for (;;) {                            /* Loop forever                       */
#ifdef MCB_LPC_1768
    /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...             */
    num += dir;
    if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; }
    else if   (num < 0) { dir =  1; num =  0;         }

    if (num < 3){
        GPIO_SetValue(1, led_mask[num]);
        Delay(500);                         /* Delay 500ms                        */
        GPIO_ClearValue(1, led_mask[num]);
        Delay(500);                         /* Delay 500ms                        */
    }
    else {
        GPIO_SetValue(2, led_mask[num]);
        Delay(500);                         /* Delay 500ms                        */
        GPIO_ClearValue(2, led_mask[num]);
        Delay(500);                         /* Delay 500ms                        */
    }
#elif defined(IAR_LPC_1768)
  GPIO_SetValue(1,(1<<25));
  Delay(500);
  GPIO_ClearValue(1,(1<<25));
  Delay(500);
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
