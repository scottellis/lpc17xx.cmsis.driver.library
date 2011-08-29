/**********************************************************************
* $Id$		i2s_two_kit.c 				2010-05-21
*//**
* @file		i2s_two_kit.c
* @brief	This example describes how to use I2S transfer data between
* 			two board
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
#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup I2S_two_kit	I2S_two_kit
 * @ingroup I2S_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define I2S_TRANSMIT			0
#define I2S_RECEIVE				!I2S_TRANSMIT


/************************** PRIVATE VARIABLE ***********************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	" I2S polling mode demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	" Use two I2S channels on 2 board to transfer data \n\r"
	"********************************************************************************\n\r";
uint8_t tx_depth_irq = 0;
uint8_t rx_depth_irq = 0;
uint32_t data;

/************************** PRIVATE FUNCTIONS *************************/
/* Interrupt service routine */
void I2S_IRQHandler(void);

void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		I2S IRQ Handler
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void I2S_IRQHandler()
{
	uint8_t rx_level,
			tx_level,
			tx_depth_irq,
			rx_depth_irq;
	uint32_t data;
	uint8_t i;

	if(I2S_GetIRQStatus(LPC_I2S, I2S_RX_MODE)){ //receive interrupt
		rx_level = I2S_GetLevel(LPC_I2S, I2S_RX_MODE);
		rx_depth_irq = I2S_GetIRQDepth(LPC_I2S, I2S_RX_MODE);

		if (rx_level >= rx_depth_irq)//receive interrupt
		{
			while(I2S_GetLevel(LPC_I2S, I2S_RX_MODE)>0)
			{
				data = I2S_Receive(LPC_I2S);
				_DBH32(data); _DBG_("");
			}
		}
	}
	else if (I2S_GetIRQStatus(LPC_I2S, I2S_TX_MODE))
	{
		tx_level = I2S_GetLevel(LPC_I2S, I2S_TX_MODE);
		tx_depth_irq = I2S_GetIRQDepth(LPC_I2S, I2S_TX_MODE);
		if(tx_level <= tx_depth_irq)//transmit interrupt
		{
			for(i=0;i<4;i++)
			{
				I2S_Send(LPC_I2S,data);
				_DBH32(data);
				_DBG_(" Send successful!!!");
				data++;
				if(data == 0xFFFFFFFF) data  = 0;
			}
		}
	}
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		print_menu
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void print_menu()
{
	_DBG_(menu);
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main I2S program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void) {
	I2S_MODEConf_Type I2S_ClkConfig;
	I2S_CFG_Type I2S_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	//print menu screen
	print_menu();

	NVIC_EnableIRQ(I2S_IRQn);
	//Setup for I2S: RX is similar with TX
	/* setup:
	 * 		- wordwidth: 16 bits
	 * 		- stereo mode
	 * 		- master mode for I2S_TX and slave for I2S_RX
	 * 		- ws_halfperiod is 31
	 * 		- not use mute mode
	 * 		- use reset and stop mode
	 * 		- select the fractional rate divider clock output as the source,
	 * 		- disable 4-pin mode
	 * 		- MCLK ouput is disable
	 * 		- Frequency = 44.1 kHz (x=8,y=51 - automatic setting)
	 * Because we use mode I2STXMODE[3:0]= 0000, I2SDAO[5]=0 and
	 * I2SRX[3:0]=0000, I2SDAI[5] = 1. So we have I2SRX_CLK = I2STX_CLK
	 * --> I2SRXBITRATE = 1 (not divide TXCLK to produce RXCLK)
	 */

#if I2S_TRANSMIT
	/* Pin configuration for I2S Transmitter:
	 * Assign: 	- P0.7 as I2STX_CLK
	 * 			- P0.8 as I2STX_WS
	 * 			- P0.9 as I2STX_SDA
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 7;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);

	I2S_Init(LPC_I2S);

	/* Audio Config*/
	I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
	I2S_ConfigStruct.mono = I2S_STEREO;
	I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
	I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
	I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
	I2S_ConfigStruct.mute = I2S_MUTE_ENABLE;
	I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);

	/* Clock Mode Config*/
	I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
	I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
	I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);

	LPC_I2S->I2STXRATE = 0x00;
	LPC_I2S->I2STXBITRATE = 0x00;
	I2S_SetBitRate(LPC_I2S, 0, I2S_TX_MODE);

	I2S_Start(LPC_I2S);
	I2S_IRQConfig(LPC_I2S,I2S_TX_MODE,4);
	I2S_IRQCmd(LPC_I2S,I2S_TX_MODE,ENABLE);

#else
	/* Pin configuration for I2S Receiver:
	 * Assign: 	- P0.4 as I2SRX_CLK
	 * 			- P0.5 as I2SRX_WS
	 * 			- P0.6 as I2SRX_SDA
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 4;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);

	I2S_Init(LPC_I2S);

	I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
	I2S_ConfigStruct.mono = I2S_STEREO;
	I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
	I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
	I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
	I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
	I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);

	/* Clock Mode Config*/
	I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
	I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
	I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);

	/* Set up frequency and bit rate*/
	I2S_FreqConfig(LPC_I2S, 44100, I2S_RX_MODE);
	I2S_IRQConfig(LPC_I2S,I2S_RX_MODE,4);
	I2S_IRQCmd(LPC_I2S,I2S_RX_MODE,ENABLE);

	_DBG_("Press '1' to start I2S operation...");
	while(_DG !='1');
	I2S_Start(LPC_I2S);
	_DBG_("I2S Start ...");
#endif
	while(1);
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
