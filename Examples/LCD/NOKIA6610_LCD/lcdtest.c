/**********************************************************************
* $Id$		lcdtest.c 				2010-06-18
*//**
* @file		lcdtest.c
* @brief	This example used to test LCD on IAR-LPC1768-KS board
* @version	3.0
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
#include "drv_glcd.h"
#include "glcd_ll.h"
#include "lpc17xx_nvic.h"
#include "core_cm3.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_libcfg.h"
#include "NXP_logo.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup LCD_NOKIA6610_LCD	NOKIA6610_LCD
 * @ingroup LCD_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define TIMER0_TICK_PER_SEC   20

/************************** PUBLIC DEFINITIONS *************************/
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;

/************************** PRIVATE DEFINITIONS *************************/
volatile Bool CntrSel = FALSE;

/*variable for clitical section entry control*/
uint32_t CriticalSecCntr;

/************************** PRIVATE FUNCTIONS *************************/
/* Interrupt service routine */
void TIMER0_IRQHandler (void);

void Dly100us(void *arg);


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		TIMER0 IRQ Handler
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void TIMER0_IRQHandler (void)
{
	//check BUT1
	if(!(GPIO_ReadValue(0)&(1<<23)))
	{
		CntrSel = FALSE;
	}
	//Check BUT2
	else if (!(GPIO_ReadValue(2)&(1<<13)))
	{
		CntrSel = TRUE;
	}
  // clear interrupt
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
}
/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Delay 100us
 * @param[in]	void *arg
 * @return 		None
 **********************************************************************/
void Dly100us(void *arg)
{
	volatile uint32_t Dly = (uint32_t)arg, Dly100;
	for(;Dly;Dly--)
    for(Dly100 = 500; Dly100; Dly100--);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main LCD program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	Bool SelHold;
	uint32_t AdcData, timer_tick;
	PINSEL_CFG_Type PinCfg;
	TIM_TIMERCFG_Type TimerCfg;
	TIM_MATCHCFG_Type MatchCfg;

	// But 0,1 init
	GPIO_SetDir(0, (1<<23), 0); //Setting BUT0 (P0.23) as input
	GPIO_SetDir(2, (1<<13), 0); //Setting BUT1 (P2.13) as input

	/* ADC Pin select
	 * P1.31 as AD0.5
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 2; //Pin has neither pull-up nor pull-down
	PinCfg.Pinnum = 31;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	// Enable ADC clock
	CLKPWR_ConfigPPWR (CLKPWR_PCONP_PCAD, ENABLE);

	/* Initialize ADC peripheral */
	ADC_Init(LPC_ADC, 100000);

	/* Enable ADC channel 5 */
	ADC_ChannelCmd (LPC_ADC, 5, ENABLE);

	/* Start ADC */
	ADC_StartCmd(LPC_ADC, ADC_START_NOW);

	/* Initialize TIMER0
	 * Timer channel: TIMER0
	 * Timer mode: every rising PCLK edge
	 * Prescale option: TICKVAL
	 * Prescale value = 0
	 */
	TimerCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TimerCfg.PrescaleValue = 0;
	TIM_Init(LPC_TIM0, TIM_COUNTER_RISING_MODE, &TimerCfg);

	//disable timer counter
	TIM_Cmd(LPC_TIM0, DISABLE);

	/* Initalize Match MR0
	 * - Enable interrupt
	 * - Enable reset
	 * - Disable stop
	 * - Match value = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0)/(TIMER0_TICK_PER_SEC)
	 */
	timer_tick = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0)/(TIMER0_TICK_PER_SEC);
	MatchCfg.MatchChannel = 0;
	MatchCfg.IntOnMatch = ENABLE;
	MatchCfg.ResetOnMatch = ENABLE;
	MatchCfg.StopOnMatch = DISABLE;
	MatchCfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	MatchCfg.MatchValue = timer_tick;
	TIM_ConfigMatch(LPC_TIM0,&MatchCfg);

	// Clear MR0 interrupt pending
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

	//Enable TIMER0 interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Enable TIMER0 counter
	TIM_Cmd(LPC_TIM0, ENABLE);

	// GLCD init
	GLCD_PowerUpInit((unsigned char *)NXP_Logo.pPicStream);
	GLCD_Backlight(BACKLIGHT_ON);

	GLCD_SetFont(&Terminal_9_12_6,0x000F00,0x00FF0);
	GLCD_SetWindow(10,116,131,131);
	GLCD_TextSetPos(0,0);

	if(CntrSel)
	{
		SelHold = TRUE;
		GLCD_print("\fContrast adj.\r");
	}
	else
	{
		SelHold = FALSE;
		GLCD_print("\fBacklight adj.\r");
	}

	while(1)
	{
		AdcData = ADC_GlobalGetData(LPC_ADC);
		if(AdcData & (1UL << 31))
		{
			//AD0 start conversion
			ADC_StartCmd(LPC_ADC, ADC_START_NOW);
			AdcData >>= 10;
			AdcData  &= 0xFF;
			if(SelHold)
			{
				// Contract adj
				GLCD_SendCmd(SETCON,(unsigned char *)&AdcData,0);
			}
			else
			{
				// Backlight adj
				AdcData >>= 1;
				GLCD_Backlight(AdcData);
			}
		}
		if(SelHold != CntrSel)
		{
			SelHold ^= 1;
			if(SelHold)
			{
				GLCD_print("\fContrast adj.\r");
			}
			else
			{
				GLCD_print("\fBacklight adj.\r");
			}
		}
	}
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
