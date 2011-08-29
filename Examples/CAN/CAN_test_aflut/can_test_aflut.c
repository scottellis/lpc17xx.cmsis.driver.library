/**********************************************************************
* $Id$		can_AFLUT_dynamic.c			2010-05-21
*//**
* @file		can_AFLUT_dynamic.c
* @brief	This example used to test acceptance filter operation and
* 			functions that support load/remove AFLUT entry dynamically
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
/** @defgroup CAN_test_aflut	CAN_test_aflut
 * @ingroup CAN_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS*************************/
#define MAX_ENTRY			512
#define CAN_TX_MSG_CNT		10
#define CAN_RX_MSG_CNT		5

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"CAN demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Use 2 CAN peripherals: CAN1&CAN2 to transfer data\n\r"
	"This example tests full Acceptance Filter operation \n\r"
	"and load/remove AFLUT entry dynamically functions \n\r"
	"********************************************************************************\n\r";

//messages for test Acceptance Filter mode
CAN_MSG_Type AFTxMsg[CAN_TX_MSG_CNT], AFRxMsg[CAN_RX_MSG_CNT];
uint32_t CANRxCount = 0, CANTxCount = 0;
uint32_t CANErrCount = 0;

AF_SectionDef AFTable;
FullCAN_Entry FullCAN_Table[6];
SFF_Entry SFF_Table[6];
SFF_GPR_Entry SFF_GPR_Table[6];
EFF_Entry EFF_Table[6];
EFF_GPR_Entry EFF_GPR_Table[6];

/************************** PRIVATE FUNCTIONS *************************/
/* CAN interrupt service routine */
void CAN_IRQHandler(void);

