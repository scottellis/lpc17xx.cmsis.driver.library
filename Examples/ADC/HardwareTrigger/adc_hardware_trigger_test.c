/**********************************************************************
* $Id$		adc_hardware_trigger_test.c			2010-06-18
*//**
* @file		adc_hardware_trigger_test.c
* @brief	This example describes how to use ADC conversion in
* 			hardware-triggered mode
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
#include "lpc17xx_adc.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup ADC_HardwareTrigger	HardwareTrigger
 * @ingroup ADC_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
#define _ADC_INT			ADC_ADINTEN2
#define _ADC_CHANNEL		ADC_CHANNEL_2

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" ADC demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use ADC with 12-bit resolution rate of 200KHz, read in hardware-triggred mode\n\r"
" Press INT0 button to start ADC conversion, ADC interrupt when conversion completed\n\r"
" ADC channel value is read back and displayed via UART0\n\r"
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
	if (ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,ADC_DATA_DONE))
	{
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
		NVIC_DisableIRQ(ADC_IRQn);
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
 * @brief		c_entry: Main ADC program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
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

	/*
	 * Init P2.10
	 *
	 */
	PinCfg.Funcnum = 1; //EINT0
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	/* Configuration for ADC:
	 *  select: ADC channel 2 (if using MCB1700 board)
	 *  		ADC channel 5 (if using IAR-LPC1768 board)
	 *  ADC conversion rate = 200KHz
	 */
	ADC_Init(LPC_ADC, 200000);
	ADC_IntConfig(LPC_ADC,_ADC_INT,ENABLE);
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL,ENABLE);
	ADC_EdgeStartConfig(LPC_ADC,ADC_START_ON_FALLING);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(ADC_IRQn, ((0x01<<3)|0x01));

	while(1)
	{
		adc_value = 0;

		// Start conversion on EINT0 falling edge
		ADC_StartCmd(LPC_ADC,ADC_START_ON_EINT0);

		/* Enable ADC in NVIC */
		NVIC_EnableIRQ(ADC_IRQn);

		_DBG("Press INT0 button to start ADC conversion on AD0.2...");_DBG_("");
		while(adc_value==0);

		//Display the result of conversion on the UART0
		_DBG("ADC value on channel 2: ");

		_DBD32(adc_value);
		_DBG_("");
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
