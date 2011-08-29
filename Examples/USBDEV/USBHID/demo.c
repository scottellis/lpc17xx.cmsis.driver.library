/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB HID Demo
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

#include "LPC17xx.h"                        /* LPC17xx definitions */

#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"

#include "demo.h"
#include "hid.h"

#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup USBDEV_USBHID	USBHID
 * @ingroup USBDEV_Examples
 * @{
 */


uint8_t InReport;                              /* HID Input Report    */
                                            /*   Bit0   : Buttons  */
                                            /*   Bit1..7: Reserved */

uint8_t OutReport;                             /* HID Out Report      */
                                            /*   Bit0..7: LEDs     */


/*
 *  Get HID Input Report -> InReport
 */

void GetInReport (void) {

  if ((LPC_GPIO2 -> FIOPIN & PBINT) == 0) {             /* Check if PBINT is pressed */
    InReport = 0x01;
  } else {
    InReport = 0x00;
  }
}


/*
 *  Set HID Output Report <- OutReport
 */

void SetOutReport (void) {
	//Because 8 LEDs are not ordered, so we have check each bit
	//of OurReport to turn on/off LED correctly
	uint8_t led_num;
	LPC_GPIO2 -> FIOCLR = LEDMSK;
	LPC_GPIO1 -> FIOCLR = 0xF0000000;
	//LED0 (P2.6)
	led_num = OutReport & (1<<0);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<6);
	else
		LPC_GPIO2 -> FIOSET |= (1<<6);
	//LED1 (P2.5)
	led_num = OutReport & (1<<1);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<5);
	else
		LPC_GPIO2 -> FIOSET |= (1<<5);
	//LED2 (P2.4)
	led_num = OutReport & (1<<2);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<4);
	else
		LPC_GPIO2 -> FIOSET |= (1<<4);
	//LED3 (P2.3)
	led_num = OutReport & (1<<3);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<3);
	else
		LPC_GPIO2 -> FIOSET |= (1<<3);
	//LED4 (P2.2)
	led_num = OutReport & (1<<4);
	if(led_num == 0)
		LPC_GPIO2 -> FIOCLR |= (1<<2);
	else
		LPC_GPIO2 -> FIOSET |= (1<<2);
	//LED5 (P1.31)
	led_num = OutReport & (1<<5);
	if(led_num == 0)
		LPC_GPIO1 -> FIOCLR |= (1<<31);
	else
		LPC_GPIO1 -> FIOSET |= (1<<31);
	//LED6 (P1.29)
	led_num = OutReport & (1<<6);
	if(led_num == 0)
		LPC_GPIO1 -> FIOCLR |= (1<<29);
	else
		LPC_GPIO1 -> FIOSET |= (1<<29);
	//LED7 (P1.28)
	led_num = OutReport & (1<<7);
	if(led_num == 0)
		LPC_GPIO1 -> FIOCLR |= (1<<28);
	else
		LPC_GPIO1 -> FIOSET |= (1<<28);
}


/* Main Program */

int main (void) {
	LPC_GPIO2 -> FIODIR = LEDMSK;             /* LEDs, port 2, bit 0~7 output only */
	LPC_GPIO1 -> FIODIR = 0xF0000000;				/* LEDs, port 1, bit 28-31 output */

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

