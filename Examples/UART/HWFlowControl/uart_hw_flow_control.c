/**********************************************************************
* $Id$		uart_hw_flow_control.c				2010-06-10
*//**
* @file		uart_hw_flow_control.c
* @brief	This example describes how to using UART Hardware flow
* 			control mode
* @version	2.0
* @date		10. June. 2010
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
/** @defgroup UART_HWFlowControl	HWFlowControl
 * @ingroup UART_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define TEST_UART (LPC_UART_TypeDef *)LPC_UART1

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] = "Hello NXP Semiconductors \n\r";
uint8_t menu2[] = "UART hardware flow control mode demo \n\r\t MCU LPC17xx - ARM Cortex-M3 \n\r\t UART1 - 9600bps \n\r";
uint8_t menu3[] = "UART demo terminated!";

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

	uint32_t idx, len;
	__IO FlagStatus exitflag;
	uint8_t buffer[10];

	/*
	 * Initialize UART1 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);//P2.0 TXD1
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);//P2.1 RXD1
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);//P2.2 CTS1
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);//P2.7 RTS1

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init(TEST_UART, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(TEST_UART, &UARTFIFOConfigStruct);

	// Configure UART1 hardware flow control RTS/CTS
	UART_FullModemForcePinState((LPC_UART1_TypeDef *)LPC_UART1,UART1_MODEM_PIN_RTS,ACTIVE);

	// Enable UART Transmit
	UART_TxCmd(TEST_UART, ENABLE);

	// print welcome screen
	print_menu();

	// Reset exit flag
	exitflag = RESET;

    /* Read some data from the buffer */
    while (exitflag == RESET)
    {
       len = 0;
        while (len == 0)
        {
            len = UART_Receive(TEST_UART, buffer, sizeof(buffer), NONE_BLOCKING);
        }

        /* Got some data */
        idx = 0;
        while (idx < len)
        {
            if (buffer[idx] == 27)
            {
                /* ESC key, set exit flag */
            	UART_Send(TEST_UART, menu3, sizeof(menu3), BLOCKING);
                exitflag = SET;
            }
            else if (buffer[idx] == 'r')
            {
                print_menu();
            }
            else
            {
                /* Echo it back */
            	UART_Send(TEST_UART, &buffer[idx], 1, BLOCKING);
            }
            idx++;
        }
    }

    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy(TEST_UART) == SET);

    // DeInitialize UART0 peripheral
    UART_DeInit(TEST_UART);

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
