/**********************************************************************
* $Id$		qei_test.c						2010-05-21
*//**
* @file		qei_test.c
* @brief	This example used to test QEI driver in Quadrature mode with
* 			velocity calculation (RPM)
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
#include "lpc17xx_qei.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "debug_frmwrk.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup QEI_Velo	QEI_Velo
 * @ingroup QEI_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
/** In case of using QEI virtual signal, this macro below must be set to 1 */
#define VIRTUAL_QEI_SIGNAL	1

/** Signal Mode setting:
 * - When = 0, PhA and PhB function as quadrature encoder inputs.
 * - When = 1, PhA functions as the direction signal and PhB functions
 * as the clock signal
 */
#define SIGNAL_MODE 		0

/** Capture Mode setting:
 * - When = 0, only PhA edges are counted (2X).
 * - When = 1, BOTH PhA and PhB edges are counted (4X), increasing
 * resolution but decreasing range
 */
#define CAP_MODE 			1

/** Velocity capture period definition (in microsecond) */
#define CAP_PERIOD			250000UL

/** Delay time to Read Velocity Accumulator and display (in microsecond)*/
#define DISP_TIME			3000000UL
/** Max velocity capture times calculated */
#define MAX_CAP_TIMES		(DISP_TIME/CAP_PERIOD)

#define ENC_RES	 			2048UL	/**< Encoder resolution (PPR) */


/* In case of using Virtual QEI signal, these following macros must be defined */
#ifdef VIRTUAL_QEI_SIGNAL
/* Max velocity */
#define MAX_VEL  			600UL 	/**< Max velocity (RPM) */
#if CAP_MODE
#define COUNT_MODE			4
#else
#define COUNT_MODE			2
#endif
#endif

/* Pin on Port 0 assigned to Phase A */
#define PHASE_A_PIN			(1<<19)
/* Pin on Port 0 assigned to Phase B */
#define PHASE_B_PIN			(1<<21)

/************************** PRIVATE VARIABLES *************************/
#ifdef VIRTUAL_QEI_SIGNAL
/** Phase Counter:
 * - 0: Phase A = 1, Phase B = 0
 * - 1: Phase A = 1, Phase B = 1
 * - 2: Phase A = 0, Phase B = 1
 * - 3: Phase A = 0, Phase B = 0
 */
__IO uint8_t PhaseCnt;
#endif /* VIRTUAL_QEI_SIGNAL */

/** Velocity Accumulator */
__IO uint64_t VeloAcc;
/** Times of Velocity capture */
__IO uint32_t VeloCapCnt;
/** Flag indicates Times of Velocity capture is enough to read out */
__IO FlagStatus VeloAccFlag;


/* Pin Configuration selection must be defined in structure following:
 * - Port Number,
 * - Pin Number,
 * - Function Number,
 * - Pin Mode,
 * - Open Drain
 */

/** QEI Phase-A Pin */
const PINSEL_CFG_Type qei_phaA_pin[1] = {{1, 20, 1, 0, 0}};
/** QEI Phase-B Pin */
const PINSEL_CFG_Type qei_phaB_pin[1] = {{1, 23, 1, 0, 0}};
/** QEI Index Pin */
const PINSEL_CFG_Type qei_idx_pin[1] = {{1, 24, 1, 0, 0}};