void CAN_PinCfg(void);
void CAN_InitMessage(void);
void CAN_SetupAFTable(void);
void CAN_InitAFMessage(void);
void PrintMessage(CAN_MSG_Type* msg);
void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		CAN IRQ Handler
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_IRQHandler(void)
{
	uint8_t IntStatus;
	//check FullCAN interrupt enable or not
	if(CAN_FullCANIntGetStatus(LPC_CANAF)== SET)
	{	//check is FullCAN interrupt occurs or not
		if ((CAN_FullCANPendGetStatus(LPC_CANAF,FULLCAN_IC0))
				||(CAN_FullCANPendGetStatus(LPC_CANAF,FULLCAN_IC1)))
		{
			//read received FullCAN Object in Object Section
			FCAN_ReadObj(LPC_CANAF, &AFRxMsg[CANRxCount]);
			CANRxCount++;
		}
	}
	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(LPC_CAN2);
	//check receive interrupt
	if((IntStatus>>0)&0x01)
	{
		CAN_ReceiveMsg(LPC_CAN2, &AFRxMsg[CANRxCount]);
		CANRxCount++;
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
 * @brief		Init AF-Look Up Table Sections entry value
 * 				We setup entries for 5 sections:
 * 				- 6 entries for FullCAN Frame Format Section
 * 				- 6 entries for Explicit Standard ID Frame Format Section
 * 				- 6 entries for Group of Standard ID Frame Format Section
 * 				- 6 entries for Explicit Extended ID Frame Format Section
 * 				- 6 entries for Group of Extended ID Frame Format Section
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_SetupAFTable(void) {
	FullCAN_Table[0].controller = CAN2_CTRL;
	FullCAN_Table[0].disable = MSG_ENABLE;
	FullCAN_Table[0].id_11 = 0x01;
	FullCAN_Table[1].controller = CAN2_CTRL;
	FullCAN_Table[1].disable = MSG_ENABLE;
	FullCAN_Table[1].id_11 = 0x02;
	FullCAN_Table[2].controller = CAN2_CTRL;
	FullCAN_Table[2].disable = MSG_ENABLE;
	FullCAN_Table[2].id_11 = 0x03;
	FullCAN_Table[3].controller = CAN2_CTRL;
	FullCAN_Table[3].disable = MSG_ENABLE;
	FullCAN_Table[3].id_11 = 0x06;
	FullCAN_Table[4].controller = CAN2_CTRL;
	FullCAN_Table[4].disable = MSG_ENABLE;
	FullCAN_Table[4].id_11 = 0x0C;
	FullCAN_Table[5].controller = CAN2_CTRL;
	FullCAN_Table[5].disable = MSG_ENABLE;
	FullCAN_Table[5].id_11 = 0x0D;

	SFF_Table[0].controller = CAN2_CTRL;
	SFF_Table[0].disable = MSG_ENABLE;
	SFF_Table[0].id_11 = 0x08;
	SFF_Table[1].controller = CAN2_CTRL;
	SFF_Table[1].disable = MSG_ENABLE;
	SFF_Table[1].id_11 = 0x09;
	SFF_Table[2].controller = CAN2_CTRL;
	SFF_Table[2].disable = MSG_ENABLE;
	SFF_Table[2].id_11 = 0x0A;
	SFF_Table[3].controller = CAN2_CTRL;
	SFF_Table[3].disable = MSG_ENABLE;
	SFF_Table[3].id_11 = 0x0B;
	SFF_Table[4].controller = CAN2_CTRL;
	SFF_Table[4].disable = MSG_ENABLE;
	SFF_Table[4].id_11 = 0x0E;
	SFF_Table[5].controller = CAN2_CTRL;
	SFF_Table[5].disable = MSG_ENABLE;
	SFF_Table[5].id_11 = 0x0F;

	SFF_GPR_Table[0].controller1 = SFF_GPR_Table[0].controller2 = CAN2_CTRL;
	SFF_GPR_Table[0].disable1 = SFF_GPR_Table[0].disable2 = MSG_ENABLE;
	SFF_GPR_Table[0].lowerID = 0x10;
	SFF_GPR_Table[0].upperID = 0x20;
	SFF_GPR_Table[1].controller1 = SFF_GPR_Table[1].controller2 = CAN2_CTRL;
	SFF_GPR_Table[1].disable1 = SFF_GPR_Table[1].disable2 = MSG_ENABLE;
	SFF_GPR_Table[1].lowerID = 0x20;
	SFF_GPR_Table[1].upperID = 0x25;
	SFF_GPR_Table[2].controller1 = SFF_GPR_Table[2].controller2 = CAN2_CTRL;
	SFF_GPR_Table[2].disable1 = SFF_GPR_Table[2].disable2 = MSG_ENABLE;
	SFF_GPR_Table[2].lowerID = 0x30;
	SFF_GPR_Table[2].upperID = 0x40;
	SFF_GPR_Table[3].controller1 = SFF_GPR_Table[3].controller2 = CAN2_CTRL;
	SFF_GPR_Table[3].disable1 = SFF_GPR_Table[3].disable2 = MSG_ENABLE;
	SFF_GPR_Table[3].lowerID = 0x40;
	SFF_GPR_Table[3].upperID = 0x50;
	SFF_GPR_Table[4].controller1 = SFF_GPR_Table[4].controller2 = CAN2_CTRL;
	SFF_GPR_Table[4].disable1 = SFF_GPR_Table[4].disable2 = MSG_ENABLE;
	SFF_GPR_Table[4].lowerID = 0x50;
	SFF_GPR_Table[4].upperID = 0x60;
	SFF_GPR_Table[5].controller1 = SFF_GPR_Table[5].controller2 = CAN2_CTRL;
	SFF_GPR_Table[5].disable1 = SFF_GPR_Table[5].disable2 = MSG_ENABLE;
	SFF_GPR_Table[5].lowerID = 0x60;
	SFF_GPR_Table[5].upperID = 0x70;

	EFF_Table[0].controller = CAN2_CTRL;
	EFF_Table[0].ID_29 = (1 << 11);
	EFF_Table[1].controller = CAN2_CTRL;
	EFF_Table[1].ID_29 = (2 << 11);
	EFF_Table[2].controller = CAN2_CTRL;
	EFF_Table[2].ID_29 = (3 << 11);
	EFF_Table[3].controller = CAN2_CTRL;
	EFF_Table[3].ID_29 = (4 << 11);
	EFF_Table[4].controller = CAN2_CTRL;
	EFF_Table[4].ID_29 = (0x0e << 11);
	EFF_Table[5].controller = CAN2_CTRL;
	EFF_Table[5].ID_29 = (0x0f << 11);

	EFF_GPR_Table[0].controller1 = EFF_GPR_Table[0].controller2 = CAN2_CTRL;
	EFF_GPR_Table[0].lowerEID = (5 << 11);
	EFF_GPR_Table[0].upperEID = (6 << 11);
	EFF_GPR_Table[1].controller1 = EFF_GPR_Table[1].controller2 = CAN2_CTRL;
	EFF_GPR_Table[1].lowerEID = (7 << 11);
	EFF_GPR_Table[1].upperEID = (8 << 11);
	EFF_GPR_Table[2].controller1 = EFF_GPR_Table[2].controller2 = CAN2_CTRL;
	EFF_GPR_Table[2].lowerEID = (9 << 11);
	EFF_GPR_Table[2].upperEID = (0x0a << 11);
	EFF_GPR_Table[3].controller1 = EFF_GPR_Table[3].controller2 = CAN2_CTRL;
	EFF_GPR_Table[3].lowerEID = (0x0b << 11);
	EFF_GPR_Table[3].upperEID = (0x0c << 11);
	EFF_GPR_Table[4].controller1 = EFF_GPR_Table[4].controller2 = CAN2_CTRL;
	EFF_GPR_Table[4].lowerEID = (0x11 << 11);
	EFF_GPR_Table[4].upperEID = (0x12 << 11);
	EFF_GPR_Table[5].controller1 = EFF_GPR_Table[5].controller2 = CAN2_CTRL;
	EFF_GPR_Table[5].lowerEID = (0x13 << 11);
	EFF_GPR_Table[5].upperEID = (0x14 << 11);

	AFTable.FullCAN_Sec = &FullCAN_Table[0];
	AFTable.FC_NumEntry = 6;
	AFTable.SFF_Sec = &SFF_Table[0];
	AFTable.SFF_NumEntry = 6;
	AFTable.SFF_GPR_Sec = &SFF_GPR_Table[0];
	AFTable.SFF_GPR_NumEntry = 6;
	AFTable.EFF_Sec = &EFF_Table[0];
	AFTable.EFF_NumEntry = 6;
	AFTable.EFF_GPR_Sec = &EFF_GPR_Table[0];
	AFTable.EFF_GPR_NumEntry = 6;
}
/*********************************************************************//**
 * @brief		Change AFLUT table
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_ChangeAFTable(void)
{
	CAN_LoadFullCANEntry(LPC_CAN2, 4);
	CAN_LoadExplicitEntry(LPC_CAN2, 5, STD_ID_FORMAT);
	CAN_LoadGroupEntry(LPC_CAN2,0x25,0x30, STD_ID_FORMAT);
	CAN_LoadExplicitEntry(LPC_CAN2, (3<<11)+0x05, EXT_ID_FORMAT);
	CAN_LoadGroupEntry(LPC_CAN2,(0x0a<<11),(0x0b<<11), EXT_ID_FORMAT);

	CAN_RemoveEntry(FULLCAN_ENTRY, 0);
	CAN_RemoveEntry(EXPLICIT_STANDARD_ENTRY, 0);
	CAN_RemoveEntry(GROUP_STANDARD_ENTRY, 0);
	CAN_RemoveEntry(EXPLICIT_EXTEND_ENTRY, 2);
	CAN_RemoveEntry(GROUP_EXTEND_ENTRY, 2);

}
/*********************************************************************//**
 * @brief		Init Transmit Message
 * 				We use 10 message to test Acceptance Filter operation, include:
 * 				- 5 messages that ID exit in 5 AF Sections -> they will be receive
 * 				- 5 messages that ID not exit in 5 AF Sections -> they will be ignored
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_InitAFMessage(void) {
	/* 1st Message with 11-bit ID which exit in AF Look-up Table in FullCAN Section */
	AFTxMsg[0].id = 0x01;
	AFTxMsg[0].len = 0x08;
	AFTxMsg[0].type = DATA_FRAME;
	AFTxMsg[0].format = STD_ID_FORMAT;
	AFTxMsg[0].dataA[0] = AFTxMsg[0].dataA[1] = AFTxMsg[0].dataA[2]= AFTxMsg[0].dataA[3]= 0x78;
	AFTxMsg[0].dataB[0] = AFTxMsg[0].dataB[1] = AFTxMsg[0].dataB[2]= AFTxMsg[0].dataB[3]= 0x21;

	/* 2nd Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[1].id = 0x04;
	AFTxMsg[1].len = 0x08;
	AFTxMsg[1].type = DATA_FRAME;
	AFTxMsg[1].format = STD_ID_FORMAT;
	AFTxMsg[1].dataA[0] = AFTxMsg[1].dataA[1] = AFTxMsg[1].dataA[2]= AFTxMsg[1].dataA[3]= 0x23;
	AFTxMsg[1].dataB[0] = AFTxMsg[1].dataB[1] = AFTxMsg[1].dataB[2]= AFTxMsg[1].dataB[3]= 0x45;

	/* 3th Message with 11-bit ID which exit in AF Look-up Table in SFF Section*/
	AFTxMsg[2].id = 0x08;
	AFTxMsg[2].len = 0x08;
	AFTxMsg[2].type = DATA_FRAME;
	AFTxMsg[2].format = STD_ID_FORMAT;
	AFTxMsg[2].dataA[0] = AFTxMsg[2].dataA[1] = AFTxMsg[2].dataA[2]= AFTxMsg[2].dataA[3]= 0x15;
	AFTxMsg[2].dataB[0] = AFTxMsg[2].dataB[1] = AFTxMsg[2].dataB[2]= AFTxMsg[2].dataB[3]= 0x36;

	/* 4th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[3].id = 0x05;
	AFTxMsg[3].len = 0x08;
	AFTxMsg[3].type = DATA_FRAME;
	AFTxMsg[3].format = STD_ID_FORMAT;
	AFTxMsg[3].dataA[0] = AFTxMsg[3].dataA[1] = AFTxMsg[3].dataA[2]= AFTxMsg[3].dataA[3]= 0x78;
	AFTxMsg[3].dataB[0] = AFTxMsg[3].dataB[1] = AFTxMsg[3].dataB[2]= AFTxMsg[3].dataB[3]= 0x21;

	/* 5th Message with 11-bit ID which exit in AF Look-up Table in Group SFF Section*/
	AFTxMsg[4].id = 0x15;
	AFTxMsg[4].len = 0x08;
	AFTxMsg[4].type = DATA_FRAME;
	AFTxMsg[4].format = STD_ID_FORMAT;
	AFTxMsg[4].dataA[0] = AFTxMsg[4].dataA[1] = AFTxMsg[4].dataA[2]= AFTxMsg[4].dataA[3]= 0x65;
	AFTxMsg[4].dataB[0] = AFTxMsg[4].dataB[1] = AFTxMsg[4].dataB[2]= AFTxMsg[4].dataB[3]= 0x37;

	/* 6th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[5].id = 0x26;
	AFTxMsg[5].len = 0x08;
	AFTxMsg[5].type = DATA_FRAME;
	AFTxMsg[5].format = STD_ID_FORMAT;
	AFTxMsg[5].dataA[0] = AFTxMsg[5].dataA[1] = AFTxMsg[5].dataA[2]= AFTxMsg[5].dataA[3]= 0x76;
	AFTxMsg[5].dataB[0] = AFTxMsg[5].dataB[1] = AFTxMsg[5].dataB[2]= AFTxMsg[5].dataB[3]= 0x32;

	/* 7th Message with 29-bit ID which exit in AF Look-up Table in EFF Section */
	AFTxMsg[6].id = (3 << 11); //0x00001800
	AFTxMsg[6].len = 0x08;
	AFTxMsg[6].type = DATA_FRAME;
	AFTxMsg[6].format = EXT_ID_FORMAT;
	AFTxMsg[6].dataA[0] = AFTxMsg[6].dataA[1] = AFTxMsg[6].dataA[2]= AFTxMsg[6].dataA[3]= 0x45;
	AFTxMsg[6].dataB[0] = AFTxMsg[6].dataB[1] = AFTxMsg[6].dataB[2]= AFTxMsg[6].dataB[3]= 0x87;

	/* 8th Message with 29-bit ID which not exit in AF Look-up Table */
	AFTxMsg[7].id = (3 << 11) + 0x05; //0x00001801
	AFTxMsg[7].len = 0x08;
	AFTxMsg[7].type = DATA_FRAME;
	AFTxMsg[7].format = EXT_ID_FORMAT;
	AFTxMsg[7].dataA[0] = AFTxMsg[7].dataA[1] = AFTxMsg[7].dataA[2]= AFTxMsg[7].dataA[3]= 0x78;
	AFTxMsg[7].dataB[0] = AFTxMsg[7].dataB[1] = AFTxMsg[7].dataB[2]= AFTxMsg[7].dataB[3]= 0x21;

	/* 9th Message with 29-bit ID which exit in AF Look-up Table in Group of EFF Section*/
	AFTxMsg[8].id = (9 << 11) + 0x01; //0x00004801
	AFTxMsg[8].len = 0x08;
	AFTxMsg[8].type = DATA_FRAME;
	AFTxMsg[8].format = EXT_ID_FORMAT;
	AFTxMsg[8].dataA[0] = AFTxMsg[8].dataA[1] = AFTxMsg[8].dataA[2]= AFTxMsg[8].dataA[3]= 0x52;
	AFTxMsg[8].dataB[0] = AFTxMsg[8].dataB[1] = AFTxMsg[8].dataB[2]= AFTxMsg[8].dataB[3]= 0x06;

	/* 10th Message with 29-bit ID which not exit in AF Look-up Table */
	AFTxMsg[9].id = (0x0A << 11) + 0x01; //0x00005001
	AFTxMsg[9].len = 0x08;
	AFTxMsg[9].type = DATA_FRAME;
	AFTxMsg[9].format = EXT_ID_FORMAT;
	AFTxMsg[9].dataA[0] = AFTxMsg[9].dataA[1] = AFTxMsg[9].dataA[2]= AFTxMsg[9].dataA[3]= 0x85;
	AFTxMsg[9].dataB[0] = AFTxMsg[9].dataB[1] = AFTxMsg[9].dataB[2]= AFTxMsg[9].dataB[3]= 0x27;

	AFRxMsg[0].id = AFRxMsg[1].id = AFRxMsg[2].id = AFRxMsg[3].id = AFRxMsg[4].id = 0x00;
	AFRxMsg[0].len = AFRxMsg[1].len = AFRxMsg[2].len = AFRxMsg[3].len = AFRxMsg[4].len = 0x00;
	AFRxMsg[0].type = AFRxMsg[1].type = AFRxMsg[2].type = AFRxMsg[3].type = AFRxMsg[4].type = 0x00;
	AFRxMsg[0].format = AFRxMsg[1].format = AFRxMsg[2].format = AFRxMsg[3].format = AFRxMsg[4].format = 0x00;
	AFRxMsg[0].dataA[0] = AFRxMsg[1].dataA[0] = AFRxMsg[2].dataA[0] = AFRxMsg[3].dataA[0] = AFRxMsg[4].dataA[0] = 0x00;
	AFRxMsg[0].dataA[1] = AFRxMsg[1].dataA[1] = AFRxMsg[2].dataA[1] = AFRxMsg[3].dataA[1] = AFRxMsg[4].dataA[1] = 0x00;
	AFRxMsg[0].dataA[2] = AFRxMsg[1].dataA[2] = AFRxMsg[2].dataA[2] = AFRxMsg[3].dataA[2] = AFRxMsg[4].dataA[2] = 0x00;
	AFRxMsg[0].dataA[3] = AFRxMsg[1].dataA[3] = AFRxMsg[2].dataA[3] = AFRxMsg[3].dataA[3] = AFRxMsg[4].dataA[3] = 0x00;

	AFRxMsg[0].dataB[0] = AFRxMsg[1].dataB[0] = AFRxMsg[2].dataB[0] = AFRxMsg[3].dataB[0] = AFRxMsg[4].dataB[0] = 0x00;
	AFRxMsg[0].dataB[1] = AFRxMsg[1].dataB[1] = AFRxMsg[2].dataB[1] = AFRxMsg[3].dataB[1] = AFRxMsg[4].dataB[1] = 0x00;
	AFRxMsg[0].dataB[2] = AFRxMsg[1].dataB[2] = AFRxMsg[2].dataB[2] = AFRxMsg[3].dataB[2] = AFRxMsg[4].dataB[2] = 0x00;
	AFRxMsg[0].dataB[3] = AFRxMsg[1].dataB[3] = AFRxMsg[2].dataB[3] = AFRxMsg[3].dataB[3] = AFRxMsg[4].dataB[3] = 0x00;
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
	uint32_t i;
	uint32_t cnt;
	CAN_ERROR error;
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

	//Enable Receive Interrupt
	CAN_IRQCmd(LPC_CAN2, CANINT_FCE, ENABLE);
	CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);

	/* First, we send 10 messages:
	 * - message 0,2,4,6,8 have id in AFLUT >>> will be received
	 * - message 1,3,5,7,9 don't have id in AFLUT >>> will be ignored
	 * Then, we change AFLUT by load/remove entries in AFLUT and re-send messages
	 * - message 1,3,5,7,9 have id in AFLUT >>> will be received
	 * - message 0,2,4,6,8 don't have id in AFLUT >>> will be ignored
	 * Note that: FullCAN Object must be read from FullCAN Object Section next to AFLUT
	 */
	/*-------------------------Init Message & AF Look-up Table------------------------*/

	_DBG_("Test Acceptance Filter function...");
	_DBG_("Press '1' to initialize message and AF Loop-up Table...");_DBG_("");
	while(_DG !='1');
	CAN_InitAFMessage(); /* initialize Transmit Message */
	_DBG_("Init message finished!!!");
	CAN_SetupAFTable(); /* initialize AF Look-up Table sections*/
	error = CAN_SetupAFLUT(LPC_CANAF,&AFTable); /* install AF Look-up Table */
	if (error != CAN_OK) {
		_DBG_("Setup AF: ERROR...");
		while (1); // AF Table has error
	}
	else _DBG_("Setup AF: SUCCESSFUL!!!");_DBG_("");


	/*-------------------------Send messages------------------------*/
	_DBG_("Press '2' to start CAN transferring operation...");_DBG_("");
	while(_DG !='2');
	for (i = 0; i < CAN_TX_MSG_CNT; i++) {
		CAN_SendMsg(LPC_CAN1, &AFTxMsg[i]);
		PrintMessage(&AFTxMsg[i]);_DBG_("");
		for(cnt=0;cnt<10000;cnt++); //transmit delay
		CANTxCount++;
	}
	_DBG_("Sending finished !!!");

	/*-------------------------Display Received messages------------------------*/
	_DBG_("Press '3' to display received messages...");_DBG_("");
	while(_DG !='3');
	for (i = 0; i < CAN_RX_MSG_CNT; i++) {
		PrintMessage(&AFRxMsg[i]);_DBG_("");
	}

	/*-------------------------Change AFLUT Table --------------------*/
	_DBG_("Press '4' to change AF look-up table...");_DBG_("");
	while(_DG !='4');
	CAN_ChangeAFTable();
	_DBG_("Change AFLUT: FINISHED!!!");
	CAN_SetAFMode(LPC_CANAF, CAN_eFCAN);
	CAN_InitAFMessage();
	CANRxCount = CANTxCount = 0;

	/*-------------------------Re-Send messages------------------------*/
	_DBG_("Press '5' to re-send messages...");_DBG_("");
	while(_DG !='5');
	for (i = 0; i < CAN_TX_MSG_CNT; i++) {
		CAN_SendMsg(LPC_CAN1, &AFTxMsg[i]);
		PrintMessage(&AFTxMsg[i]);_DBG_("");
		for(cnt=0;cnt<10000;cnt++); //transmit delay
		CANTxCount++;
	}

	/*-------------------------Display received messages------------------------*/
	_DBG_("Re-Sending finished !!!");

	_DBG_("Press '6' to display received messages...");_DBG_("");
	while(_DG !='6');
	for (i = 0; i < CAN_RX_MSG_CNT; i++) {
		PrintMessage(&AFRxMsg[i]);_DBG_("");
	}
	_DBG_("Demo terminal !!!");

	CAN_DeInit(LPC_CAN1);
	CAN_DeInit(LPC_CAN2);
	while (1);
	return 0;
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
