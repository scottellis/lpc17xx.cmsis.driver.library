/**********************************************************************
* $Id$		rs485_master.c			2010-05-21
*//**
* @file		rs485_master.c
* @brief	This example used to test RS485 functionality on UART1 of
* 			LPC1768.In this case, RS485 function on UART1 acts as Master
* 			on RS485 bus.
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
/** @defgroup UART_RS485_Master	RS485_Master
 * @ingroup UART_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
// Slave Address
#define SLAVE_ADDR_A 'A'
#define SLAVE_ADDR_B 'B'

/* buffer size definition */
#define UART_RING_BUFSIZE 256

/* Buf mask */
#define __BUF_MASK (UART_RING_BUFSIZE-1)
/* Check buf is full or not */
#define __BUF_IS_FULL(head, tail) ((tail&__BUF_MASK)==((head+1)&__BUF_MASK))
/* Check buf will be full in next receiving or not */
#define __BUF_WILL_FULL(head, tail) ((tail&__BUF_MASK)==((head+2)&__BUF_MASK))
/* Check buf is empty */
#define __BUF_IS_EMPTY(head, tail) ((head&__BUF_MASK)==(tail&__BUF_MASK))
/* Reset buf */
#define __BUF_RESET(bufidx)	(bufidx=0)
#define __BUF_INCR(bufidx)	(bufidx=(bufidx+1)&__BUF_MASK)

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] = "Hello NXP Semiconductors \n\r";
uint8_t menu2[] = "RS485 demo in Master mode \n\r";
uint8_t send_menu[] = "Sending... \n\r";
uint8_t recv_menu[] = "Receive: ";
uint8_t p_err_menu[] = "Parity error \n\r";
uint8_t f_err_menu[] = "Frame error \n\r";
uint8_t nextline[] = "\n\r";

uint8_t slaveA_msg[] = "Msg A: Hello NXP";
uint8_t slaveB_msg[] = "Msg B: Hello NXP";
uint8_t terminator = 13;

/************************** PRIVATE TYPES *************************/
/** @brief UART Ring buffer structure */
typedef struct
{
    __IO uint32_t tx_head;                /*!< UART Tx ring buffer head index */
    __IO uint32_t tx_tail;                /*!< UART Tx ring buffer tail index */
    __IO uint32_t rx_head;                /*!< UART Rx ring buffer head index */
    __IO uint32_t rx_tail;                /*!< UART Rx ring buffer tail index */
    __IO uint8_t  tx[UART_RING_BUFSIZE];  /*!< UART Tx data ring buffer */
    __IO uint8_t  rx[UART_RING_BUFSIZE];  /*!< UART Rx data ring buffer */
} UART_RING_BUFFER_T;

/************************** PRIVATE VARIABLES *************************/
// UART Ring buffer
UART_RING_BUFFER_T rb;

/************************** PRIVATE FUNCTIONS *************************/
void UART1_IRQHandler(void);
void UART_IntReceive(void);
void UART_IntErr(uint8_t bLSErrType);

uint32_t UARTReceive(LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint8_t buflen);
void print_menu(void);



/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		UART1 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART1_IRQHandler(void)
{
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
			UART_IntErr(tmp1);
		}
	}

	// Receive Data Available or Character time-out
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI)){
		UART_IntReceive();
	}

}

/********************************************************************//**
 * @brief 		UART receive function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void UART_IntReceive(void)
{
	uint8_t tmpc;
	uint32_t rLen;

	while(1){
		// Call UART read function in UART driver
		rLen = UART_Receive((LPC_UART_TypeDef *)LPC_UART1, &tmpc, 1, NONE_BLOCKING);
		// If data received
		if (rLen){
			/* Check if buffer is more space
			 * If no more space, remaining character will be trimmed out
			 */
			if (!__BUF_IS_FULL(rb.rx_head,rb.rx_tail)){
				rb.rx[rb.rx_head] = tmpc;
				__BUF_INCR(rb.rx_head);
			}
		}
		// no more data
		else {
			break;
		}
	}
}


/*********************************************************************//**
 * @brief		UART Line Status Error
 * @param[in]	bLSErrType	UART Line Status Error Type
 * @return		None
 **********************************************************************/
