/**********************************************************************
* $Id$		adc_interrupt_test.c			2010-05-21
*//**
* @file		adc_interrupt_test.c
* @brief	This example describes how to use ADC conversion in
* 			interrupt mode
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
#include "lpc17xx_adc.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup ADC_INTERRUPT	INTERRUPT
 * @ingroup ADC_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

#ifdef MCB_LPC_1768
#define _ADC_INT			ADC_ADINTEN2
#define _ADC_CHANNEL		ADC_CHANNEL_2
#elif defined(IAR_LPC_1768)
#define _ADC_INT			ADC_ADINTEN5
#define _ADC_CHANNEL		ADC_CHANNEL_5
#endif

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" ADC demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use ADC with 12-bit resolution rate of 200KHz, read in interrupt mode\n\r"
" To get ADC channel value and display via UART0\n\r"
" Turn the potentiometer to see how ADC value changes\n\r"
"********************************************************************************\n\r";

uint32_t adc_value;

/************************** PRIVATE FUNCTION *************************/
void ADC_IRQHandler(void);

void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		ADC interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void ADC_IRQHandler(void)
{
	adc_value = 0;
#ifdef MCB_LPC_1768
	if (ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,ADC_DATA_DONE))
	{
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
		NVIC_DisableIRQ(ADC_IRQn);
	}
#elif defined(IAR_LPC_1768)
	if (ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_5,ADC_DATA_DONE))
	{
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_5);
		NVIC_DisableIRQ(ADC_IRQn);
	}
#endif
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
 * @brief		c_entry: Main ADC program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t tmp;

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

	/* Because the potentiometer on different boards (MCB & IAR) connect
	 * with different ADC channel, so we have to configure correct ADC channel
	 * on each board respectively.
	 * If you want to check other ADC channels, you have to wire this ADC pin directly
	 * to potentiometer pin (please see schematic doc for more reference)
	 */
#ifdef MCB_LPC_1768
	/*
	 * Init ADC pin connect
	 * AD0.2 on P0.25
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
#elif defined (IAR_LPC_1768)
	/* select P1.31 as AD0.5 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 31;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);
#endif


	/* Configuration for ADC:
	 *  select: ADC channel 2 (if using MCB1700 board)
	 *  		ADC channel 5 (if using IAR-LPC1768 board)
	 *  ADC conversion rate = 200KHz
	 */
	ADC_Init(LPC_ADC, 200000);
	ADC_IntConfig(LPC_ADC,_ADC_INT,ENABLE);
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL,ENABLE);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(ADC_IRQn, ((0x01<<3)|0x01));

	while(1)
	{
		// Start conversion
		ADC_StartCmd(LPC_ADC,ADC_START_NOW);

		/* Enable ADC in NVIC */
		NVIC_EnableIRQ(ADC_IRQn);

		//Display the result of conversion on the UART0
#ifdef MCB_LPC_1768
		_DBG("ADC value on channel 2: ");
#elif defined (IAR_LPC_1768)
		_DBG("ADC value on channel 5: ");
#endif
		_DBD32(adc_value);
		_DBG_("");
		for(tmp = 0; tmp < 1000000; tmp++);
	}
	ADC_DeInit(LPC_ADC);
	return (0);
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
