/**********************************************************************
* $Id$		adc_interrupt_test.c			2010-07-16
*//**
* @file		adc_interrupt_test.c
* @brief	This example describes how to use ADC conversion in
* 			burst mode
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
#include "lpc17xx_adc.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup ADC_Burst		Burst
 * @ingroup ADC_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
#define MCB_LPC_1768
//#define IAR_LPC_1768
#define MCB_LPC17XX_ADC_INJECT_TEST

#ifdef MCB_LPC_1768
#define MCB_LPC_1768_ADC_BURST_MULTI
#endif

#ifdef MCB_LPC_1768
#define _ADC_INT			ADC_ADINTEN2
#define _ADC_CHANNEL		ADC_CHANNEL_2

#ifdef MCB_LPC_1768_ADC_BURST_MULTI
#define _ADC_INT_3			ADC_ADINTEN3
#define _ADC_CHANNEL_3		ADC_CHANNEL_3
#endif

#elif defined(IAR_LPC_1768)
#define _ADC_INT			ADC_ADINTEN5
#define _ADC_CHANNEL		ADC_CHANNEL_5
#endif

/** DMA size of transfer */
#define DMA_SIZE		8

#ifdef MCB_LPC17XX_ADC_INJECT_TEST
#define GPIO_INT	(1<<10)
#define POLL_LED	(1<<4)		// P1.28
#endif
/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" ADC burst demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use ADC with 12-bit resolution rate of 200KHz, running burst mode (single or multiple input)\n\r"
" Display ADC value via UART0\n\r"
" Turn the potentiometer to see how ADC value changes\n\r"
"********************************************************************************\n\r";

#ifdef MCB_LPC17XX_ADC_INJECT_TEST
static BOOL_8 toggle=FALSE;
#endif
/************************** PRIVATE FUNCTION *************************/
void print_menu(void);

#ifdef MCB_LPC17XX_ADC_INJECT_TEST
void EINT3_IRQHandler(void);
#endif

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
#ifdef MCB_LPC17XX_ADC_INJECT_TEST
void EINT3_IRQHandler(void)
{
	  if (GPIO_GetIntStatus(1, 10, 1))
	  {
		  GPIO_ClearInt(2,(1<<10));
		  toggle=~toggle;
#ifdef MCB_LPC_1768_ADC_BURST_MULTI
		  if(toggle)
		  {
			  ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL_3,ENABLE);
			  FIO_ByteSetValue(1, 3, POLL_LED);
		  }
		  else
		  {
			  ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL_3,DISABLE);
			  FIO_ByteClearValue(1, 3, POLL_LED);
		  }
#endif
	  }
}
#endif
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
#ifdef MCB_LPC_1768_ADC_BURST_MULTI
	PINSEL_CFG_Type PinCfg1;
#endif
	uint32_t tmp;
	uint32_t adc_value;
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
#ifdef MCB_LPC_1768_ADC_BURST_MULTI
	/*
	 * Init ADC pin connect
	 * AD0.3 on P0.26
	 */
	PinCfg1.Funcnum = 1;
	PinCfg1.OpenDrain = 0;
	PinCfg1.Pinmode = 0;
	PinCfg1.Pinnum = 26;
	PinCfg1.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg1);
#endif
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
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL,ENABLE);
#ifdef MCB_LPC_1768_ADC_BURST_MULTI
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL_3,ENABLE);
#endif

#ifdef MCB_LPC17XX_ADC_INJECT_TEST
	FIO_ByteSetDir(1, 3, POLL_LED, 1);
	FIO_ByteClearValue(1, 3, POLL_LED);
	// Enable GPIO interrupt P2.10
	GPIO_IntCmd(2,(1<<10),1);
	NVIC_EnableIRQ(EINT3_IRQn);
#endif

	//Start burst conversion
	ADC_BurstCmd(LPC_ADC,ENABLE);
	while(1)
	{
#ifdef MCB_LPC_1768
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
		_DBG("ADC value on channel 2: ");
#elif defined (IAR_LPC_1768)
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_5);
		_DBG("ADC value on channel 5: ");
#endif
		_DBD32(adc_value);
		_DBG_("");
#ifdef MCB_LPC_1768_ADC_BURST_MULTI
		adc_value =  ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_3);
		_DBG("ADC value on channel 3: ");
		_DBD32(adc_value);
		_DBG_("");
#endif
		// Wait for a while
		for(tmp = 0; tmp < 1500000; tmp++);
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