void UART_IntErr(uint8_t bLSErrType)
{
	if (bLSErrType & UART_LSR_PE){
		UART_Send(LPC_UART0, p_err_menu, sizeof(p_err_menu), BLOCKING);
	}

	if (bLSErrType & UART_LSR_FE){
		UART_Send(LPC_UART0, f_err_menu, sizeof(f_err_menu), BLOCKING);
	}
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		UART read function for interrupt mode (using ring buffers)
 * @param[in]	UARTPort	Selected UART peripheral used to send data,
 * 				should be UART0
 * @param[out]	rxbuf Pointer to Received buffer
 * @param[in]	buflen Length of Received buffer
 * @return 		Number of bytes actually read from the ring buffer
 **********************************************************************/
uint32_t UARTReceive(LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint8_t buflen)
{
    uint8_t *data = (uint8_t *) rxbuf;
    uint32_t bytes = 0;

	/* Temporarily lock out UART receive interrupts during this
	   read so the UART receive interrupt won't cause problems
	   with the index values */
	UART_IntConfig(UARTPort, UART_INTCFG_RBR, DISABLE);

	/* Loop until receive buffer ring is empty or
		until max_bytes expires */
	while ((buflen > 0) && (!(__BUF_IS_EMPTY(rb.rx_head, rb.rx_tail))))
	{
		/* Read data from ring buffer into user buffer */
		*data = rb.rx[rb.rx_tail];
		data++;

		/* Update tail pointer */
		__BUF_INCR(rb.rx_tail);

		/* Increment data count and decrement buffer size count */
		bytes++;
		buflen--;
	}

	/* Re-enable UART interrupts */
	UART_IntConfig(UARTPort, UART_INTCFG_RBR, ENABLE);

    return bytes;
}

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	UART_Send(LPC_UART0, menu1, sizeof(menu1), BLOCKING);
	UART_Send(LPC_UART0, menu2, sizeof(menu2), BLOCKING);
}



/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main UART-RS485 program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration
	PINSEL_CFG_Type PinCfg;
	// RS485 configuration
	UART1_RS485_CTRLCFG_Type rs485cfg;
	// Temp. data
	uint32_t idx, len;
	uint8_t buffer[10];
	int32_t exit_flag, addr_toggle;

	// UART0 section ----------------------------------------------------
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
	 * Baudrate = 115200 bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = 115200;

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

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(LPC_UART0, ENABLE);

	// print welcome screen
	print_menu();


	// UART1 - RS485 section -------------------------------------------------
	/*
	 * Initialize UART1 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	// TXD1 - P2.0
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	// RXD1 - P2.1
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	// DTR1 - P2.5
	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);



	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600 bps
	 * 8 data bit
	 * 1 Stop bit
	 * Parity: None
	 * Note: Parity will be enabled later in UART_RS485Config() function.
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	// Initialize UART0 peripheral with given to corresponding parameter
	UART_Init((LPC_UART_TypeDef *)LPC_UART1, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART1, &UARTFIFOConfigStruct);

	// Configure RS485
	/*
	 * - Auto Direction in Tx/Rx driving is enabled
	 * - Direction control pin is set to DTR1
	 * - Direction control pole is set to "1" that means direction pin
	 * will drive to high state before transmit data.
	 * - Multidrop mode is disable
	 * - Auto detect address is disabled
	 * - Receive state is enable
	 */
	rs485cfg.AutoDirCtrl_State = ENABLE;
	rs485cfg.DirCtrlPin = UART1_RS485_DIRCTRL_DTR;
	rs485cfg.DirCtrlPol_Level = SET;
	rs485cfg.DelayValue = 50;
	rs485cfg.NormalMultiDropMode_State = DISABLE;
	rs485cfg.AutoAddrDetect_State = DISABLE;
	rs485cfg.MatchAddrValue = 0;
	rs485cfg.Rx_State = ENABLE;
	UART_RS485Config(LPC_UART1, &rs485cfg);

	/* Enable UART Rx interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_RBR, ENABLE);
	/* Enable UART line status interrupt */
	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART1, UART_INTCFG_RLS, ENABLE);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART0 channel */
	NVIC_EnableIRQ(UART1_IRQn);

	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART1, ENABLE);

	addr_toggle = 1;
	// for testing...
	while (1){

		// Send slave addr -----------------------------------------
		UART_Send(LPC_UART0, send_menu, sizeof(send_menu), BLOCKING);
		// Send slave addr on RS485 bus
		if (addr_toggle){
			UART_RS485SendSlvAddr(LPC_UART1, SLAVE_ADDR_A);
		} else {
			UART_RS485SendSlvAddr(LPC_UART1, SLAVE_ADDR_B);
		}
		// delay for a while
		for (len = 0; len < 1000; len++);

		// Send data -----------------------------------------------
		if (addr_toggle){
			UART_RS485SendData(LPC_UART1, slaveA_msg, sizeof(slaveA_msg));
		} else {
			UART_RS485SendData(LPC_UART1, slaveB_msg, sizeof(slaveB_msg));
		}
		// Send terminator
		UART_RS485SendData(LPC_UART1, &terminator, 1);
		// delay for a while
		 for (len = 0; len < 1000; len++);

		 // Receive data from slave --------------------------------
		UART_Send(LPC_UART0, recv_menu, sizeof(recv_menu), BLOCKING);
		// If address 'A' required response...
		if (addr_toggle){
			exit_flag = 0;
			while (!exit_flag){
				len = UARTReceive((LPC_UART_TypeDef *)LPC_UART1, buffer, sizeof(buffer));
				/* Got some data */
				idx = 0;
				while (idx < len)
				{
					if (buffer[idx] == 13){
						exit_flag = 1;
					} else {
						/* Echo it back */
						UART_Send(LPC_UART0, &buffer[idx], 1, BLOCKING);
					}
					idx++;
				}
			}
		}

		UART_Send(LPC_UART0, nextline, sizeof(nextline), BLOCKING);
		addr_toggle = (addr_toggle ? 0 : 1);
		// long delay here
		for (len = 0; len < 10000000; len++);
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
