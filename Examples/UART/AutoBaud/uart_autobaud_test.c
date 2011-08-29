/**********************************************************************
* $Id$		uart_autobaud_test.c				2010-05-21
*//**
* @file		uart_autobaud_test.c
* @brief	This example describes how to configure UART using auto-baud
* 			rate in interrupt mode
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
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup UART_AutoBaud	AutoBaud
 * @ingroup UART_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t syncmenu[] = "AutoBaudrate Status: Synchronous! \n\r";
uint8_t menu1[] = "Hello NXP Semiconductors \n\r";
uint8_t menu2[] =
"UART Auto Baudrate demo\n\r\t "
"MCU LPC17xx - ARM Cortex-M3 \n\r\t "
"UART0 - Auto Baud rate mode used \n\r";
uint8_t menu3[] = "UART demo terminated!\n";

/* Synchronous Flag */
__IO FlagStatus Synchronous;

/************************** PRIVATE FUNCTIONS *************************/
void UART0_IRQHandler(void);

void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief	UART0 interrupt handler sub-routine
 * @param	None
 * @return	None
 **********************************************************************/
void UART0_IRQHandler(void)
{
	// Call Standard UART 0 interrupt handler
	uint32_t intsrc, tmp, tmp1;

	/* Determine the interrupt source */
	intsrc = UART_GetIntId(LPC_UART0);
	tmp = intsrc & UART_IIR_INTID_MASK;

	// Receive Line Status
	if (tmp == UART_IIR_INTID_RLS){
		// Check line status
		tmp1 = UART_GetLineStatus(LPC_UART0);
		// Mask out the Receive Ready and Transmit Holding empty status
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		// If any error exist
		if (tmp1) {

			while(tmp1){
				; //implement error handling here
			}
		}
	}


	intsrc &= (UART_IIR_ABEO_INT | UART_IIR_ABTO_INT);
	// Check if End of auto-baudrate interrupt or Auto baudrate time out
	if (intsrc){
		// Clear interrupt pending
		if(intsrc & UART_IIR_ABEO_INT)
			UART_ABClearIntPending(LPC_UART0, UART_AUTOBAUD_INTSTAT_ABEO);
		if (intsrc & UART_IIR_ABTO_INT)
			UART_ABClearIntPending(LPC_UART0, UART_AUTOBAUD_INTSTAT_ABTO);
			if (Synchronous == RESET)
			{
				/* Interrupt caused by End of auto-baud */
				if (intsrc & UART_AUTOBAUD_INTSTAT_ABEO){
					// Disable AB interrupt
					UART_IntConfig(LPC_UART0, UART_INTCFG_ABEO, DISABLE);
					// Set Sync flag
					Synchronous = SET;
				}

				/* Auto-Baudrate Time-Out interrupt (not implemented) */
				if (intsrc & UART_AUTOBAUD_INTSTAT_ABTO) {
					/* Just clear this bit - Add your code here */
					UART_ABClearIntPending(LPC_UART0, UART_AUTOBAUD_INTSTAT_ABTO);
				}
			}
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
	UART_Send(LPC_UART0, menu1, sizeof(menu1), BLOCKING);
	UART_Send(LPC_UART0, menu2, sizeof(menu2), BLOCKING);
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
	// Auto baudrate configuration structure
	UART_AB_CFG_Type ABConfig;

	uint32_t idx, len;
	__IO FlagStatus exitflag;
	uint8_t buffer[10];

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

	/* Initialize UART0 peripheral with given to corresponding parameter
	 * in this case, don't care the baudrate value UART initialized
	 * since this will be determine when running auto baudrate
	 */
	UART_Init(LPC_UART0, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);


	// Enable UART Transmit
	UART_TxCmd(LPC_UART0, ENABLE);


    /* Enable UART End of Auto baudrate interrupt */
	UART_IntConfig(LPC_UART0, UART_INTCFG_ABEO, ENABLE);
	/* Enable UART Auto baudrate timeout interrupt */
	UART_IntConfig(LPC_UART0, UART_INTCFG_ABTO, ENABLE);

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART0 channel */
    NVIC_EnableIRQ(UART0_IRQn);


/* ---------------------- Auto baud rate section ----------------------- */
	// Reset Synchronous flag for auto-baudrate mode
	Synchronous = RESET;

	// Configure Auto baud rate mode
    ABConfig.ABMode = UART_AUTOBAUD_MODE0;
    ABConfig.AutoRestart = ENABLE;

    // Start auto baudrate mode
    UART_ABCmd(LPC_UART0, &ABConfig, ENABLE);
    print_menu();

    /* Loop until auto baudrate mode complete */
    while (Synchronous == RESET);


    // Print status of auto baudrate
    UART_Send(LPC_UART0, syncmenu, sizeof(syncmenu), BLOCKING);
/* ---------------------- End of Auto baud rate section ----------------------- */

	// print welcome screen
	print_menu();

	// reset exit flag
	exitflag = RESET;

    /* Read some data from the buffer */
    while (exitflag == RESET)
    {
       len = 0;
        while (len == 0)
        {
            len = UART_Receive(LPC_UART0, buffer, sizeof(buffer), NONE_BLOCKING);
        }

        /* Got some data */
        idx = 0;
        while (idx < len)
        {
            if (buffer[idx] == 27)
            {
                /* ESC key, set exit flag */
            	UART_Send(LPC_UART0, menu3, sizeof(menu3), BLOCKING);
                exitflag = SET;
            }
            else if (buffer[idx] == 'r')
            {
                print_menu();
            }
            else
            {
                /* Echo it back */
            	UART_Send(LPC_UART0, &buffer[idx], 1, BLOCKING);
            }
            idx++;
        }
    }

    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy(LPC_UART0) == SET);

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
