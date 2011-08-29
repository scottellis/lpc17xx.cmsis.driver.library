/**********************************************************************
* $Id$		i2c_monitor.c  				2010-07-16
*//**
* @file		i2c_monitor.c
* @brief	This example describes how to uses I2C peripheral on LPC1768
* 			in monitor mode
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
#include "lpc17xx_i2c.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup I2C_Monitor		Monitor
 * @ingroup I2C_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define I2CDEV LPC_I2C0

/** Max buffer length */
#define BUFFER_SIZE			0x80
/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"I2C monitor I2C bus demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - This example describes how to uses I2C peripheral on LPC1768 \n\r"
"\t in monitor mode. \n\r"
"\t I2C0 monitors I2C data transfered on I2C bus. \n\r"
"\t UART0 (115200bps, 8 data bit, no parity, 1 stop bit, no flow control) \n\r"
"\t is used to display captured data. \n\r"
"********************************************************************************\n\r";
uint8_t buffer[BUFFER_SIZE];
BOOL_8 done=FALSE;
uint32_t count=0;

/************************** PRIVATE FUNCTIONS *************************/
void I2C0_IRQHandler(void);
void print_menu(void);
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief 		Main I2C0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void I2C0_IRQHandler(void)
{
	done = I2C_MonitorHandler(LPC_I2C0,buffer,count);
	if(done)
	{
		I2C_MonitorModeConfig(I2CDEV,(uint32_t)I2C_MONITOR_CFG_MATCHALL, DISABLE);
		I2C_MonitorModeCmd(I2CDEV, DISABLE);
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
	_DBG(menu1);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	uint8_t idx,i;
	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	//print menu screen
	print_menu();

	/* I2C block ------------------------------------------------------------------- */

	/*
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);//SDA0
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);//SCL0

	// Initialize I2C peripheral
	I2C_Init(I2CDEV, 100000);

	/* Configure interrupt for I2C in NVIC of ARM core */
    /* Disable I2C0 interrupt */
    NVIC_DisableIRQ(I2C0_IRQn);
    /* preemption = 1, sub-priority = 0 */
    NVIC_SetPriority(I2C0_IRQn, ((0x01<<3)|0x01));
    //enable I2C interrupt
    I2C_IntCmd(LPC_I2C0, ENABLE);

	/* Enable I2C operation */
	I2C_Cmd(I2CDEV, ENABLE);

	while(1)
	{
		idx=0;count=0;
		while(idx<2)
		{
			if(idx==0)
			{
				_DBG_("\n\rEnter monitor buffer size: ");
			}
			idx++;
			switch(_DG)
			{
				case '0': count=(count<<4)|0x00;break;
				case '1': count=(count<<4)|0x01;break;
				case '2': count=(count<<4)|0x02;break;
				case '3': count=(count<<4)|0x03;break;
				case '4': count=(count<<4)|0x04;break;
				case '5': count=(count<<4)|0x05;break;
				case '6': count=(count<<4)|0x06;break;
				case '7': count=(count<<4)|0x07;break;
				case '8': count=(count<<4)|0x08;break;
				case '9': count=(count<<4)|0x09;break;
				case 'a': count=(count<<4)|0x0A;break;
				case 'A': count=(count<<4)|0x0A;break;
				case 'b': count=(count<<4)|0x0B;break;
				case 'B': count=(count<<4)|0x0B;break;
				case 'c': count=(count<<4)|0x0C;break;
				case 'C': count=(count<<4)|0x0C;break;
				case 'd': count=(count<<4)|0x0D;break;
				case 'D': count=(count<<4)|0x0D;break;
				case 'e': count=(count<<4)|0x0E;break;
				case 'E': count=(count<<4)|0x0E;break;
				case 'f': count=(count<<4)|0x0F;break;
				case 'F': count=(count<<4)|0x0F;break;
				default: idx=0;count=0;break;
			}
			if(idx==2)
			{
				if(count>BUFFER_SIZE)
				{
					_DBG_("invalid! The size is bigger than ");_DBH(BUFFER_SIZE);
					idx=0;count=0;
				}
				else
					_DBH(count);
			}
		}
		//Configure I2C in monitor mode
		I2C_MonitorModeConfig(I2CDEV,(uint32_t)I2C_MONITOR_CFG_MATCHALL, ENABLE);
		I2C_MonitorModeCmd(I2CDEV, ENABLE);

		_DBG_("\n\rStart monitoring I2C bus...");

		while(done==FALSE); done=FALSE;
		_DBG_("done!");
		for(i=0;i<count;i++)
		{
			if((i%16)==0) _DBG_("");
			_DBH(buffer[i]);_DBC(0x20);
			buffer[i]=0;
		}

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
