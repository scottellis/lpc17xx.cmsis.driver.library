/**********************************************************************
* $Id$		privilege.c				2010-07-18
*//**
* @file		privilege.c
* @brief	This example used to test privilege feature of Cortex-M3
* 			processor
* @version	1.0
* @date		18. July. 2010
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
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup Cortex_M3_Privilege_mode	Privilege_mode
 * @ingroup Cortex_M3_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS*************************/
/* Uncomment to chose current using eval board */
//#define MCB_LPC_1768
#define IAR_LPC_1768

/* CONTROL register controls the privileged level for software execution
 * when the processor is in Thread mode
 */
#define	THREAD_PRIVILEGED			0
#define THREAD_UNPRIVILEGED			1
#define THREAD_PRIVILEDGED_MASK		0xFFFFFFFE

/* Used LED */
#ifdef MCB_LPC_1768
#define PRIVILEGE_LED				28 //LED P1.28
#define PRIVILEGE_LEDPORT			1
#define UNPRIVILEGE_LED				29 //LED P1.29
#define UNPRIVILEGE_LEDPORT			1
#elif defined(IAR_LPC_1768)
#define PRIVILEGE_LED				25 //LED1: P1.25
#define PRIVILEGE_LEDPORT			1
#define UNPRIVILEGE_LED				4	//LED2: P0.4
#define	UNPRIVILEGE_LEDPORT			0
#endif

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"Privileged demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"This example used to test Privileged feature of Cortex-M3 processor\n\r"
	"********************************************************************************\n\r";
#ifdef MCB_LPC_1768
uint8_t menu2[]=
	"LED1 (P1.28) will be turned on in privilege mode \n\r"
	"LED2 (P1.29) will be turned on in un-privilege mode\n\r";
#elif defined (IAR_LPC_1768)
uint8_t menu2[]=
	"LED1 (P1.25) will be turned on in privilege mode \n\r"
	"LED2 (P0.4) will blink in un-privilege mode\n\r";
#endif

/************************** SYSTEM CALL EXCEPTION *********************/
#if defined ( __CC_ARM   )
__ASM void __SVC(void)
{
  SVC 0x01
  BX R14
}
#elif defined ( __ICCARM__ )
static __INLINE  void __SVC()                     { __ASM ("svc 0x01");}
#elif defined   (  __GNUC__  )
static __INLINE void __SVC()                      { __ASM volatile ("svc 0x01");}
#endif
/************************** PRIVATE FUNCTIONS *************************/
/*********************************************************************//**
  * @brief  SVC Hander
  * @param  None
  * @retval None
 **********************************************************************/
void SVC_Handler(void)
{
  /* Change Thread mode to privileged */
  __set_CONTROL(2);
}

/************************** PRIVATE FUNCTIONS *************************/
void LED_Init(void);
void Turn_on_LED(uint8_t Led_num);
void Turn_off_LED(uint8_t Led_num);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Initialize LED
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void LED_Init(void)
{
	//setting two LEDs as output and turn off all
	GPIO_SetDir(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED),1);
	GPIO_SetDir(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED),1);
#ifdef MCB_LPC_1768
	GPIO_ClearValue(1,(1<<PRIVILEGE_LED)|(1<<UNPRIVILEGE_LED));
	GPIO_ClearValue(2,(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6));
#elif defined(IAR_LPC_1768)
	GPIO_SetValue(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED));
	GPIO_SetValue(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED));
#endif
}

/*********************************************************************//**
 * @brief		Turn on LED
 * @param[in]	Led_num		LED number, should be:
 * 				- PRIVILEGE_LED
 * 				- UNPRIVILEGE_LED
 * @return 		None
 **********************************************************************/
void Turn_on_LED(uint8_t Led_num)
{
#ifdef MCB_LPC_1768
	if(Led_num == PRIVILEGE_LED){
		GPIO_SetValue(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED));}
	else{
		GPIO_SetValue(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED));
	}
