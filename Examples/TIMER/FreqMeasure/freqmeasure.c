/**********************************************************************
* $Id$		freqmeasure.c				2010-07-16
*//**
* @file		freqmeasure.c
* @brief	This example describes how to use Timer to measure a
* 			signal's frequency
* @version	1.0
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
/** @defgroup TIMER_FreqMeasure		FreqMeasure
 * @ingroup TIMER_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"Timer measure frequency demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 -  115200 bps \n\r"
" Use timer 0 to measure input signal frequency through its CAP0.0 \n\r"
" Use timer 2 to generate different frequency signals \n\r"
"********************************************************************************\n\r";

TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;

BOOL_8 first_capture,done;
uint32_t capture;
uint8_t count=0;
/************************** PRIVATE FUNCTIONS *************************/
/* Interrupt service routines */
void TIMER0_IRQHandler(void);

void print_menu(void);
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		TIMER0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void TIMER0_IRQHandler(void)
{
	if (TIM_GetIntCaptureStatus(LPC_TIM0,0))
	{
		TIM_ClearIntCapturePending(LPC_TIM0,0);
		if(first_capture==TRUE)
		{
			TIM_Cmd(LPC_TIM0,DISABLE);
			TIM_ResetCounter(LPC_TIM0);
			TIM_Cmd(LPC_TIM0,ENABLE);
			count++;
			if(count==5)first_capture=FALSE; //stable
		}
		else
		{
			count=0; //reset count for next use
			done=TRUE;
			capture = TIM_GetCaptureValue(LPC_TIM0,0);
		}
	}
}

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
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;
	uint8_t idx;
	uint16_t tem;
	uint32_t freq,temcap;
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


	//Config P1.26 as CAP0.0
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);
	// Configure P0.6 as MAT2.0
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);

	while(1)
	{
		// Initialize timer 0, prescale count time of 1uS
		TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
		TIM_ConfigStruct.PrescaleValue	= 1;

		// use channel 0, CAPn.0
		TIM_CaptureConfigStruct.CaptureChannel = 0;
		// Enable capture on CAPn.0 rising edge
		TIM_CaptureConfigStruct.RisingEdge = ENABLE;
		// Enable capture on CAPn.0 falling edge
		TIM_CaptureConfigStruct.FallingEdge = DISABLE;
		// Generate capture interrupt
		TIM_CaptureConfigStruct.IntOnCaption = ENABLE;

		// Set configuration for Tim_config and Tim_MatchConfig
		TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
		TIM_ConfigCapture(LPC_TIM0, &TIM_CaptureConfigStruct);
		TIM_ResetCounter(LPC_TIM0);
	///////////////////////////////////////////////////////////////////
		idx=0;freq=0;tem=0;
		while(idx<3)
		{
			if(idx==0)
			_DBG("\n\rPlease input frequency (from 1 to 999 hz):");
			tem = _DG;
			switch(tem)
			{
			case '0':case'1':case '2':case '3':case '4':case '5':case '6':case '7':case'8':case '9':
				tem = tem - 0x30;
				idx++;
				if(idx==1) tem = tem * 100;
				else if (idx==2) tem = tem * 10;
				freq = freq + tem;
				if(idx==3)_DBD16(freq);
				tem = 0;
				break;
			default:
				_DBG("...Please input digits from 0 to 9 only!");
				idx = 0; tem = 0; freq = 0;
				break;
			}
		}
		TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
		TIM_ConfigStruct.PrescaleValue	= 1;//1us
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
		TIM_MatchConfigStruct.MatchValue   = 500000/freq;
		TIM_ConfigMatch(LPC_TIM2,&TIM_MatchConfigStruct);

		TIM_Cmd(LPC_TIM2,ENABLE);
	//////////////////////////////////////////////////////////////////
		/* preemption = 1, sub-priority = 1 */
		NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
		/* Enable interrupt for timer 0 */
		NVIC_EnableIRQ(TIMER0_IRQn);

		first_capture = TRUE;done=FALSE;capture=0;
		// To start timer 0
		TIM_Cmd(LPC_TIM0,ENABLE);

		_DBG("\n\rMeasuring......");
		while(done==FALSE);
		temcap = 1000000 / capture;_DBD16(temcap);_DBG("hz");
		NVIC_DisableIRQ(TIMER0_IRQn);
		TIM_DeInit(LPC_TIM0);
		TIM_DeInit(LPC_TIM2);
		_DBG("\n\rPress c to continue measuring other signals...");
		while(_DG != 'c');
	}
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
