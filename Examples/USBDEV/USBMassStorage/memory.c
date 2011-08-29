/*----------------------------------------------------------------------------
 *      Name:    MEMORY.C
 *      Purpose: USB Mass Storage Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing
 *      else gives you the right to use this software.
 *
 *      Copyright (c) 2005-2009 Keil Software.
 *---------------------------------------------------------------------------*/

#include "LPC17xx.h"

#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "mscuser.h"

#include "memory.h"

#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup USBDEV_USBMassStorage	USBMassStorage
 * @ingroup USBDEV_Examples
 * @{
 */

extern uint8_t Memory[MSC_MemorySize];         /* MSC Memory in RAM */


/* Main Program */

int main (void) {
	uint32_t n;

	for (n = 0; n < MSC_ImageSize; n++) {     /* Copy Initial Disk Image */
		Memory[n] = DiskImage[n];               /*   from Flash to RAM     */
	}

	USB_Init();                               /* USB Initialization */
	USB_Connect(TRUE);                        /* USB Connect */

	while (1);                                /* Loop forever */
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
