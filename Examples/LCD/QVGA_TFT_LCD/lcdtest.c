/**********************************************************************
* $Id$		lcdtest.c 				2010-05-21
*//**
* @file		lcdtest.c
* @brief	This example used to test a LCD on MCB1700 board
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
#include "lpc17xx_gpio.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "GLCD.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup LCD_QVGA_TFT_LCD	QVGA_TFT_LCD
 * @ingroup LCD_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ********************/
// Time out definition - used in blocking mode in read/write function
#define TIME_OUT	10000


/************************** PRIVATE VARIABLES *************************/
//uint8_t menu1[] =
//"********************************************************************************\n\r"
//"Hello NXP Semiconductors \n\r"
//" 4-bit LCD demo \n\r"
//"\t - MCU: LPC17xx \n\r"
//"\t - Core: ARM Cortex-M3 \n\r"
//"\t - Communicate via: UART0 - 115200bps \n\r"
//" This example used to test LCD via GPIO peripheral on LPC1768 \n\r"
//"********************************************************************************\n\r";
//uint8_t menu2[] = "Demo terminated! \n\r";

// UART Configuration structure variable
//UART_CFG_Type UARTConfigStruct;

uint8_t lcd_text[2][16+1] = {"   NXP SEMI.    ",      /* Buffer for LCD text      */
                          "  LPC1768/CM3" };

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
//void print_menu(void)
//{
//	_DBG(menu1);
//}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main LCD program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
//	/* Initialize debug via UART0
//	 * – 115200bps
//	 * – 8 data bit
//	 * – No parity
//	 * – 1 stop bit
//	 * – No flow control
//	 */
//	debug_frmwrk_init();
//
//	// print welcome screen
//	print_menu();

	/* LCD block section -------------------------------------------- */
	GLCD_Init();
	GLCD_Clear(White);

	/* Update LCD Module display text. */
	GLCD_DisplayString(0,0, lcd_text[0] );
	GLCD_DisplayString(1,2, lcd_text[1] );

    /* Loop forever */
    while(1);
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
