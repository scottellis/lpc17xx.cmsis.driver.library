/**********************************************************************
* $Id$		dac_speaker.c				2010-07-16
*//**
* @file		dac_speaker.c
* @brief	This example used to test DAC peripheral through speaker
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
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/************************** PUBLIC DEFINITIONS *************************/
/* Region size definitions */
#define MPU_REGION_SIZE_32B		0x04
#define MPU_REGION_SIZE_64B		0x05
#define MPU_REGION_SIZE_128B	0x06
#define MPU_REGION_SIZE_256B	0x07
#define MPU_REGION_SIZE_512B	0x08
#define MPU_REGION_SIZE_1KB		0x09
#define MPU_REGION_SIZE_2KB		0x0A
#define MPU_REGION_SIZE_4KB		0x0B
#define MPU_REGION_SIZE_8KB		0x0C
#define MPU_REGION_SIZE_16KB	0x0D
#define MPU_REGION_SIZE_32KB	0x0E
#define MPU_REGION_SIZE_64KB	0x0F
#define MPU_REGION_SIZE_128KB	0x10
#define MPU_REGION_SIZE_256KB	0x11
#define MPU_REGION_SIZE_512KB	0x12
#define MPU_REGION_SIZE_1MB		0x13
#define MPU_REGION_SIZE_2MB		0x14
#define MPU_REGION_SIZE_4MB		0x15
#define MPU_REGION_SIZE_8MB		0x16
#define MPU_REGION_SIZE_16MB	0x17
#define MPU_REGION_SIZE_32MB	0x18
#define MPU_REGION_SIZE_64MB	0x19
#define MPU_REGION_SIZE_128MB	0x1A
#define MPU_REGION_SIZE_256MB	0x1B
#define MPU_REGION_SIZE_512MB	0x1C
#define MPU_REGION_SIZE_1GB		0x1D
#define MPU_REGION_SIZE_2GB		0x1E
#define MPU_REGION_SIZE_4GB		0x1F

/* Access permission definitions */
#define MPU_NO_ACCESS							0x00
#define MPU_PRIVILEGED_ACESS_USER_NO_ACCESS		0x01
#define MPU_PRIVILEGED_RW_USER_READ_ONLY		0x02
#define MPU_FULL_ACCESS							0x03
#define MPU_UNPREDICTABLE						0x04
#define MPU_PRIVILEGED_READ_ONLY_USER_NO_ACCESS	0x05
#define MPU_READ_ONLY							0x06

/* RASR bit definitions */
#define MPU_RASR_REGION_SIZE(n)			((uint32_t)(n<<1))
#define MPU_RASR_ACCESS_PERMISSION(n)	((uint32_t)(n<<24))
#define MPU_REGION_ENABLE				((uint32_t)(1<<0))

/* Example group ----------------------------------------------------------- */
/** @defgroup Cortex_M3_MPU	MPU
 * @ingroup Cortex_M3_Examples
 * @{
 */
/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"MPU demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Set up 6 region memory and try to access memory that don't allow to invoke\n\r"
	"Memory Management Handler\n\r"
	"********************************************************************************\n\r";
Bool Led_State = FALSE;