/************************** PRIVATE FUNCTIONS *************************/
#ifdef VIRTUAL_QEI_SIGNAL
void VirtualQEISignal_Init(void);
void TIMER0_IRQHandler(void);
#endif
void QEI_IRQHandler(void);


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
#ifdef VIRTUAL_QEI_SIGNAL
/*********************************************************************//**
 * @brief		Timer 0 interrupt handler. This sub-routine will set/clear
 * 				two Phase A-B output pin to their phase state
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void TIMER0_IRQHandler(void)
{
	if (TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)) {

		// Set/Clear phase A/B pin corresponding to their state
		switch (PhaseCnt) {
		case 0:
			GPIO_SetValue(0,PHASE_A_PIN);
			GPIO_ClearValue(0,PHASE_B_PIN);
			break;
		case 1:
			GPIO_SetValue(0, PHASE_A_PIN | PHASE_B_PIN);
			break;
		case 2:
			GPIO_SetValue(0, PHASE_B_PIN);
			GPIO_ClearValue(0, PHASE_A_PIN);
			break;
		case 3:
			GPIO_ClearValue(0, PHASE_A_PIN | PHASE_B_PIN);
			break;

		default:
			break;
		}

		// update PhaseCnt
		PhaseCnt = (PhaseCnt + 1) & 0x03;

		// Clear Timer 0 match interrupt pending
		TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
	}
}
#endif
/*********************************************************************//**
 * @brief		QEI interrupt handler. This sub-routine will update current
 * 				value of captured velocity in to velocity accumulate.
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void QEI_IRQHandler(void)
{
	// Check whether if velocity timer overflow
	if (QEI_GetIntStatus(LPC_QEI, QEI_INTFLAG_TIM_Int) == SET) {
		if (VeloAccFlag == RESET) {

			// Get current velocity captured and update to accumulate
			VeloAcc += QEI_GetVelocityCap(LPC_QEI);

			// Update Velocity capture times
			VeloAccFlag = ((VeloCapCnt++) >= MAX_CAP_TIMES) ? SET : RESET;
		}
		// Reset Interrupt flag pending
		QEI_IntClear(LPC_QEI, QEI_INTFLAG_TIM_Int);
	}

	// Check whether if direction change occurred
	if (QEI_GetIntStatus(LPC_QEI, QEI_INTFLAG_DIR_Int) == SET) {
		// Print direction status
		_DBG("Direction has changed: ");
		_DBG_((QEI_GetStatus(LPC_QEI, QEI_STATUS_DIR) == SET) ? "1" : "0");
		// Reset Interrupt flag pending
		QEI_IntClear(LPC_QEI, QEI_INTFLAG_DIR_Int);
	}
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
#ifdef VIRTUAL_QEI_SIGNAL
/*********************************************************************//**
 * @brief		Initializes signal supplying for QEI peripheral by using timer
 * 			match interrupt output, that will generate two virtual signal on
 * 			Phase-A and Phase-B. These two clock are 90 degrees out of phase.
 * 			In this case, a 'virtual encoder' that has these following parameter:
 * 			- Encoder type			: Quadrature encoder
 * 			- Max velocity			: MAX_VEL (Round Per Minute)
 * 			- Encoder Resolution	: ENC_RES (Pulse Per Round)
 * 			The calculated frequency is: Freq = (MAX_VEL * ENC_RES * COUNT_MODE) / 60 (Hz)
 * 			The timer therefore should be set to tick every cycle T = 1/Freq (second)
 * Figure:
 *           |-----|     |-----|
 * Phase A --|     |-----|     |-----
 *              |-----|     |-----|
 * Phase B -----|     |-----|     |--
 *
 *           |--|--|--|--|--|--|--|--
 *            T  T  T  T  T  T  T
 *
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void VirtualQEISignal_Init(void)
{
	uint32_t pclk;
	TIM_TIMERCFG_Type TimerConfig;
	TIM_MATCHCFG_Type TimerMatchConfig;

	_DBG_("Initializing Virtual QEI signal...");

	// Initialize timer 0, Prescale value in tick value option with tick value = 1
	TimerConfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TimerConfig.PrescaleValue	= 1;
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TimerConfig);

	// Get actual peripheral clock of timer 0
	pclk = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0);
	pclk = pclk / ((MAX_VEL * ENC_RES * COUNT_MODE) / 60 );
	// Set match for match channel 0
	TimerMatchConfig.MatchChannel = 0;
	TimerMatchConfig.MatchValue = pclk;
	TimerMatchConfig.IntOnMatch = ENABLE;
	TimerMatchConfig.ExtMatchOutputType = 3;
	TimerMatchConfig.ResetOnMatch = ENABLE;
	TimerMatchConfig.StopOnMatch = DISABLE;
	TIM_ConfigMatch(LPC_TIM0, &TimerMatchConfig);

	// Reconfigures GPIO for pin used as Phase A and Phase B output
	GPIO_SetDir(0, PHASE_A_PIN | PHASE_B_PIN, 1);
	// Set default State after initializing
	GPIO_ClearValue(0, PHASE_A_PIN | PHASE_B_PIN);
	// Reset Phase Counter
	PhaseCnt = 0;

	/* preemption = 1, sub-priority = 2 */
	NVIC_SetPriority(TIMER0_IRQn, ((0x02<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	// To start timer 0
	TIM_Cmd(LPC_TIM0,ENABLE);
}
#endif /* VIRTUAL_QEI_SIGNAL */

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main QEI program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	QEI_CFG_Type QEIConfig;
	QEI_RELOADCFG_Type ReloadConfig;
	uint32_t rpm, averageVelo;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	_DBG_("Hello QEI ...");
	_DBG("Speed will be sampled every each ");
	_DBD32(CAP_PERIOD);
	_DBG_(" us");
	_DBG_("This value will be accumulated to display as RPM after every each");
	_DBD32(DISP_TIME);
	_DBG_(" us");

	/* Initialize QEI configuration structure to default value */
#if CAP_MODE
	QEIConfig.CaptureMode = QEI_CAPMODE_4X;
#else
	QEIConfig.CaptureMode = QEI_CAPMODE_2X;
#endif
	QEIConfig.DirectionInvert = QEI_DIRINV_NONE;
	QEIConfig.InvertIndex = QEI_INVINX_NONE;
#if SIGNAL_MODE
	QEIConfig.SignalMode = QEI_SIGNALMODE_CLKDIR;
#else
	QEIConfig.SignalMode = QEI_SIGNALMODE_QUAD;
#endif

	/* Set QEI function pin
	 * P1.20: MCI0
	 * P1.23: MCI1
	 * P1.24: MCI2
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 20;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 23;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);

	/* Initialize QEI peripheral with given configuration structure */
	QEI_Init(LPC_QEI, &QEIConfig);

	// Set timer reload value for  QEI that used to set velocity capture period
	ReloadConfig.ReloadOption = QEI_TIMERRELOAD_USVAL;
	ReloadConfig.ReloadValue = CAP_PERIOD;
	QEI_SetTimerReload(LPC_QEI, &ReloadConfig);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(QEI_IRQn, ((0x01<<3)|0x01));
	/* Enable interrupt for QEI  */
	NVIC_EnableIRQ(QEI_IRQn);

	// Reset VeloAccFlag
	VeloAccFlag = RESET;
	// Reset value of Acc and Acc count to default
	VeloAcc = 0;
	VeloCapCnt = 0;

	// Enable interrupt for velocity Timer overflow for capture velocity into Acc */
	QEI_IntCmd(LPC_QEI, QEI_INTFLAG_TIM_Int, ENABLE);
	// Enable interrupt for direction change */
	QEI_IntCmd(LPC_QEI, QEI_INTFLAG_DIR_Int, ENABLE);

#ifdef VIRTUAL_QEI_SIGNAL
	// This used for generating virtual QEI signal
	VirtualQEISignal_Init();
#endif

	// Main loop
	while (1) {

		// Check VeloAccFlag continuously
		if (VeloAccFlag == SET) {
			// Get Acc
			averageVelo = (uint32_t)(VeloAcc / VeloCapCnt);
			rpm = QEI_CalculateRPM(LPC_QEI, averageVelo, ENC_RES);
			// Disp the result
			_DBG("Sampling Speed: ");
			_DBD32(rpm);
			_DBG_("RPM");
			// Reset VeloAccFlag
			VeloAccFlag = RESET;
			// Reset value of Acc and Acc count to default
			VeloAcc = 0;
			VeloCapCnt = 0;
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
