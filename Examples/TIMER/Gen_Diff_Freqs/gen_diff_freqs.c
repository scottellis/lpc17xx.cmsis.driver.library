/**********************************************************************
* $Id$		gen_diff_freqs.c				2010-07-16
*//**
* @file		gen_diff_freqs.c
* @brief	This example describes how to use TIMER0 and TIMER2 to
* 			generate 2 different frequency signals
* @version	2.0
* @date		16. July. 2010
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
#include "lpc17xx_timer.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup TIMER_Gen_Diff_Freqs		Gen_Diff_Freqs
 * @ingroup TIMER_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"Timer generate different frequencies demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use timer 0 and timer 2 to generate 2 different frequency signals\n\r"
" UART0 used to display and input frequency\n\r"
"********************************************************************************\n\r";

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);


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


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main TIMER program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;
	uint8_t idx;
	uint16_t tem;
	uint32_t freq1, freq2;

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
	while(1)
	{
		// Select frequency for channel1 (MAT0.0)
		idx=0;freq1=0;tem=0;
		while(idx<3)
		{
			if(idx==0)
			_DBG("\n\rPlease input frequency for channel1 (from 1 to 999 hz):");
			tem = _DG;
			switch(tem)
			{
			case '0':case'1':case '2':case '3':case '4':case '5':case '6':case '7':case'8':case '9':
				tem = tem - 0x30;
				idx++;
				if(idx==1) tem = tem * 100;
				else if (idx==2) tem = tem * 10;
				freq1 = freq1 + tem;
				if(idx==3)_DBD16(freq1);
				tem = 0;
				break;
			default:
				_DBG("...Please input digits from 0 to 9 only!");
				idx = 0; tem = 0; freq1 = 0;
				break;
			}
		}
		idx=0;freq2=0;tem=0;
		while(idx<3)
		{
			if(idx==0)
			_DBG("\n\rPlease input frequency for channel2 (from 1 to 999 hz):");
			tem = _DG;
			switch(tem)
			{
			case '0':case'1':case '2':case '3':case '4':case '5':case '6':case '7':case'8':case '9':
				tem = tem - 0x30;
				idx++;
				if(idx==1) tem = tem * 100;
				else if (idx==2) tem = tem * 10;
				freq2 = freq2 + tem;
				if(idx==3)_DBD16(freq2);
				tem = 0;
				break;
			default:
				_DBG("...Please input digits from 0 to 9 only!");
				idx = 0; tem = 0; freq2 = 0;
				break;
			}
		}
		// Configure P1.28 as MAT0.0
		PinCfg.Funcnum = 3;
		PinCfg.OpenDrain = 0;
		PinCfg.Pinmode = 0;
		PinCfg.Portnum = 1;
		PinCfg.Pinnum = 28;
		PINSEL_ConfigPin(&PinCfg);
		// Configure P0.6 as MAT2.0
		PinCfg.Funcnum = 3;
		PinCfg.OpenDrain = 0;
		PinCfg.Pinmode = 0;
		PinCfg.Portnum = 0;
		PinCfg.Pinnum = 6;
		PINSEL_ConfigPin(&PinCfg);

		// Initialize timer, prescale count time of 1uS
		TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
		TIM_ConfigStruct.PrescaleValue	= 1;
		TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
		TIM_Init(LPC_TIM2, TIM_TIMER_MODE,&TIM_ConfigStruct);

		// use channel 0, MR0
		TIM_MatchConfigStruct.MatchChannel = 0;
		// Disable interrupt when MR0 matches the value in TC register
		TIM_MatchConfigStruct.IntOnMatch   = FALSE;
		//Enable reset on MR0: TIMER will reset if MR0 matches it
		TIM_MatchConfigStruct.ResetOnMatch = TRUE;
		//Stop on MR0 if MR0 matches it
		TIM_MatchConfigStruct.StopOnMatch  = FALSE;
		//Toggle MR0.0 pin if MR0 matches it
		TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
		// Set Match value
		TIM_MatchConfigStruct.MatchValue   = 500000/freq1;
		TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);

		// Set Match value
		TIM_MatchConfigStruct.MatchValue   = 500000/freq2;
		TIM_ConfigMatch(LPC_TIM2,&TIM_MatchConfigStruct);

		TIM_Cmd(LPC_TIM0,ENABLE);
		TIM_Cmd(LPC_TIM2,ENABLE);
		_DBG("\n\rGenerating two different frequency signals..");
		_DBG("\n\rPress ESC if you want to terminate and choose other frequencies");
		while(_DG!=27);
		TIM_DeInit(LPC_TIM0);
		TIM_DeInit(LPC_TIM2);
	}
	return (1);
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
