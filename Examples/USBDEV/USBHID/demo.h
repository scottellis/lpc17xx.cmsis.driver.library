/*----------------------------------------------------------------------------
 *      Name:    DEMO.H
 *      Purpose: USB HID Demo Definitions
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

/* Push Button Definitions */
#define PBINT  0x00000400  /* P2.10 */

/* LED Definitions */
#define LEDMSK 0x000000FF /* P2.0..7 */

/* HID Demo Variables */
extern uint8_t InReport;
extern uint8_t OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetOutReport (void);
