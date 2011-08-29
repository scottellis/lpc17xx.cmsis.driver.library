/**********************************************************************
* $Id$		uart_dma_test.c				2010-05-21
*//**
* @file		uart_dma_test.c
* @brief	This example describes how to using UART in DMA mode
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
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup UART_DMA	DMA
 * @ingroup UART_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/* Receive buffer size */
#define RX_BUF_SIZE	0x10

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"Hello NXP Semiconductors \n\r"
"UART interrupt mode demo using ring buffer \n\r\t "
"MCU LPC17xx - ARM Cortex-M3 \n\r\t "
"UART0 - 9600bps \n\r"
" This is a long string. It transferred in to DMA memory and transmit through Tx line \n\r"
" on UART0 peripheral. To use UART with DMA mode, FIFO function must be enabled \n\r";

uint8_t menu3[] = "UART demo terminated!\n";

// Receive buffer
__IO uint8_t rx_buf[RX_BUF_SIZE];

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;

// Terminal Counter flag for Channel 1
__IO uint32_t Channel1_TC;

// Error Counter flag for Channel 1
__IO uint32_t Channel1_Err;


/************************** PRIVATE FUNCTIONS *************************/
void DMA_IRQHandler (void);

void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		GPDMA interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void DMA_IRQHandler (void)
{
	uint32_t tmp;
		// Scan interrupt pending
	for (tmp = 0; tmp <= 7; tmp++) {
		if (GPDMA_IntGetStatus(GPDMA_STAT_INT, tmp)){
			// Check counter terminal status
			if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, tmp)){
				// Clear terminate counter Interrupt pending
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, tmp);

				switch (tmp){
					case 0:
						Channel0_TC++;
						GPDMA_ChannelCmd(0, DISABLE);
						break;
					case 1:
						Channel1_TC++;
						GPDMA_ChannelCmd(1, DISABLE);
						break;
					default:
						break;
				}

			}
				// Check error terminal status
			if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, tmp)){
				// Clear error counter Interrupt pending
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, tmp);
				switch (tmp){
					case 0:
						Channel0_Err++;
						GPDMA_ChannelCmd(0, DISABLE);
						break;
					case 1:
						Channel1_Err++;
						GPDMA_ChannelCmd(1, DISABLE);
						break;
					default:
						break;
				}
			}
		}
	}
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main UART program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	uint8_t *rx_char;
	uint32_t idx;
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	GPDMA_Channel_CFG_Type GPDMACfg;
	// Pin configuration for UART0
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init(LPC_UART0, &UARTConfigStruct);


	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Enable DMA mode in UART
	UARTFIFOConfigStruct.FIFO_DMAMode = ENABLE;

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(LPC_UART0, ENABLE);


	/* GPDMA Interrupt configuration section ------------------------------------------------- */

    /* Initialize GPDMA controller */
	GPDMA_Init();


	/* Setting GPDMA interrupt */
    // Disable interrupt for DMA
    NVIC_DisableIRQ (DMA_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));


	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t) &menu1;
	// Destination memory - don't care
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = sizeof(menu1);
	// Transfer width - don't care
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection - don't care
	GPDMACfg.SrcConn = 0;
	// Destination connection
	GPDMACfg.DstConn = GPDMA_CONN_UART0_Tx;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	// Setup GPDMA channel --------------------------------
	// channel 1
	GPDMACfg.ChannelNum = 1;
	// Source memory - don't care
	GPDMACfg.SrcMemAddr = 0;
	// Destination memory
	GPDMACfg.DstMemAddr = (uint32_t) &rx_buf;
	// Transfer size
	GPDMACfg.TransferSize = sizeof(rx_buf);
	// Transfer width - don't care
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	// Source connection
	GPDMACfg.SrcConn = GPDMA_CONN_UART0_Rx;
	// Destination connection - don't care
	GPDMACfg.DstConn = 0;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	GPDMA_Setup(&GPDMACfg);

	/* Reset terminal counter */
	Channel0_TC = 0;
	/* Reset Error counter */
	Channel0_Err = 0;

    // Enable interrupt for DMA
    NVIC_EnableIRQ (DMA_IRQn);

	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(0, ENABLE);
	// Make sure GPDMA channel 1 is disabled
	GPDMA_ChannelCmd(1, DISABLE);

	/* Wait for GPDMA on UART0 Tx processing complete */
    while ((Channel0_TC == 0) && (Channel0_Err == 0));

    // Main loop - echos back to the terminal
    while (1)
    {
    	/* Reset terminal counter */
    	Channel1_TC = 0;
    	/* Reset Error counter */
    	Channel1_Err = 0;

    	// Setup channel with given parameter
    	GPDMA_Setup(&GPDMACfg);

    	// Enable GPDMA channel 1
    	GPDMA_ChannelCmd(1, ENABLE);

    	// Clear Rx buffer using DMA
    	for (idx = 0; idx < RX_BUF_SIZE; idx++){
    		rx_buf[idx] = 0;
    	}

        // now, start receive character using GPDMA
        rx_char = (uint8_t *) &rx_buf;
        while ((Channel1_TC == 0) && (Channel1_Err == 0)){
			// Check whether if there's any character received, then print it back
			if (*rx_char != 0)
			{
				UART_Send(LPC_UART0, rx_char, 1, BLOCKING);
				rx_char++;
			}
        }
    }

    // DeInitialize UART0 peripheral
    UART_DeInit(LPC_UART0);

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
