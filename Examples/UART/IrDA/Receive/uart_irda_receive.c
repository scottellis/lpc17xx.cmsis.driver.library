/**********************************************************************
* $Id$		uart_irda_receive.c				2010-06-07
*//**
* @file		uart_irda_receive.c
* @brief	This example describes how to using UART in irDA mode
* @version	2.0
* @date		07. June. 2010
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
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup UART_IrDA_Receive	Receive
 * @ingroup UART_IrDA_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define UART_PORT 0

#if (UART_PORT == 0)
#define TEST_UART LPC_UART0
#elif (UART_PORT == 1)
#define TEST_UART (LPC_UART_TypeDef *)UART1
#endif
#define TEST_IRDA LPC_UART3

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] = "Hello NXP Semiconductors \n\r";
uint8_t menu2[] = "UART IrDA mode demo \n\r\t MCU LPC17xx - ARM Cortex-M3 \n\r\t UART0 - 9600bps communicates with PC \n\r\t UART3 - 9600bps receives infrared signal\n\r";
uint8_t menu3[] = "\t The demo keep reading input infrared signal and display through 8 led bank \n\r";
/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	UART_Send(TEST_UART, menu1, sizeof(menu1), BLOCKING);
	UART_Send(TEST_UART, menu2, sizeof(menu2), BLOCKING);
	UART_Send(TEST_UART, menu3, sizeof(menu3), BLOCKING);
}



/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main UART program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration for UART0
	PINSEL_CFG_Type PinCfg;
	uint32_t len;
	uint32_t led_mask[] = { 1<<28, 1<<29, 1UL<<31, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6 };
	uint8_t buffer,i;

	//Initialize for 8 led bank
	GPIO_SetDir(1, 0xB0000000, 1);           /* LEDs on PORT1 defined as Output    */
	GPIO_SetDir(2, 0x0000007C, 1);           /* LEDs on PORT2 defined as Output    */

	GPIO_ClearValue(1, 0xB0000000);
	GPIO_ClearValue(2, 0x0000007C);

#if (UART_PORT == 0)
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);//P0.2 TXD0
#endif

#if (UART_PORT == 1)
	/*
	 * Initialize UART1 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
#endif
	/*
	 * Initialize UART3 pin connect
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);//P0.26 RXD3

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	// Initialize UART0 & UART3 peripheral with given to corresponding parameter
	UART_Init(TEST_UART, &UARTConfigStruct);
	UART_Init(TEST_IRDA, &UARTConfigStruct);
	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 & UART3 peripheral
	UART_FIFOConfig(TEST_UART, &UARTFIFOConfigStruct);
	UART_FIFOConfig(TEST_IRDA, &UARTFIFOConfigStruct);
	//Configure and enable IrDA mode on UART
	UART_IrDACmd(TEST_IRDA,ENABLE);

	// Enable UART Transmit
	UART_TxCmd(TEST_UART, ENABLE);

	// print welcome screen
	print_menu();

    /* Read some data from the buffer */
    while (1)
    {
    	len=0;
    	while(len==0)
    	{
        	len = UART_Receive(TEST_IRDA, &buffer, 1, NONE_BLOCKING);
    	}
    	if(buffer!=0)
    	{
    		for(i=0;i<8;i++)
    		{
    	        if((buffer>>i)&0x01){//set
    	        	if(i<3)
    	        		GPIO_SetValue(1, led_mask[i]);
    	        	else
    	        		GPIO_SetValue(2, led_mask[i]);
    	        }
    	        else { 					//clear
    	        	if(i<3)
    	        		GPIO_ClearValue(1, led_mask[i]);
    	        	else
    	        		GPIO_ClearValue(2, led_mask[i]);
    	        }
    		}
    	}
    	else //clear 8 led bank
    	{
    		GPIO_ClearValue(1, 0xB0000000);
    		GPIO_ClearValue(2, 0x0000007C);
    	}
    }

    // wait for current transmission complete - THR must be empty
    //while (UART_CheckBusy(TEST_UART) == SET);

    // DeInitialize UART0 peripheral
    //UART_DeInit(TEST_UART);

    /* Loop forever */
    //while(1);
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
