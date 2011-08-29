/**********************************************************************
* $Id$		pwm_single_edge.c 				2010-05-21
*//**
* @file		pwm_single_edge.c
* @brief	This program illustrates the PWM signal on 6 Channels
*           in single edge mode
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
/** @defgroup PWM_Single_Edge	Single_Edge
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
	uint8_t temp, temp2;
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
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 0; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}


	/* Set match value for PWM match channel 0 = 256, update immediately */
	PWM_MatchUpdate(LPC_PWM1, 0, 256, PWM_MATCH_UPDATE_NOW);
	/* PWM Timer/Counter will be reset when channel 0 matching
	 * no interrupt when match
	 * no stop when match */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);

	/* Configure each PWM channel: --------------------------------------------- */
	/* - Single edge
	 * - PWM Duty on each PWM channel determined by
	 * the match on channel 0 to the match of that match channel.
	 * Example: PWM Duty on PWM channel 1 determined by
	 * the match on channel 0 to the match of match channel 1.
	 */

	/* Configure PWM channel edge option
	 * Note: PWM Channel 1 is in single mode as default state and
	 * can not be changed to double edge mode */
	for (temp = 2; temp < 7; temp++)
	{
		PWM_ChannelConfig(LPC_PWM1, temp, PWM_CHANNEL_SINGLE_EDGE);
	}


	/* Configure match value for each match channel */
	temp2 = 10;
	for (temp = 1; temp < 7; temp++)
	{
		/* Set up match value */
		PWM_MatchUpdate(LPC_PWM1, temp, temp2, PWM_MATCH_UPDATE_NOW);
		/* Configure match option */
		PWMMatchCfgDat.IntOnMatch = DISABLE;
		PWMMatchCfgDat.MatchChannel = temp;
		PWMMatchCfgDat.ResetOnMatch = DISABLE;
		PWMMatchCfgDat.StopOnMatch = DISABLE;
		PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
		/* Enable PWM Channel Output */
		PWM_ChannelCmd(LPC_PWM1, temp, ENABLE);
		/* Increase match value by 10 */
		temp2 += 10;
	}

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
