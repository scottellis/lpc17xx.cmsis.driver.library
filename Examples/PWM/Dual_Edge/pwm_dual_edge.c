/**********************************************************************
* $Id$		pwm_dual_edge.c 				2010-05-21
*//**
* @file		pwm_dual_edge.c
* @brief	This program illustrates the PWM signal on 3 Channels in
* 			both edge mode and single mode
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
#include "lpc17xx_pwm.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup PWM_Dual_Edge	Dual_Edge
 * @ingroup PWM_Examples
 * @{
 */

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main PWM program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	uint8_t temp;
	PWM_TIMERCFG_Type PWMCfgDat;
	PWM_MATCHCFG_Type PWMMatchCfgDat;
	PINSEL_CFG_Type PinCfg;

	/* PWM block section -------------------------------------------- */
	/* Initialize PWM peripheral, timer mode
	 * PWM prescale value = 1 (absolute value - tick value) */
	PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	PWMCfgDat.PrescaleValue = 1;
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &PWMCfgDat);

	/*
	 * Initialize PWM pin connect
	 *
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 0; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}


	/* Set match value for PWM match channel 0 = 100, update immediately */
	PWM_MatchUpdate(LPC_PWM1, 0, 100, PWM_MATCH_UPDATE_NOW);
	/* PWM Timer/Counter will be reset when channel 0 matching
	 * no interrupt when match
	 * no stop when match */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);

	/* Configure each PWM channel: --------------------------------------------- */
	/* - Channel 2: Double Edge
	 * - Channel 4: Double Edge
	 * - Channel 5: Single Edge
	 * The Match register values are as follows:
	 * - MR0 = 100 (PWM rate)
	 * - MR1 = 41, MR2 = 78 (PWM2 output)
	 * - MR3 = 53, MR4 = 27 (PWM4 output)
	 * - MR5 = 65 (PWM5 output)
	 * PWM Duty on each PWM channel:
	 * - Channel 2: Set by match 1, Reset by match 2.
	 * - Channel 4: Set by match 3, Reset by match 4.
	 * - Channel 5: Set by match 0, Reset by match 5.
	 */

	/* Edge setting ------------------------------------ */
	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_DUAL_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 4, PWM_CHANNEL_DUAL_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 5, PWM_CHANNEL_SINGLE_EDGE);

	/* Match value setting ------------------------------------ */
	PWM_MatchUpdate(LPC_PWM1, 1, 41, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 2, 78, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 3, 53, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 4, 27, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 5, 65, PWM_MATCH_UPDATE_NOW);


	/* Match option setting ------------------------------------ */
	for (temp = 1; temp < 6; temp++)
	{
		/* Configure match option */
		PWMMatchCfgDat.IntOnMatch = DISABLE;
		PWMMatchCfgDat.MatchChannel = temp;
		PWMMatchCfgDat.ResetOnMatch = DISABLE;
		PWMMatchCfgDat.StopOnMatch = DISABLE;
		PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
	}

	/* Enable PWM Channel Output ------------------------------------ */
	/* Channel 2 */
	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE);
	/* Channel 4 */
	PWM_ChannelCmd(LPC_PWM1, 4, ENABLE);
	/* Channel 5 */
	PWM_ChannelCmd(LPC_PWM1, 5, ENABLE);

	/* Reset and Start counter */
	PWM_ResetCounter(LPC_PWM1);
	PWM_CounterCmd(LPC_PWM1, ENABLE);

	/* Start PWM now */
	PWM_Cmd(LPC_PWM1, ENABLE);

    /* Loop forever */
    while(1);
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
