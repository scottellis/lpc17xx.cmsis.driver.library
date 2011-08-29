#include <stdio.h>
#include <string.h>

#include "debug_frmwrk.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip-conf.h"
#include "uipopt.h"
#include "uip_arp.h"
#include "uip.h"
#include "emac.h"
#include "lpc_types.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#define LED_PIN 	(1<<6)
#define LED2_MASK	((1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6))
#define LED1_MASK	((1<<28) | (1<<29) | (1<<31))


/* For debugging... */
#include <stdio.h>
#define DB	_DBG((uint8_t *)_db)
char _db[64];

void LED_Init (void)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t temp;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 2; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 29;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 31;
	PINSEL_ConfigPin(&PinCfg);

	// Set direction to output
	GPIO_SetDir(2, LED2_MASK, 1);
	GPIO_SetDir(1, LED1_MASK, 1);

	/* Turn off all LEDs */
	GPIO_ClearValue(2, LED2_MASK);
	GPIO_ClearValue(1, LED1_MASK);
}

/*************************************************************************
 * Function Name: uip_log
 * Parameters: none
 *
 * Return: none
 *
 * Description: Events logging
 *
 *************************************************************************/
void uip_log (char *m)
{
	_DBG("uIP log message: ");
	_DBG(m);
	_DBG_("");
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
int c_entry(void)
{
	UNS_32 i, delay;
	uip_ipaddr_t ipaddr;
	struct timer periodic_timer, arp_timer;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	_DBG_("Hello NXP Semiconductors");
	_DBG_("uIP porting on LPC17xx");


	// Initialize LED for system tick timer
	LED_Init();

	_DBG_("Init Clock");
	// Sys timer init 1/100 sec tick
	clock_init();

	timer_set(&periodic_timer, CLOCK_SECOND / 2); /*0.5s */
	timer_set(&arp_timer, CLOCK_SECOND * 10);	/*10s */

	_DBG_("Init EMAC");
	// Initialize the ethernet device driver
	while(!tapdev_init()){
		// Delay for a while then continue initializing EMAC module
		_DBG_("Error during initializing EMAC, restart after a while");
		for (delay = 0x100000; delay; delay--);
	}


#if 1

	_DBG_("Init uIP");
	// Initialize the uIP TCP/IP stack.
	uip_init();

	// init MAC address
	uip_ethaddr.addr[0] = EMAC_ADDR0;
	uip_ethaddr.addr[1] = EMAC_ADDR1;
	uip_ethaddr.addr[2] = EMAC_ADDR2;
	uip_ethaddr.addr[3] = EMAC_ADDR3;
	uip_ethaddr.addr[4] = EMAC_ADDR4;
	uip_ethaddr.addr[5] = EMAC_ADDR5;
	uip_setethaddr(uip_ethaddr);


	uip_ipaddr(ipaddr, 192,168,0,100);
	sprintf(_db, "Set own IP address: %d.%d.%d.%d \n\r", \
			((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
			((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
	DB;
	uip_sethostaddr(ipaddr);

	uip_ipaddr(ipaddr, 192,168,0,1);
	sprintf(_db, "Set Router IP address: %d.%d.%d.%d \n\r", \
			((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
			((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
	DB;
	uip_setdraddr(ipaddr);

	uip_ipaddr(ipaddr, 255,255,255,0);
	sprintf(_db, "Set Subnet mask: %d.%d.%d.%d \n\r", \
			((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], \
			((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3]);
	DB;
	uip_setnetmask(ipaddr);

	// Initialize the HTTP server ----------------------------
	_DBG_("Init HTTP");
	httpd_init();
	_DBG_("Init complete!");

  while(1)
  {
    uip_len = tapdev_read(uip_buf);
    if(uip_len > 0)
    {
      if(BUF->type == htons(UIP_ETHTYPE_IP))
      {
	      uip_arp_ipin();
	      uip_input();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */

	      if(uip_len > 0)
        {
	        uip_arp_out();
	        tapdev_send(uip_buf,uip_len);
	      }
      }
      else if(BUF->type == htons(UIP_ETHTYPE_ARP))
      {
        uip_arp_arpin();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */
	      if(uip_len > 0)
        {
	        tapdev_send(uip_buf,uip_len);
	      }
      }
    }
    else if(timer_expired(&periodic_timer))
    {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++)
      {
      	uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0)
        {
          uip_arp_out();
          tapdev_send(uip_buf,uip_len);
        }
      }
#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
          uip_arp_out();
          tapdev_send();
        }
      }
#endif /* UIP_UDP */
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer))
      {
        timer_reset(&arp_timer);
        uip_arp_timer();
      }
    }
  }
#endif

  while (1);
}

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
