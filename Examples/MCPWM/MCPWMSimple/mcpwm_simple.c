/**********************************************************************
* $Id$		mcpwm_simple.c 				2010-05-21
*//**
* @file		mcpwm_simple.c
* @brief	This example used to test MCPWM driver
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
#include "lpc17xx_mcpwm.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_clkpwr.h"
#include "debug_frmwrk.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup MCPWM_MCPWMSimple	MCPWMSimple
 * @ingroup MCPWM_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS **********************/
/** MCPWM in 3-phase DC motor mode test */
#define DC_MODE_TEST			1
/** MCPWM in 3-phase AC motor mode test */
#define AC_MODE_TEST			0
/** MCPWM tested with Capture function */
#define CAPTURE_MODE_TEST 		0


/************************** PRIVATE VARIABLES *************************/
#if CAPTURE_MODE_TEST
/** Capture configuration data */
MCPWM_CAPTURE_CFG_Type captureCfg;
/** capture flag */
__IO FlagStatus CapFlag;
/** Latest capture value */
__IO uint32_t CapVal;
#endif
/************************** PRIVATE FUNCTIONS *************************/
void MCPWM_IRQHandler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		MCPWM interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void MCPWM_IRQHandler(void)
{
#if CAPTURE_MODE_TEST
	// Check whether if capture event interrupt is set
	if (MCPWM_GetIntStatus(LPC_MCPWM, MCPWM_INTFLAG_CAP0)) {
		if (CapFlag == RESET) {
			// Store capture value
			CapVal = MCPWM_GetCapture(LPC_MCPWM, 0);
			// toggle capture flag
			CapFlag = SET;
			// Disable interrupt for capture event
			MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, DISABLE);
		}
		// Clear pending interrupt
		MCPWM_IntClear(LPC_MCPWM, MCPWM_INTFLAG_CAP0);
	}
#endif
}


/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main MCPWM program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	// MCPWM Channel configuration data
	MCPWM_CHANNEL_CFG_Type channelsetup[3];
	uint32_t i;
	PINSEL_CFG_Type PinCfg;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	_DBG_("Hello MCPWM ...");

	/* Pin configuration for MCPWM function:
	 * Assign: 	- P1.19 as MCOA0 - Motor Control Channel 0 Output A
	 * 			- P1.22 as MCOB0 - Motor Control Channel 0 Output B
	 * 			- P1.25 as MCOA1 - Motor Control Channel 1 Output A
	 * 			- P1.26 as MCOB1 - Motor Control Channel 1 Output B
	 * 			- P1.28 as MCOA2 - Motor Control Channel 2 Output A
	 * 			- P1.29 as MCOB2 - Motor Control Channel 2 Output B
	 * 			- P1.20 as MCI0	 - Motor Control Feed Back Channel 0
	 * Warning: According to Errata.lpc1768-18.March.2010: Input pin (MIC0-2)
	 * on the Motor Control PWM peripheral are not functional
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 19;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 22;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 25;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 29;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 20;
	PINSEL_ConfigPin(&PinCfg);

	/* Disable interrupt for MCPWM  */
	NVIC_DisableIRQ(MCPWM_IRQn);
	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(MCPWM_IRQn, ((0x01<<3)|0x01));

	/* Init MCPWM peripheral */
	MCPWM_Init(LPC_MCPWM);

	channelsetup[0].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[0].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[0].channelDeadtimeEnable = DISABLE;
	channelsetup[0].channelDeadtimeValue = 0;
	channelsetup[0].channelUpdateEnable = ENABLE;
	channelsetup[0].channelTimercounterValue = 0;
	channelsetup[0].channelPeriodValue = 300;
	channelsetup[0].channelPulsewidthValue = 0;

	channelsetup[1].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[1].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[1].channelDeadtimeEnable = DISABLE;
	channelsetup[1].channelDeadtimeValue = 0;
	channelsetup[1].channelUpdateEnable = ENABLE;
	channelsetup[1].channelTimercounterValue = 0;
	channelsetup[1].channelPeriodValue = 300;
	channelsetup[1].channelPulsewidthValue = 100;

	channelsetup[2].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[2].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[2].channelDeadtimeEnable = DISABLE;
	channelsetup[2].channelDeadtimeValue = 0;
	channelsetup[2].channelUpdateEnable = ENABLE;
	channelsetup[2].channelTimercounterValue = 0;
	channelsetup[2].channelPeriodValue = 300;
	channelsetup[2].channelPulsewidthValue = 200;

	MCPWM_ConfigChannel(LPC_MCPWM, 0, &channelsetup[0]);
	MCPWM_ConfigChannel(LPC_MCPWM, 1, &channelsetup[1]);
	MCPWM_ConfigChannel(LPC_MCPWM, 2, &channelsetup[2]);