#elif defined(IAR_LPC_1768)
	if(Led_num == PRIVILEGE_LED){
		GPIO_ClearValue(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED));}
	else{
		GPIO_ClearValue(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED));
	}
#endif
}

/*********************************************************************//**
 * @brief		Turn off LED
 * @param[in]	Led_num		LED number, should be:
 * 				- PRIVILEGE_LED
 * 				- UNPRIVILEGE_LED
 * @return 		None
 **********************************************************************/
void Turn_off_LED(uint8_t Led_num)
{
#ifdef MCB_LPC_1768
	if(Led_num == PRIVILEGE_LED){
		GPIO_ClearValue(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED));}
	else{
		GPIO_ClearValue(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED));
	}
#elif defined(IAR_LPC_1768)
	if(Led_num == PRIVILEGE_LED){
		GPIO_SetValue(PRIVILEGE_LEDPORT,(1<<PRIVILEGE_LED));}
	else{
		GPIO_SetValue(UNPRIVILEGE_LEDPORT,(1<<UNPRIVILEGE_LED));
	}
#endif
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: main function
 * @param[in]	none
 * @return 		int
 **********************************************************************/
int c_entry(void)			/* Main Program */
{
	uint32_t priviledge_status;
	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();
	LED_Init();
	//print menu
	_DBG(menu);
	_DBG_(menu2);

	/* Thread mode is privilege out of reset */
	/* First checking Thread mode is privilege or un-privilege */
	priviledge_status = __get_CONTROL();
	if((priviledge_status & (~THREAD_PRIVILEDGED_MASK))==THREAD_PRIVILEGED)
	{
		_DBG_("Thread mode is privileged!");
		Turn_on_LED(PRIVILEGE_LED);
	}
	else
	{
		_DBG_("Theard mode is unprivileged! It's not compliant with Cortex-M3 Technical");
		while(1); //Error loop
	}
	/* Wait user press '1' character to change to un-privilege mode */
	_DBG_("Press '1' to change to unprivilege mode ...\n\r");
	while(_DG !='1');

	/* Changing to un-privilege mode */
	__set_CONTROL((priviledge_status & THREAD_PRIVILEDGED_MASK)|THREAD_UNPRIVILEGED);
	_DBG_("Changed to unprivilege mode!");

	/* Check */
	priviledge_status = __get_CONTROL();
	if((priviledge_status & ~THREAD_PRIVILEDGED_MASK)==THREAD_UNPRIVILEGED)
	{
		_DBG_("Check: Thread mode change to unprivilege successful!");
		Turn_off_LED(PRIVILEGE_LED);
		Turn_on_LED(UNPRIVILEGE_LED);
	}
	else
	{
		_DBG_("Check: Thread mode is still privileged! ERROR...");
		while(1); //Error loop
	}
	/* Wait user press '1' character to change to un-privilege mode */
	_DBG_("Press '2' to change to privilege mode by calling system call exception...\n\r");
	while(_DG !='2');
	/* Call system call exception to re-change Thread mode into privilege */
	__SVC();
	_DBG_("Called system call exception!");

	/* Check */
	priviledge_status = __get_CONTROL();
	if((priviledge_status & ~THREAD_PRIVILEDGED_MASK)==THREAD_PRIVILEGED)
	{
		_DBG_("Check: Thread mode change to privilege successful!");
		Turn_off_LED(UNPRIVILEGE_LED);
		Turn_on_LED(PRIVILEGE_LED);
	}
	else
	{
		_DBG_("Check: Theard mode is still unprivileged! ERROR...");
		while(1); //Error loop
	}
	_DBG_("Demo terminate!");
	while (1);
	return 0;
}

/* With ARM and GHS toolsets, the entry point is main() - this will
 allow the linker to generate wrapper code to setup stacks, allocate
 heap area, and initialize and copy code and data segments. For GNU
 toolsets, the entry point is through __start() in the crt0_gnu.asm
 file, and that startup code will setup stacks and data */
int main(void) {
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
void check_failed(uint8_t *file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
		;
}
#endif

/*
 * @}
 */
