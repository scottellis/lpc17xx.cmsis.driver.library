/**********************************************************************
* $Id$		can_ledcontrol.c			2010-07-16
*//**
* @file		can_ledcontrol.c
* @brief	This example describes how to use CAN frames to control
* 			LED display
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
#include "lpc17xx_can.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup CAN_LedControl	CAN_LedControl
 * @ingroup CAN_Examples
 * @{
 */

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"*******************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"CAN demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Use two CAN peripherals: CAN1 and CAN2 to communicate\n\r"
	"Use CAN frames to control LED display\n\r"
	"*******************************************************************************\n\r";

/** CAN variable definition **/
CAN_MSG_Type TXMsg, RXMsg; // messages for test Bypass mode
uint8_t LED_Value;
uint32_t LED[8] = {(1<<6),(1<<5),(1<<4),(1<<3),(1<<2),(1<<31),(1<<29),(1<<28)};

/************************** PRIVATE FUNCTIONS *************************/
void CAN_IRQHandler(void);

void CAN_InitMessage(void);
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
	uint32_t data,i;
	/* get interrupt status
	 * Note that: Interrupt register CANICR will be reset after read.
	 * So function "CAN_IntGetStatus" should be call only one time
	 */
	IntStatus = CAN_IntGetStatus(LPC_CAN2);
	//check receive interrupt
	if((IntStatus>>0)&0x01)
	{
		CAN_ReceiveMsg(LPC_CAN2,&RXMsg);
		data = RXMsg.dataA[0];
		for(i=0;i<8;i++)
		{
			if ((data >> i)&0x01)
			{
				if(i<5)
					GPIO_SetValue(2,LED[i]);
				else
					GPIO_SetValue(1,LED[i]);
			}
			else
			{
				if(i<5)
					GPIO_ClearValue(2,LED[i]);
				else
					GPIO_ClearValue(1,LED[i]);
			}
		}

	}
}

/*-------------------------PRIVATE FUNCTIONS----------------------------*/
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
	_DBG(menu);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main CAN program body
 * @param[in]	none
 * @return 		int
 **********************************************************************/
int c_entry(void) { /* Main Program */
	PINSEL_CFG_Type PinCfg;
	uint32_t test;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	print_menu();

	/* LED setting */
	GPIO_SetDir(1,(1<<28)|(1<<29)|(1<<31),1);
	GPIO_SetDir(2,(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6),1);

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
	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);
	CAN_InitMessage();

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);
	while(1)
	{
		_DBG("Press LED value that you want to display \n\r"
			  "This value should be in range from 0x00 to 0xFF: ");
loop:
		LED_Value = 0;
		test = _DG;
		switch(test)
		{
		case '0': LED_Value = 0; break;
		case '1': LED_Value = 1; break;
		case '2': LED_Value = 2; break;
		case '3': LED_Value = 3; break;
		case '4': LED_Value = 4; break;
		case '5': LED_Value = 5; break;
		case '6': LED_Value = 6; break;
		case '7': LED_Value = 7; break;
		case '8': LED_Value = 8; break;
		case '9': LED_Value = 9; break;
		case 'A': LED_Value = 0x0A; break;
		case 'a': LED_Value = 0x0A; break;
		case 'B': LED_Value = 0x0B; break;
		case 'b': LED_Value = 0x0B; break;
		case 'C': LED_Value = 0x0C; break;
		case 'c': LED_Value = 0x0C; break;
		case 'D': LED_Value = 0x0D; break;
		case 'd': LED_Value = 0x0D; break;
		case 'E': LED_Value = 0x0E; break;
		case 'e': LED_Value = 0x0E; break;
		case 'F': LED_Value = 0x0F; break;
		case 'f': LED_Value = 0x0F; break;
		default:
			_DBG_("\n\rInvalid input, please type again!");
			goto loop;
		}
		test = _DG;
		switch(test)
		{
		case '0': LED_Value = (LED_Value<<4)|0; break;
		case '1': LED_Value = (LED_Value<<4)|1; break;
		case '2': LED_Value = (LED_Value<<4)|2; break;
		case '3': LED_Value = (LED_Value<<4)|3; break;
		case '4': LED_Value = (LED_Value<<4)|4; break;
		case '5': LED_Value = (LED_Value<<4)|5; break;
		case '6': LED_Value = (LED_Value<<4)|6; break;
		case '7': LED_Value = (LED_Value<<4)|7; break;
		case '8': LED_Value = (LED_Value<<4)|8; break;
		case '9': LED_Value = (LED_Value<<4)|9; break;
		case 'A': LED_Value = (LED_Value<<4)|0x0A; break;
		case 'a': LED_Value = (LED_Value<<4)|0x0A; break;
		case 'B': LED_Value = (LED_Value<<4)|0x0B; break;
		case 'b': LED_Value = (LED_Value<<4)|0x0B; break;
		case 'C': LED_Value = (LED_Value<<4)|0x0C; break;
		case 'c': LED_Value = (LED_Value<<4)|0x0C; break;
		case 'D': LED_Value = (LED_Value<<4)|0x0D; break;
		case 'd': LED_Value = (LED_Value<<4)|0x0D; break;
		case 'E': LED_Value = (LED_Value<<4)|0x0E; break;
		case 'e': LED_Value = (LED_Value<<4)|0x0E; break;
		case 'F': LED_Value = (LED_Value<<4)|0x0F; break;
		case 'f': LED_Value = (LED_Value<<4)|0x0F; break;
		default:
			_DBG_("\n\rInvalid input, please type again!");
			goto loop;
		}
		_DBH32(LED_Value);_DBG_("");
		_DBG_("Display LED... ");_DBG_("");
		TXMsg.dataA[0] = LED_Value;
		CAN_SendMsg(LPC_CAN1, &TXMsg);
	}
	return 1;
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
