/**********************************************************************
* $Id$		10ms_base.c				2010-05-21
*//**
* @file		10ms_base.c
* @brief	This example describes how to configure System Tick to generate
* 			interrupt each 10ms
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
#include "lpc17xx_libcfg.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SysTick_10ms_base	10ms_base
 * @ingroup SysTick_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
FunctionalState Cur_State = ENABLE;

/************************** PRIVATE FUNCTIONS *************************/
void SysTick_Handler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		SysTick interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void SysTick_Handler(void)
{
	//Clear System Tick counter flag
	SYSTICK_ClearCounterFlag();
	//toggle P0.0
	if (Cur_State == ENABLE)
	{
		//pull-down pin
		GPIO_ClearValue(0, (1<<0));
		Cur_State = DISABLE;
	}
	else
	{
		GPIO_SetValue(0, (1<<0));
		Cur_State = ENABLE;
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
	//Use P0.0 to test System Tick interrupt
	GPIO_SetDir(0, (1<<0), 1); //Set P0.0 as output

	//Initialize System Tick with 10ms time interval
	SYSTICK_InternalInit(10);
	//Enable System Tick interrupt
	SYSTICK_IntCmd(ENABLE);
	//Enable System Tick Counter
	SYSTICK_Cmd(ENABLE);

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
