/**********************************************************************
* $Id$		ssp_dma.c					2010-05-21
*//**
* @file		ssp_dma.c
* @brief	This example describes how to use SPP in Master mode with
* 			loop-back mode (MOSI <-> MISO), using DMA for both Tx and
* 			Rx channel
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
#include "lpc17xx_ssp.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SSP_dma	dma
 * @ingroup SSP_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
/* For DMA controller */
#define DMA_DATA_SIZE	65

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"SSP demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
" This example uses SSP function in MASTER mode \n\r"
" with Loop-back mode (MOSI <-> MISO) \n\r"
" Transfer 64 bytes of data (in DMA mode for both Tx and Rx \n\r"
" channel) \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";

// SSP Configuration structure variable
SSP_CFG_Type SSP_ConfigStruct;

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;

// Terminal Counter flag for Channel 1
__IO uint32_t Channel1_TC;

// Error Counter flag for Channel 1
__IO uint32_t Channel1_Err;

// DMA source variable
uint8_t dma_src[DMA_DATA_SIZE];

// DMA source variable
uint8_t dma_dst[DMA_DATA_SIZE];


/************************** PRIVATE FUNCTIONS *************************/
void DMA_IRQHandler (void);

void print_menu(void);
void Buffer_Init(void);
void Buffer_Verify(void);
void Error_Loop(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		GPDMA interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void DMA_IRQHandler (void)
{
	// check GPDMA interrupt on channel 0
	if (GPDMA_IntGetStatus(GPDMA_STAT_INT, 0)){
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)){
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 0);
				Channel0_TC++;
		}
		// Check error terminal status
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0)){
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 0);
			Channel0_Err++;
		}
	}
	// check GPDMA interrupt on channel 1
	if (GPDMA_IntGetStatus(GPDMA_STAT_INT, 1)){
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 1)){
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 1);
				Channel1_TC++;
		}
		// Check error terminal status
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 1)){
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 1);
			Channel1_Err++;
		}
	}
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Init(void)
{
	uint32_t i;
	uint8_t *src_addr = (uint8_t *)dma_src;
	uint8_t *dest_addr = (uint8_t *)dma_dst;

	 for ( i = 0; i < DMA_DATA_SIZE; i++ )
	{
		*src_addr++ = i;
		*dest_addr++ = 0;
	}
}

/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Verify(void)
{
	uint32_t i;
	uint8_t *src_addr = (uint8_t *)dma_src;
	uint8_t *dest_addr = (uint8_t *)dma_dst;

	for ( i = 0; i < DMA_DATA_SIZE; i++ )
	{
		if ( *src_addr++ != *dest_addr++ )
		{
			/* Call Error Loop */
			_DBG_("Verify error");
			Error_Loop();
		}
	}
}

/*********************************************************************//**
 * @brief		Error Loop (called by Buffer_Verify() if any error)
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Error_Loop(void)
{
	/* Loop forever */
	while (1);
}

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main SSP program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	GPDMA_Channel_CFG_Type GPDMACfg;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SSP pin connect
	 * P0.15 - SCK;
	 * P0.16 - SSEL
	 * P0.17 - MISO
	 * P0.18 - MOSI
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 18;
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

	/* Initializing SSP device section ------------------------------------------------------ */
	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);
	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP0, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP0, ENABLE);


	/* GPDMA Interrupt configuration section ------------------------------------------------- */
	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));
	/* Enable SSP0 interrupt */
	NVIC_EnableIRQ(DMA_IRQn);


	/* Initializing Buffer section ----------------------------------------------------------- */
	Buffer_Init();

    /* Initialize GPDMA controller */
	GPDMA_Init();


	/* Setting GPDMA interrupt */
    // Disable interrupt for DMA
    NVIC_DisableIRQ (DMA_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));


    /* Configure GPDMA channel 0 -------------------------------------------------------------*/
    /* DMA Channel 0 */
    GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t) &dma_src;
	// Destination memory - Not used
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = sizeof(dma_src);
	// Transfer width - not used
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection
	GPDMACfg.DstConn = GPDMA_CONN_SSP0_Tx;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	/* Reset terminal counter */
	Channel0_TC = 0;
	/* Reset Error counter */
	Channel0_Err = 0;


    /* Configure GPDMA channel 1 -------------------------------------------------------------*/
    /* DMA Channel 1 */
	GPDMACfg.ChannelNum = 1;
	// Source memory - not used
	GPDMACfg.SrcMemAddr = 0;
	// Destination memory - Not used
	GPDMACfg.DstMemAddr = (uint32_t) &dma_dst;
	// Transfer size
	GPDMACfg.TransferSize = sizeof(dma_dst);
	// Transfer width - not used
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	// Source connection
	GPDMACfg.SrcConn = GPDMA_CONN_SSP0_Rx;
	// Destination connection - not used
	GPDMACfg.DstConn = 0;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	/* Reset terminal counter */
	Channel1_TC = 0;
	/* Reset Error counter */
	Channel1_Err = 0;

	_DBG_("Start transfer...");

    // Enable Tx and Rx DMA on SSP0
	SSP_DMACmd (LPC_SSP0, SSP_DMA_RX, ENABLE);
	SSP_DMACmd (LPC_SSP0, SSP_DMA_TX, ENABLE);

	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(0, ENABLE);
	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(1, ENABLE);

    // Enable interrupt for DMA
    NVIC_EnableIRQ (DMA_IRQn);

	/* Wait for GPDMA processing complete */
	while (((Channel0_TC == 0) && (Channel0_Err == 0)) \
			|| ((Channel1_TC == 0) && (Channel1_Err ==0)));

	/* Verify buffer */
	Buffer_Verify();

	_DBG_("Verify complete!");

    /* Loop forever */
    while(1);
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
