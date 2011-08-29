/**********************************************************************
* $Id$		systick_stclk.c				2010-05-21
*//**
* @file		systick_stclk.c
* @brief	This example describes how to configure System Tick to use
* 			with external clock source STCLK
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
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SysTick_STCLK	STCLK
 * @ingroup SysTick_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
FunctionalState Cur_State = ENABLE;
Bool IO_State = FALSE;

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
	PINSEL_CFG_Type PinCfg;
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;

	// Conifg P1.28 as MAT0.0
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);

	/* P3.26 as STCLK */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 3;
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize timer 0, prescale count time of 10uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 10;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Disable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	// Set Match value, count value of 10 (10 * 10uS = 100uS --> 10KHz)
	TIM_MatchConfigStruct.MatchValue   = 10;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	TIM_Cmd(LPC_TIM0,ENABLE);

	GPIO_SetDir(0, (1<<0), 1); //Set P0.0 as output

	//Use P0.0 to test System Tick interrupt
	/* Initialize System Tick with 10ms time interval
	 * Frequency input = 10kHz /2 = 5kHz
	 * Time input = 10ms
	 */
	SYSTICK_ExternalInit(5000, 10);
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
