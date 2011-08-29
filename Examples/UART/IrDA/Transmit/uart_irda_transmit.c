/**********************************************************************
* $Id$		uart_irda_transmit.c				2010-06-07
*//**
* @file		uart_irda_transmit.c
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

/* Example group ----------------------------------------------------------- */
/** @defgroup UART_IrDA_Transmit	Transmit
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
uint8_t menu2[] = "UART IrDA mode demo \n\r\t MCU LPC17xx - ARM Cortex-M3 \n\r\t UART0 - 9600bps communicates with PC \n\r\t UART3 - 9600bps transmits infrared signals\n\r";
uint8_t menu3[] = "\t\t- Press Esc to terminate this demo \n\r\t\t- Press 'r' to re-print this menu \n\r";
uint8_t menu4[] = "\n\rUART demo terminated!";
uint8_t menu5[] = "\n\rEnter a hex byte value to transmit: 0x";
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
	// Pin configuration for UART
	PINSEL_CFG_Type PinCfg;
	uint32_t idx,len;
	__IO FlagStatus exitflag;
	uint8_t buffer,temp;

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
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);//P0.3 RXD0
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
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);//P0.25 TXD3

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
	UART_TxCmd(TEST_IRDA, ENABLE);
	// print welcome screen
	print_menu();

	// Reset exit flag
	exitflag = RESET;
	idx=0;buffer=0;

    /* Read some data from the buffer */
    while (exitflag == RESET)
    {
    	if(idx==0)
    	{
    		UART_Send(TEST_UART, menu5, sizeof(menu5), BLOCKING);
    	}
    	len=0;
    	while(len==0)
    	{
    		len = UART_Receive(TEST_UART, &temp, 1, NONE_BLOCKING);
    	}
    	if(temp==27)
    	{
    		UART_Send(TEST_UART, menu4, sizeof(menu4), BLOCKING);
    		exitflag=SET;
    	}
    	else if(temp=='r')
    	{
    		idx=0;buffer=0;
    		print_menu();
    		UART_Send(TEST_IRDA, &buffer, 1, BLOCKING);
    	}
    	else
    	{
    		idx++;
    		switch(temp)
    		{
    		case '0': buffer=(buffer<<4)|0x00;break;
    		case '1': buffer=(buffer<<4)|0x01;break;
    		case '2': buffer=(buffer<<4)|0x02;break;
    		case '3': buffer=(buffer<<4)|0x03;break;
    		case '4': buffer=(buffer<<4)|0x04;break;
    		case '5': buffer=(buffer<<4)|0x05;break;
    		case '6': buffer=(buffer<<4)|0x06;break;
    		case '7': buffer=(buffer<<4)|0x07;break;
    		case '8': buffer=(buffer<<4)|0x08;break;
    		case '9': buffer=(buffer<<4)|0x09;break;
    		case 'a': buffer=(buffer<<4)|0x0A;break;
    		case 'A': buffer=(buffer<<4)|0x0A;break;
    		case 'b': buffer=(buffer<<4)|0x0B;break;
    		case 'B': buffer=(buffer<<4)|0x0B;break;
    		case 'c': buffer=(buffer<<4)|0x0C;break;
    		case 'C': buffer=(buffer<<4)|0x0C;break;
    		case 'd': buffer=(buffer<<4)|0x0D;break;
    		case 'D': buffer=(buffer<<4)|0x0D;break;
    		case 'e': buffer=(buffer<<4)|0x0E;break;
    		case 'E': buffer=(buffer<<4)|0x0E;break;
    		case 'f': buffer=(buffer<<4)|0x0F;break;
    		case 'F': buffer=(buffer<<4)|0x0F;break;
    		default: idx=0;buffer=0;break;
    		}
    		if(idx==2)
    		{
    			temp=buffer>>4;
    			if(temp <= 9)temp=temp+ 0x30;
    			else temp=temp+0x37;
    			UART_Send(TEST_UART, &temp, 1, BLOCKING);
    			temp=(buffer&0x0F);
    			if(temp <= 9)temp=temp+ 0x30;
    			else temp=temp+0x37;
    			UART_Send(TEST_UART, &temp, 1, BLOCKING);

            	UART_Send(TEST_IRDA, &buffer, 1, BLOCKING);
            	idx=0;buffer=0;
    		}
    	}
    }
    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy(TEST_UART) == SET);
    while (UART_CheckBusy(TEST_IRDA) == SET);
    // DeInitialize UART0 & UART3 peripheral
    UART_DeInit(TEST_UART);
    UART_DeInit(TEST_IRDA);
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
