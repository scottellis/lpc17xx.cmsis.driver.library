/**********************************************************************
* $Id$		can_test_bypass_mode.c			2010-05-21
*//**
* @file		can_test_bypass_mode.c
* @brief	This example used to test Bypass mode
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
#include "lpc17xx_can.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup CAN_test_bypass_mode	CAN_test_bypass_mode
 * @ingroup CAN_Examples
 * @{
 */
/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"*******************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"CAN bypass demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Use two CAN peripherals: CAN1 and CAN2 to communicate\n\r"
	"This example used to test Bypass mode\n\r"
	"*******************************************************************************\n\r";

/** CAN variable definition **/
CAN_MSG_Type TXMsg, RXMsg; // messages for test Bypass mode
uint32_t CANRxCount, CANTxCount = 0;

/************************** PRIVATE FUNCTIONS *************************/
void CAN_IRQHandler(void);

void CAN_InitMessage(void);
void PrintMessage(CAN_MSG_Type* msg);
void print_menu();


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		CAN_IRQ Handler, control receive message operation
 * param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_IRQHandler()
{
	uint8_t IntStatus;
	uint32_t data1;
	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(LPC_CAN2);
	//check receive interrupt
	if((IntStatus>>0)&0x01)
	{
		CAN_ReceiveMsg(LPC_CAN2,&RXMsg);
		PrintMessage(&RXMsg);
		CANRxCount++; //count success received message
		//increase data for next TX Message
		TXMsg.id ++;
		data1 = (TXMsg.dataA[0])|(((TXMsg.dataA[1]))<<8)|((TXMsg.dataA[2])<<16)|((TXMsg.dataA[3])<<24);
		if(data1 == 0xFFFFFFFF) data1 = 0;
		else data1++;
		*((uint8_t *) &TXMsg.dataA[0])= *((uint8_t *) &TXMsg.dataB[0])= data1 & 0x000000FF;
		*((uint8_t *) &TXMsg.dataA[1])= *((uint8_t *) &TXMsg.dataB[1])=(data1 & 0x0000FF00)>>8;;
		*((uint8_t *) &TXMsg.dataA[2])= *((uint8_t *) &TXMsg.dataB[2])=(data1 & 0x00FF0000)>>16;
		*((uint8_t *) &TXMsg.dataA[3])= *((uint8_t *) &TXMsg.dataB[3])=(data1 & 0xFF000000)>>24;

		CAN_SendMsg(LPC_CAN1, &TXMsg);
	}
}

/*-------------------------PRIVATE FUNCTIONS----------------------------*/
/*********************************************************************//**
 * @brief		Print Message via COM1
 * param[in]	msg: point to CAN_MSG_Type object that will be printed
 * @return 		none
 **********************************************************************/
void PrintMessage(CAN_MSG_Type* CAN_Msg)
{
	uint32_t data;
	_DBG("Message ID:     ");
	_DBH32(CAN_Msg->id);_DBG_("");
	_DBG("Message length: ");
	_DBH32(CAN_Msg->len);_DBG_(" BYTES");
	_DBG("Message type:   ");
	if(CAN_Msg->type==DATA_FRAME)
	{
		_DBG_("DATA FRAME ");
	}
	else
		_DBG_("REMOTE FRAME ");
	_DBG("Message format: ");
	if(CAN_Msg->format==STD_ID_FORMAT)
	{
		_DBG_("STANDARD ID FRAME FORMAT");
	}
	else
		_DBG_("EXTENDED ID FRAME FORMAT");
	_DBG("Message dataA:  ");
	data = (CAN_Msg->dataA[0])|(CAN_Msg->dataA[1]<<8)|(CAN_Msg->dataA[2]<<16)|(CAN_Msg->dataA[3]<<24);
	_DBH32(data);_DBG_("");
	data = (CAN_Msg->dataB[0])|(CAN_Msg->dataB[1]<<8)|(CAN_Msg->dataB[2]<<16)|(CAN_Msg->dataB[3]<<24);
	_DBG("Message dataB:  ");
	_DBH32(data);_DBG_("");
	_DBG_("");
}

/*********************************************************************//**
 * @brief		Initialize transmit and receive message for Bypass operation
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_InitMessage(void) {
	TXMsg.format = EXT_ID_FORMAT;
	TXMsg.id = 0x00001234;
	TXMsg.len = 8;
	TXMsg.type = DATA_FRAME;
	TXMsg.dataA[0] = TXMsg.dataA[1] = TXMsg.dataA[2] = TXMsg.dataA[3] = 0x00000000;
	TXMsg.dataB[0] = TXMsg.dataB[1] = TXMsg.dataB[2] = TXMsg.dataB[3] = 0x00000000;

	RXMsg.format = 0x00;
	RXMsg.id = 0x00;
	RXMsg.len = 0x00;
	RXMsg.type = 0x00;
	RXMsg.dataA[0] = RXMsg.dataA[1] = RXMsg.dataA[2] = RXMsg.dataA[3] = 0x00000000;
	RXMsg.dataB[0] = RXMsg.dataB[1] = RXMsg.dataB[2] = RXMsg.dataB[3] = 0x00000000;
}

/*********************************************************************//**
 * @brief		print menu
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void print_menu()
{
	_DBG_(menu);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main CAN program body
 * @param[in]	none
 * @return 		int
 **********************************************************************/
int c_entry(void) { /* Main Program */
	PINSEL_CFG_Type PinCfg;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	print_menu();

	/* Pin configuration
	 * CAN1: select P0.0 as RD1. P0.1 as TD1
	 * CAN2: select P2.7 as RD2, P2.8 as RD2
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Pinnum = 7;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);

	//Initialize CAN1 & CAN2
	CAN_Init(LPC_CAN1, 125000);
	CAN_Init(LPC_CAN2, 125000);

	//Enable Interrupt
	CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);

	_DBG_("CAN test Bypass Mode function...");
	_DBG_("Press '1' to initialize CAN message...");_DBG_("");
	while(_DG !='1');
	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);
	CAN_InitMessage();
	PrintMessage(&TXMsg);
	_DBG_("Message ID and data will be increased continuously...");

	_DBG_("Press '2' to start CAN operation...");
	while(_DG !='2');

	/** To test Bypass Mode: we send infinite messages to CAN2 and check
	 * receive process via COM1
	 */
	CAN_SendMsg(LPC_CAN1, &TXMsg);

	while (1);
}

/* With ARM and GHS toolsets, the entry point is main() - this will
 allow the linker to generate wrapper code to setup stacks, allocate
 heap area, and initialize and copy code and data segments. For GNU
 toolsets, the entry point is through __start() in the crt0_gnu.asm
 file, and that startup code will setup stacks and data */
int main(void) {
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
void check_failed(uint8_t *file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
		;
}
#endif

/*
 * @}
 */
