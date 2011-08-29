/**********************************************************************
* $Id$		vt_relocation.c 				2010-06-18
*//**
* @file		vt_relocation.c
* @brief	This example used to test NVIC Vector Table relocation
* 			function
* @version	1.0
* @date		18. June. 2010
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
#include <stdio.h>

#include "lpc17xx_libcfg.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_systick.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup NVIC_VecTable_Relocation	VecTable_Relocation
 * @ingroup NVIC_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
/* Vector Table Offset */
#define VTOR_OFFSET		0x20080000

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"Privileged demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example used to test NVIC Vector Table Relocation function\n\r"
	"********************************************************************************\n\r";
FunctionalState Cur_State = DISABLE;

/************************** PRIVATE FUNCTIONS *************************/

void SysTick_Handler(void);
void print_menu(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		SysTick interrupt handler
 * @param		None
 * @return 		None
 ***********************************************************************/
void SysTick_Handler(void)
{
	//Clear System Tick counter flag
	SYSTICK_ClearCounterFlag();
	//toggle P0.0
	if (Cur_State == ENABLE)
	{
		//pull-down pin
		GPIO_ClearValue(1, (1<<28));
		Cur_State = DISABLE;
	}
	else
	{
		GPIO_SetValue(1, (1<<28));
		Cur_State = ENABLE;
	}
}
/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu);
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry (void)
{
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

	//Use P0.0 to test System Tick interrupt
	GPIO_SetDir(1, (1<<28), 1); //Set P0.0 as output

	_DBG("Remapping Vector Table at address: ");
	_DBH32(VTOR_OFFSET); _DBG_("");
	NVIC_SetVTOR(VTOR_OFFSET);

	/* Copy Vector Table from 0x00000000 to new address
	 * In ROM mode: Vector Interrupt Table is initialized at 0x00000000
	 * In RAM mode: Vector Interrupt Table is initialized at 0x10000000
	 * Aligned: 256 words
	 */

#if(__RAM_MODE__==0)//Run in ROM mode
	memcpy(VTOR_OFFSET, 0x00000000, 256*4);
#else
	memcpy(VTOR_OFFSET, 0x10000000, 256*4);
#endif

	_DBG_("If Vector Table remapping is successful, LED P1.28 will blink by using SysTick interrupt");
	//Initialize System Tick with 100ms time interval
	SYSTICK_InternalInit(100);
	//Enable System Tick interrupt
	SYSTICK_IntCmd(ENABLE);
	//Enable System Tick Counter
	SYSTICK_Cmd(ENABLE);

	while(1);
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