#if DC_MODE_TEST
	/*
	 * - DC mode enabled.
	 * - Invert Output enabled
	 * - A0 and A1 output pin is internally routed to A0 signal
	 */
	MCPWM_DCMode(LPC_MCPWM, ENABLE, ENABLE, (MCPWM_PATENT_A0|MCPWM_PATENT_A1));
#endif

#if AC_MODE_TEST
	/*
	 * - AC mode is enabled.
	 */
	MCPWM_ACMode(LPC_MCPWM, ENABLE);
#endif

#if CAPTURE_MODE_TEST
	/*
	 * Capture mode in this case is used to detect the falling edge on MCO0B output pin.
	 * The MCFB0 input pin therefore must be connected to MCO0B. (P1.20 - P1.22)
	 * - Capture Channel 0.
	 * - Capture falling edge on MCFB0 input pin.
	 * - Interrupt enabled on capture event.
	 */
	captureCfg.captureChannel = 0;
	captureCfg.captureFalling = ENABLE;
	captureCfg.captureRising = DISABLE;
	captureCfg.hnfEnable = DISABLE;
	captureCfg.timerReset = DISABLE;
	MCPWM_ConfigCapture(LPC_MCPWM, 0, &captureCfg);

	// Reset flag for the first time
	CapFlag = RESET;

	// Enable interrupt for capture event on MCI0 (MCFB0)
	MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, ENABLE);

	/* Enable interrupt for MCPWM  */
	NVIC_EnableIRQ(MCPWM_IRQn);
#endif

	MCPWM_Start(LPC_MCPWM, ENABLE, ENABLE, ENABLE);

	// Main loop
	while (1) {
//		Timer_Wait(LPC_TIM0, 1000);
		//delay
		for(i=0;i<100000;i++);

		channelsetup[0].channelPulsewidthValue = (channelsetup[0].channelPulsewidthValue >= 300) ?
												0 : channelsetup[0].channelPulsewidthValue + 20;
		channelsetup[1].channelPulsewidthValue = (channelsetup[1].channelPulsewidthValue >= 300) ?
												0 : channelsetup[1].channelPulsewidthValue + 20;
		channelsetup[2].channelPulsewidthValue = (channelsetup[2].channelPulsewidthValue >= 300) ?
												0 : channelsetup[2].channelPulsewidthValue + 20;
		_DBG_("Update!");
		MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[0]);
		MCPWM_WriteToShadow(LPC_MCPWM, 1, &channelsetup[1]);
		MCPWM_WriteToShadow(LPC_MCPWM, 2, &channelsetup[2]);
#if CAPTURE_MODE_TEST
		// Check capture flag is set or not
		if (CapFlag) {
			// Print out the value
			_DBG("Capture Value: ");
			_DBD32(CapVal); _DBG_("");

			// Setup a new capture event
			MCPWM_ConfigCapture(LPC_MCPWM, 0, &captureCfg);

			// Re-Enable interrupt for capture event on MCI0 (MCFB0)
			MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, ENABLE);

			// Reset flag
			CapFlag = RESET;
		}
#endif
	}
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
