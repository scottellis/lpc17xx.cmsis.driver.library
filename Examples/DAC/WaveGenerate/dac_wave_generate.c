/**********************************************************************
* $Id$		dac_wave_generate.c				2010-07-16
*//**
* @file		dac_wave_generate.c
* @brief	This example describes how to use DAC to generate a sine wave,
 * 			triangle wave or escalator wave
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
#include "lpc17xx_dac.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup DAC_WaveGenerate		WaveGenerate
 * @ingroup DAC_Examples
 * @{
 */

/************************** PRIVATE MACROS *************************/
/** DMA size of transfer */
#define DMA_SIZE_SINE		60
#define NUM_SAMPLE_SINE		60
#define DMA_SIZE			64
#define NUM_SAMPLE			64

#define SIGNAL_FREQ_IN_HZ	60
#define PCLK_DAC_IN_MHZ	25 //CCLK divided by 4

#define DAC_GENERATE_SINE		1
#define DAC_GENERATE_TRIANGLE	2
#define DAC_GENERATE_ESCALATOR	3
#define DAC_GENERATE_NONE		0

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" DAC generate signals demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 kbps \n\r"
" Use DAC to generate sine, triangle, escalator wave, frequency adjustable\n\r"
" Signal samples are transmitted to DAC by DMA memory to peripheral\n\r"
"********************************************************************************\n\r";

/************************** PRIVATE FUNCTION *************************/
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
 * @brief		c_entry: Main DAC program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	DAC_CONVERTER_CFG_Type DAC_ConverterConfigStruct;
	GPDMA_Channel_CFG_Type GPDMACfg;
	GPDMA_LLI_Type DMA_LLI_Struct;
	uint32_t tmp;
	uint8_t i,option;
	uint32_t sin_0_to_90_16_samples[16]={\
			0,1045,2079,3090,4067,\
			5000,5877,6691,7431,8090,\
			8660,9135,9510,9781,9945,10000\
	};
	uint32_t dac_lut[NUM_SAMPLE];

	/*
	 * Init DAC pin connect
	 * AOUT on P0.26
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

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
		//Select option
		_DBG("Please choose signal type:\n\r");
		_DBG("\t1) Sine wave.\n\r");
		_DBG("\t2) Triangle wave.\n\r");
		_DBG("\t3) Escalator wave.\n\r");
		option = DAC_GENERATE_NONE;
		while(option == DAC_GENERATE_NONE)
		{
			switch(_DG)
			{
			case '1': option = DAC_GENERATE_SINE; break;
			case '2': option = DAC_GENERATE_TRIANGLE; break;
			case '3': option = DAC_GENERATE_ESCALATOR; break;
			default:
				_DBG("Wrong choice! Please select 1 or 2 or 3 only!\n\r");
				option = DAC_GENERATE_NONE;
				break;
			}
		}

		//Prepare DAC look up table
		switch(option)
		{
		case DAC_GENERATE_SINE:
			for(i=0;i<NUM_SAMPLE_SINE;i++)
			{
				if(i<=15)
				{
					dac_lut[i] = 512 + 512*sin_0_to_90_16_samples[i]/10000;
					if(i==15) dac_lut[i]= 1023;
				}
				else if(i<=30)
				{
					dac_lut[i] = 512 + 512*sin_0_to_90_16_samples[30-i]/10000;
				}
				else if(i<=45)
				{
					dac_lut[i] = 512 - 512*sin_0_to_90_16_samples[i-30]/10000;
				}
				else
				{
					dac_lut[i] = 512 - 512*sin_0_to_90_16_samples[60-i]/10000;
				}
				dac_lut[i] = (dac_lut[i]<<6);
			}
			break;
		case DAC_GENERATE_TRIANGLE:
			for(i=0;i<NUM_SAMPLE;i++)
			{
				if(i<32) dac_lut[i]= 32*i;
				else if (i==32) dac_lut[i]= 1023;
				else dac_lut[i] = 32*(NUM_SAMPLE-i);
				dac_lut[i] = (dac_lut[i]<<6);
			}
			break;
		case DAC_GENERATE_ESCALATOR:
			for(i=0;i<NUM_SAMPLE;i++)
			{
				dac_lut[i] = (1023/3)*(i/16);
				dac_lut[i] = (dac_lut[i]<<6);
			}
			break;
		default: break;
		}

		//Prepare DMA link list item structure
		DMA_LLI_Struct.SrcAddr= (uint32_t)dac_lut;
		DMA_LLI_Struct.DstAddr= (uint32_t)&(LPC_DAC->DACR);
		DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;
		DMA_LLI_Struct.Control= ((option==DAC_GENERATE_SINE)?DMA_SIZE_SINE:DMA_SIZE)
								| (2<<18) //source width 32 bit
								| (2<<21) //dest. width 32 bit
								| (1<<26) //source increment
								;


		/* GPDMA block section -------------------------------------------- */
		/* Initialize GPDMA controller */
		GPDMA_Init();

		// Setup GPDMA channel --------------------------------
		// channel 0
		GPDMACfg.ChannelNum = 0;
		// Source memory
		GPDMACfg.SrcMemAddr = (uint32_t)(dac_lut);
		// Destination memory - unused
		GPDMACfg.DstMemAddr = 0;
		// Transfer size
		GPDMACfg.TransferSize = ((option==DAC_GENERATE_SINE)?DMA_SIZE_SINE:DMA_SIZE);
		// Transfer width - unused
		GPDMACfg.TransferWidth = 0;
		// Transfer type
		GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
		// Source connection - unused
		GPDMACfg.SrcConn = 0;
		// Destination connection
		GPDMACfg.DstConn = GPDMA_CONN_DAC;
		// Linker List Item - unused
		GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;
		// Setup channel with given parameter
		GPDMA_Setup(&GPDMACfg);

		DAC_ConverterConfigStruct.CNT_ENA =SET;
		DAC_ConverterConfigStruct.DMA_ENA = SET;
		DAC_Init(LPC_DAC);
		/* set time out for DAC*/
		tmp = (PCLK_DAC_IN_MHZ*1000000)/(SIGNAL_FREQ_IN_HZ*((option==DAC_GENERATE_SINE)?NUM_SAMPLE_SINE:NUM_SAMPLE));
		DAC_SetDMATimeOut(LPC_DAC,tmp);
		DAC_ConfigDAConverterControl(LPC_DAC, &DAC_ConverterConfigStruct);

		//Start the demo
		switch(option)
		{
		case DAC_GENERATE_SINE:
			_DBG("\n\rDAC is generating 60Hz sine wave...");
			break;
		case DAC_GENERATE_TRIANGLE:
			_DBG("\n\rDAC is generating 60Hz triangle wave...");
			break;
		case DAC_GENERATE_ESCALATOR:
			_DBG("\n\rDAC is generating 60Hz escalator wave...");
			break;
		default: break;
		}

		// Enable GPDMA channel 0
		GPDMA_ChannelCmd(0, ENABLE);

		_DBG_("\n\rPreass ESC if you want to terminate!");
		while(_DG!=27);

		// Disable GPDMA channel 0
		GPDMA_ChannelCmd(0, DISABLE);

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