/************************** PRIVATE FUNCTIONS *************************/
void MemManage_Handler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		Memory Management interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void MemManage_Handler(void)
{
	uint32_t i;
	//Blink LED P1.28
	if(Led_State == FALSE)
	{
		GPIO_SetValue(1,(1<<28));
		Led_State = TRUE;
		for(i = 0;i<0x70000;i++);//delay
	}
	else
	{
		GPIO_ClearValue(1,(1<<28));
		Led_State = FALSE;
		for(i = 0;i<0x70000;i++);//delay
	}
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main MPU program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	uint32_t test;
	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	_DBG(menu);

	//Turn off all LEDs
	GPIO_SetDir(1,(1<<28)|(1<<29)|(1<<31),1);
	GPIO_SetDir(2,(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6),1);


	/* - Region 0: 0x00000000 - 0x0007FFFF --- on-chip non-volatile memory
	 * 		+ Size: 512kB
	 * 		+ Acess permission: full access
	 */
	MPU->RNR = 0;//indicate MPU region 0
	MPU->RBAR = 0x00000000; // update the base address for the region 0
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)		//full access
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_512KB)	//512Kb size
				|MPU_REGION_ENABLE;								//region enable

	/* - Region 1: 0x10000000 - 0x10007FFF --- on-chip SRAM
	 * 		+ Size: 32kB
	 * 		+ Access permission: full access
	 */
	MPU->RNR = 1;
	MPU->RBAR = 0x10000000; // update the base address for the region 1
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_32KB)
				|MPU_REGION_ENABLE;

	 /* - Region 2:	0x40000000 - 0x400FFFFF --- APB peripheral
	 * 		+ Size: 1MB
	 * 		+ Access permission: full access
	 */
	MPU->RNR = 2;
	MPU->RBAR = 0x40000000; // update the base address for the region 2
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_1MB)
				|MPU_REGION_ENABLE;

	 /* - Region 3: 0x50000000 - 0x501FFFFF --- AHB peripheral
	 * 		+ Size: 2MB
	 * 		+ AP=b011: full access
	 */
	MPU->RNR = 3;
	MPU->RBAR = 0x50000000; // update the base address for the region 3
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_2MB)
				|MPU_REGION_ENABLE;

	 /* - Region 4: 0xE0000000 - 0xE00FFFFF --- System control
	 * 	 	+ Size: 1MB
	 * 		+ Access permission: full access
	 */
	MPU->RNR = 4;
	MPU->RBAR = 0xE0000000; // update the base address for the region 4
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_1MB)
				|MPU_REGION_ENABLE;

	 /* - Region 5:0x2007C000 - 0x2007FFFF --- on chip SRAM
	 * 	 	+ Size: 16kB
	 * 		+ Access permission: full access
	 */
	MPU->RNR = 5;
	MPU->RBAR = 0x2007C000; // update the base address for the region 5
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_16KB)
				|MPU_REGION_ENABLE;

	 /* - Region 6: 0x20080000 - 0x20083FFF --- on chip SRAM
	 * 		+ Size: 16kB
	 * 		+ Access permission: no access
	 */
	MPU->RNR = 6;
	MPU->RBAR = 0x20080000; // update the base address for the region 6
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_NO_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_16KB)
				|MPU_REGION_ENABLE;

	 /* - Region 5:0x2009C000 - 0x2009FFFF --- GPIO peripheral
	 * 	 	+ Size: 16kB
	 * 		+ Access permission: full access
	 */
	MPU->RNR = 7;
	MPU->RBAR = 0x2009C000; // update the base address for the region 6
	MPU->RASR = MPU_RASR_ACCESS_PERMISSION(MPU_FULL_ACCESS)
				|MPU_RASR_REGION_SIZE(MPU_REGION_SIZE_16KB)
				|MPU_REGION_ENABLE;


	_DBG_("Setup MPU: \n\r"
		  "This provide 8 regions: \n\r"
		  "Region 0 - Privileged code: 0x00000000 - 0x0007FFFF(512kB)\n\r"
		  "Region 1 - Privileged data: 0x10000000 - 0x10007FFF(32kB)\n\r"
		  "Region 2 - APB Peripheral:  0x40000000 - 0x400FFFFF(1MB)\n\r"
		  "Region 3 - AHB peripheral:  0x50000000 - 0x501FFFFF(2MB)\n\r"
		  "Region 4 - System control:  0xE0000000 - 0xE00FFFFF(1MB)\n\r"
		  "Region 5 - On-chip SRAM:    0x2007C000 - 0x2007FFFF(16kB)\n\r"
		  "Region 6 - Private SRAM:    0x20080000 - 0x20083FFF(16kB)\n\r"
		  "Region 7 - GPIO peripheral: 0x2009C000 - 0x2009FFFF(16Kb)\n\r"
		  "Region 6 can not access (just used for testing)");

	SCB->SHCSR |=(1<<16); //Enable Memory management fault
	MPU->CTRL =(1<<0); //Enable the MPU
	_DBG_("Enable MPU!");

	//try to access to this memory range
	_DBG_("Press '1' to try to read memory from region 1");
	while(_DG !='1');
	test = (*(unsigned int *)0x10000000);
	_DBG_("Read successful!!!");

	_DBG_("Press '2' to try to read memory from region 6\n\r"
		  "Read memory at this region is not allow, LED p1.28 will blink...");
	while(_DG !='2');
	test = (*(unsigned int *)0x20080004);
	test++;

	while(test);
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
