/**********************************************************************
* $Id$		nvic_priority.c 				2010-06-18
*//**
* @file		nvic_priority.c
* @brief	This example used to test NVIC Grouping Priority function
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
#include "lpc17xx_libcfg.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup NVIC_Priority	Priority
 * @ingroup NVIC_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS*************************/
/* Interrupt mode
 * - 0: Tail-chaining interrupt
 * - 1: Late-arriving interrupt
 */
#define INT_MODE	1

/************************** PRIVATE FUNCTIONS *************************/
void EINT0_IRQHandler(void);
void EINT3_IRQHandler(void);

void print_menu(void);
void delay (void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		External interrupt 0 handler
 * 				This interrupt occurs when pressing button INT0
 * @param		None
 * @return 		None
 ***********************************************************************/
void EINT0_IRQHandler(void)
{
	uint8_t i;
	EXTI_ClearEXTIFlag(EXTI_EINT0);
	for (i= 0; i<10; i++)
	{
		GPIO_SetValue(1,(1<<29));
		delay();
		GPIO_ClearValue(1,(1<<29));
		delay();
	}
}

/*********************************************************************//**
 * @brief 		External interrupt 3 handler
 * 				This interrupt occurs when turn ADC potentiometer
 * @param		None
 * @return 		None
 ***********************************************************************/
void EINT3_IRQHandler(void)
{
	uint8_t j;
	if (GPIO_GetIntStatus(0, 25, 1))
	{
		 GPIO_ClearInt(0,(1<<25));
		for (j= 0; j<10; j++)
		{
			GPIO_SetValue(1,(1<<28));
			delay();
			GPIO_ClearValue(1,(1<<28));
			delay();
		}
	}
}
/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		delay function
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void delay (void) {
  unsigned int i;

  for (i = 0; i < 0x400000; i++) {
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
	EXTI_InitTypeDef EXTICfg;
	PINSEL_CFG_Type PinCfg;

	/* Configure:
	 * 		+ LED1: P1.28
	 * 		+ LED2: P1.29
	 */
	GPIO_SetDir(1, (1<<28)|(1<<29), 1);

	/* Setting P2.10 as EINT0 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize External 0 interrupt */
	EXTI_Init();
	EXTICfg.EXTI_Line = EXTI_EINT0;
	/* edge sensitive */
	EXTICfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	EXTICfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_Config(&EXTICfg);


#if (INT_MODE == 0) //same group, different sub-levels (Tail-chaining example)
	NVIC_SetPriorityGrouping(4); //sets group priorities: 8 - subpriorities: 3
	NVIC_SetPriority(EINT0_IRQn, 2);  //000:10 (bit 7:3)  assign eint0 to group 0, sub-priority 2 within group 0
	NVIC_SetPriority(EINT3_IRQn, 1);  //000:01 (bit 7:3)  assign gpioint to group 0, sub-priority 1 within group 0
#else //different group - (Late-arriving example)
	NVIC_SetPriorityGrouping(4);  //sets group priorities: 8 - subpriorities: 3
	NVIC_SetPriority(EINT0_IRQn, 0);   //000:00 (bit 7:3) assign eint0 to group 0, sub-priority 0 within group 0
	NVIC_SetPriority(EINT3_IRQn, 4);   //001:00 (bit 7:3) assign GPIO int to group 1, sub-priority 0 within group 1
#endif

	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);

	//Enable GPIO interrupt  P0.25/AD0.2
	GPIO_IntCmd(0,(1<<25),1);

	while(1);
}

/* With ARM and GHS toolsets, the entry point is main() - this will
 allow the linker to generate wrapper code to setup stacks, allocate
 heap area, and initialize and copy code and data segments. For GNU
 toolsets, the entry point is through __start() in the crt0_gnu.asm
 file, and that startup code will setup stacks and data */
int main(void) {
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
void check_failed(uint8_t *file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
		;
}
#endif

/*
 * @}
 */
