/**********************************************************************
* $Id$		i2s_test_4_wire.c 				2010-05-21
*//**
* @file		i2s_test_4_wire.c
* @brief	This example describes how to use I2S 4-wire mode
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
/** @defgroup I2S_test_4_wire	I2S_test_4_wire
 * @ingroup I2S_Examples
 * @{
 */

///************************** PRIVATE DEFINITIONS *************************/
/** Max buffer length */
#define BUFFER_SIZE			0x400
/** I2S Buffer Source Address is AHBRAM1_BASE that used for USB RAM purpose, but
 * it is not used in this example, so this memory section can be used for general purpose
 * memory
 */
#define I2S_BUFFER_SRC			LPC_AHBRAM1_BASE
/** I2S Buffer Destination Address is (AHBRAM1_BASE + 0x100UL) that used for USB RAM purpose, but
 * it is not used in this example, so this memory section can be used for general purpose
 * memory
 */
#define I2S_BUFFER_DST			(I2S_BUFFER_SRC+0x1000UL)

/************************** PRIVATE VARIABLES ***********************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	" I2S 4-wire demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	" Use two I2S channels in the same board to transfer data in 4-wire mode\n\r"
	"********************************************************************************\n\r";
volatile uint8_t  I2STXDone = 0;
volatile uint8_t  I2SRXDone = 0;

volatile uint32_t *I2STXBuffer = (uint32_t*)(I2S_BUFFER_SRC);
volatile uint32_t *I2SRXBuffer = (uint32_t *)(I2S_BUFFER_DST);

volatile uint32_t I2SReadLength = 0;
volatile uint32_t I2SWriteLength = 0;


/************************** PRIVATE FUNCTIONS *************************/
void Buffer_Init(void);
Bool Buffer_Verify(void);
void print_menu(void);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void Buffer_Init(void) {
	uint32_t i;

	for (i = 0; i < BUFFER_SIZE; i++) {
		I2STXBuffer[i] = i;
		I2SRXBuffer[i] = 0;
	}
}

/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		TRUE - if two buffers are similar
 * 				FALSE - if two buffers are different
 **********************************************************************/
Bool Buffer_Verify(void) {
	uint32_t i;
	uint32_t *pTX = (uint32_t *) &I2STXBuffer[0];
	/* Generally, first element of RX buffer is a dummy data, so
	 * it will be discarded
	 */
	uint32_t *pRX = (uint32_t *) &I2SRXBuffer[1];

	for (i = 1; i < BUFFER_SIZE; i++) {
		if (*pTX++ != *pRX++)  {
			return FALSE;
		}
	}
	return TRUE;
}
/*********************************************************************//**
 * @brief		Print menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
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
	uint32_t i;
	uint8_t dummy=0;
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

	Buffer_Init();

	/* Pin configuration:
	 * Assign: 	- P0.4 as I2SRX_CLK
	 * 			- P0.5 as I2SRX_WS
	 * 			- P0.6 as I2SRX_SDA
	 * 			- P0.7 as I2STX_CLK
	 * 			- P0.8 as I2STX_WS
	 * 			- P0.9 as I2STX_SDA
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
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);

	I2S_Init(LPC_I2S);

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

	/* Audio Config*/
	I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
	I2S_ConfigStruct.mono = I2S_STEREO;
	I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
	I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
	I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
	I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
	I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);

	I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
	I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);

	/* Clock Mode Config*/
	I2S_ClkConfig.clksel = I2S_CLKSEL_FRDCLK;
	I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
	I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
	I2S_ClkConfig.fpin = I2S_4PIN_ENABLE;
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);

	/* Set up frequency and bit rate*/
	I2S_FreqConfig(LPC_I2S, 44100, I2S_TX_MODE);

	 I2S_Start(LPC_I2S);
	 while(I2STXDone == 0||I2SRXDone == 0){
	   if(I2STXDone ==0){
			 while (I2S_GetLevel(LPC_I2S,I2S_TX_MODE)!=0x00);
			 I2S_Send(LPC_I2S,I2STXBuffer[I2SWriteLength]);
			 I2SWriteLength +=1;
			 if(I2SWriteLength == BUFFER_SIZE) I2STXDone = 1;

		}
		if(I2SRXDone == 0)
		{
			while(I2S_GetLevel(LPC_I2S,I2S_RX_MODE)==0x00);
			if(dummy == 0) //dummy receive
			{
				i = I2S_Receive(LPC_I2S);
					dummy = 1;
			}
			else
			{
				*(uint32_t *)(&I2SRXBuffer[I2SReadLength]) = I2S_Receive(LPC_I2S);
				I2SReadLength +=1;
			}
			if(I2SReadLength == BUFFER_SIZE) I2SRXDone = 1;
		}
	 }
	 for(i=0;i<BUFFER_SIZE;i++)
	 {
		 _DBH32(I2SRXBuffer[i]);_DBG_("");
	 }
	 if(Buffer_Verify())
	 {
		 _DBG_("Verify Buffer: OK...");
	 }
	 else
	 {
		 _DBG_("Verify Buffer: ERROR...");
	 }
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
