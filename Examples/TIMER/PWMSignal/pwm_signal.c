/**********************************************************************
* $Id$		pwm_signal.c				2010-05-21
*//**
* @file		pwm_signal.c
* @brief	This example use TIMERs to generate 4 PWM signals with
* 			different duty cycle
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
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup TIMER_PWMSignal	PWMSignal
 * @ingroup TIMER_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"System Tick demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example used TIMERs to generate 4 PWM signals with different duty cycles \n\r"
	"********************************************************************************\n\r";
FunctionalState	PWM0_State = ENABLE;
FunctionalState	PWM1_State = ENABLE;
FunctionalState	PWM2_State = ENABLE;
FunctionalState	PWM3_State = ENABLE;

TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct;

/************************** PRIVATE FUNCTIONS *************************/
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);

void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		TIMER0 interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void TIMER0_IRQHandler(void)
{
	//duty cycle = 12.5%
	TIM_Cmd(LPC_TIM0,DISABLE);
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	TIM_ResetCounter(LPC_TIM0);
	if((PWM0_State == ENABLE))
	{
		TIM_UpdateMatchValue(LPC_TIM0, 0, 100);
		PWM0_State = DISABLE;
	}
	else
	{
		TIM_UpdateMatchValue(LPC_TIM0, 0, 700);
		PWM0_State = ENABLE;
	}
	TIM_Cmd(LPC_TIM0,ENABLE);
}

/*********************************************************************//**
 * @brief 		TIMER1 interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void TIMER1_IRQHandler(void)
{
	//duty cycle = 25%
	TIM_Cmd(LPC_TIM1,DISABLE);
	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
	TIM_ResetCounter(LPC_TIM1);
	if((PWM1_State == ENABLE))
	{
		TIM_UpdateMatchValue(LPC_TIM1,0, 200);
		PWM1_State = DISABLE;
	}
	else
	{
		TIM_UpdateMatchValue(LPC_TIM1,0, 600);
		PWM1_State = ENABLE;
	}
	TIM_Cmd(LPC_TIM1,ENABLE);
}

/*********************************************************************//**
 * @brief 		TIMER2 interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void TIMER2_IRQHandler(void)
{
	//duty cycle = 37,5%
	TIM_Cmd(LPC_TIM2,DISABLE);
	TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
	TIM_ResetCounter(LPC_TIM2);
	if((PWM2_State == ENABLE))
	{
		TIM_UpdateMatchValue(LPC_TIM2,0, 300);
		PWM2_State = DISABLE;
	}
	else
	{
		TIM_UpdateMatchValue(LPC_TIM2,0, 500);
		PWM2_State = ENABLE;
	}
	TIM_Cmd(LPC_TIM2,ENABLE);
}

/*********************************************************************//**
 * @brief 		TIMER3 interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void TIMER3_IRQHandler(void)
{
	//duty cycle = 50%
	TIM_Cmd(LPC_TIM3,DISABLE);
	TIM_ClearIntPending(LPC_TIM3, TIM_MR0_INT);
	TIM_ResetCounter(LPC_TIM3);
	TIM_Cmd(LPC_TIM3,ENABLE);
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
 * @brief		c_entry: Main TIMER program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void)
{
	PINSEL_CFG_Type PinCfg;

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

	/* Select pin:
	 * P1.28: MAT0.0
	 * P1.22: MAT1.0
	 * P0.6:  MAT2.0
	 * P0.10: MAT3.0
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 22;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize 4 timers, prescale count time of 100uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 100;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_Init(LPC_TIM3, TIM_TIMER_MODE,&TIM_ConfigStruct);

	// Configure 4 match channels
	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	// Set Match value
	TIM_MatchConfigStruct.MatchValue   = 700;
	// Set configuration for Tim_MatchConfig
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	TIM_MatchConfigStruct.MatchValue   = 600;
	TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);
	TIM_MatchConfigStruct.MatchValue   = 500;
	TIM_ConfigMatch(LPC_TIM2,&TIM_MatchConfigStruct);
	TIM_MatchConfigStruct.MatchValue   = 800;
	TIM_ConfigMatch(LPC_TIM3,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	// To start timer 0
	TIM_Cmd(LPC_TIM0,ENABLE);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER1_IRQn, ((0x01<<3)|0x02));
	/* Enable interrupt for timer 1 */
	NVIC_EnableIRQ(TIMER1_IRQn);
	// To start timer 1
	TIM_Cmd(LPC_TIM1,ENABLE);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER2_IRQn, ((0x01<<3)|0x03));
	/* Enable interrupt for timer 1 */
	NVIC_EnableIRQ(TIMER2_IRQn);
	// To start timer 1
	TIM_Cmd(LPC_TIM2,ENABLE);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(TIMER3_IRQn, ((0x01<<3)|0x04));
	/* Enable interrupt for timer 1 */
	NVIC_EnableIRQ(TIMER3_IRQn);
	// To start timer 1
	TIM_Cmd(LPC_TIM3,ENABLE);

	while(1);
	return (1);
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
