/**********************************************************************
* $Id$		template.c 				yyyy-mm-dd
*//**
* @file		template.c
* @brief	A simple template for GNU example
* @version	x.x
* @date		dd. mm. yy
* @author	xxxx
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
/* Include files */
/* <<add include files here>> */
#include "lpc17xx_libcfg.h"

/* Add Private Types */
/* <<add private type here >> */

/* Add Private Variables */
/* <<add private variables here >> */

/* Add Private Functions */
/* <<add private functions here >> */

/**
 * @brief Main program body
 */
int c_entry(void)
{
	/* <<add code here >> */
	while(1);
	return 1;
}

/* Support required entry point for other toolchain */
int main (void)
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

