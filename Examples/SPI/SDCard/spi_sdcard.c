/**********************************************************************
* $Id$		spi_sdcard.c					2010-07-16
*//**
* @file		spi_sdcard.c
* @brief	This example describes how to use SPI to read SD card's CID register
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
#include "lpc17xx_spi.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup SPI_SDCard		SDCard
 * @ingroup SPI_Examples
 * @{
 */

/************************** PRIVATE DEFINTIONS *********************/
// PORT number that /CS pin assigned on
#define CS_PORT_NUM		0
// PIN number that  /CS pin assigned on
#define CS_PIN_NUM		16

#define SD_DETECT_PORTNUM	4
#define SD_DETECT_PINNUM	29
typedef enum _sd_connect_status
{
	SD_CONNECTED,
	SD_DISCONNECTED
}sd_connect_status;
typedef enum _sd_error
{
	SD_OK,
	SD_NG,
	SD_CMD_BAD_PARAMETER,
	SD_ERROR_TOKEN,
	SD_ERROR_TIMEOUT,
	SD_ERROR_BUS_NOT_IDLE,
	SD_ERROR_CMD0,
	SD_ERROR_CMD55,
	SD_ERROR_ACMD41,
	SD_ERROR_CMD59
}sd_error;

//SD command code
#define 	CMD0_GO_IDLE_STATE            0x00
#define		CMD1_SEND_OPCOND              0x01
#define 	CMD9_SEND_CSD                 0x09
#define 	CMD10_SEND_CID                0x0a
#define  	CMD12_STOP_TRANSMISSION       0x0b
#define 	CMD13_SEND_STATUS             0x0c
#define 	CMD16_SET_BLOCKLEN            0x10
#define 	CMD17_READ_SINGLE_BLOCK       0x11
#define 	CMD18_READ_MULTIPLE_BLOCK     0x12
#define 	CMD24_WRITE_BLOCK             0x18
#define 	CMD25_WRITE_MULTIPLE_BLOCK    0x19
#define 	CMD27_PROGRAM_CSD             0x1b
#define 	CMD28_SET_WRITE_PROT          0x1c
#define 	CMD29_CLR_WRITE_PROT          0x1d
#define 	CMD30_SEND_WRITE_PROT         0x1e
#define 	CMD32_ERASE_WR_BLK_START_ADDR 0x20
#define 	CMD33_ERASE_WR_BLK_END_ADDR   0x21
#define 	CMD38_ERASE                   0x26
#define 	CMD55_APP_CMD                 0x37
#define 	CMD56_GEN_CMD                 0x38
#define 	CMD58_READ_OCR                0x3a
#define 	CMD59_CRC_ON_OFF              0x3b
/* Application-specific commands (always prefixed with CMD55_APP_CMD) */
#define	 	ACMD13_SD_STATUS              0x0d
#define 	ACMD22_SEND_NUM_WR_BLOCKS     0x16
#define 	ACMD23_SET_WR_BLK_ERASE_COUNT 0x17
#define 	ACMD41_SEND_OP_COND           0x29
#define 	ACMD42_SET_CLR_CARD_DETECT    0x2a
#define 	ACMD51_SEND_SCR               0x33
/* R1 format responses (ORed together as a bit-field) */
#define 	R1_NOERROR   				  0x00
#define 	R1_IDLE      				  0x01
#define 	R1_ERASE     				  0x02
#define 	R1_ILLEGAL   				  0x04
#define 	R1_CRC_ERR   				  0x08
#define 	R1_ERASE_SEQ 				  0x10
#define 	R1_ADDR_ERR  				  0x20
#define 	R1_PARAM_ERR 				  0x40
/* R2 format responses - second byte only, first is identical to R1 */
#define 	R2_LOCKED      				  0x01
#define 	R2_WP_FAILED   				  0x02
#define 	R2_ERROR       				  0x04
#define 	R2_CTRL_ERR    				  0x08
#define 	R2_ECC_FAIL    				  0x10
#define 	R2_WP_VIOL     				  0x20
#define 	R2_ERASE_PARAM 				  0x40
#define 	R2_RANGE_ERR   				  0x80

#define GETBIT(in, bit) ((in & (1<<bit)) >> bit)
#define SD_CMD_BLOCK_LENGTH		6
#define SD_DATA_BLOCK_LENGTH	515
#define SD_WAIT_R1_TIMEOUT		100000
/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"SPI communicate with SD card demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
" Demo SPI module, read SD card's CID register and display via UART0\n\r"
"********************************************************************************\n\r";
// SPI Configuration structure variable
SPI_CFG_Type SPI_ConfigStruct;
// SPI Data Setup structure variable
SPI_DATA_SETUP_Type xferConfig;

uint8_t sd_cmd_buf[SD_CMD_BLOCK_LENGTH];
uint8_t sd_data_buf[SD_DATA_BLOCK_LENGTH];
/************************** PRIVATE FUNCTIONS *************************/
void CS_Init(void);
void CS_Force(int32_t state);
void print_menu(void);
sd_connect_status SD_GetCardConnectStatus(void);
uint8_t crc_7(uint8_t old_crc, uint8_t data);
uint8_t crc_7final(uint8_t old_crc);
uint32_t SD_SendReceiveData_Polling(void* tx_buf, void* rx_buf, uint32_t length);
void SD_SendCommand(uint8_t cmd, uint8_t *arg);
sd_error SD_WaitR1(uint8_t *buffer, uint32_t length, uint32_t timeout);
sd_error SD_WaitDeviceIdle(uint32_t num_char);
sd_error SD_Init(uint8_t retries);
sd_error SD_GetCID(void);
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/

/*-------------------------PRIVATE FUNCTIONS------------------------------*/

/*********************************************************************//**
 * @brief 		Initialize CS pin as GPIO function to drive /CS pin
 * 				due to definition of CS_PORT_NUM and CS_PORT_NUM
 * @param		None
 * @return		None
 ***********************************************************************/
void CS_Init(void)
{
	GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
	GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
}
/*********************************************************************//**
 * @brief 		Drive CS output pin to low/high level to select slave device
 * 				via /CS pin state
 * @param[in]	state State of CS output pin that will be driven:
 * 				- 0: Drive CS pin to low level
 * 				- 1: Drive CS pin to high level
 * @return		None
 ***********************************************************************/
void CS_Force(int32_t state)
{
	if (state){
		GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	}else{
		GPIO_ClearValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	}
}
/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}
/*********************************************************************//**
 * @brief		check if SD card is inserted or not
 * @param[in]	none
 * @return 		SD_CONNECTED or SD_DISCONNECTED
 **********************************************************************/
sd_connect_status SD_GetCardConnectStatus(void)
{
	sd_connect_status ret=SD_DISCONNECTED;

	if((GPIO_ReadValue(SD_DETECT_PORTNUM)&(1<<SD_DETECT_PINNUM))== 0)
		ret = SD_CONNECTED;

	return ret;
}
/*********************************************************************//**
 * @brief		Calculate CRC-7 as required by SD specification
 * @param[in]	- old_crc: 0x00 to start new CRC
 * 			    or value from previous call to continue.
 * 				- data: data byte to add to CRC computation
 * @return 		CRC-7 checksum which MUST be augmented by crc_7augment() before used
 **********************************************************************/
uint8_t crc_7(uint8_t old_crc, uint8_t data)
{
  uint8_t new_crc,x;

  new_crc = old_crc;
  for (x = 7; x >= 0; x--) {
    new_crc <<= 1;
    new_crc |= GETBIT(data,x);
    if (GETBIT(new_crc, 7) == 1) {
      new_crc ^= 0x89; /*CRC-7's polynomial is x^7 + x^3 + 1*/
    }
    if(x==0) break;
  }
  return new_crc;
}
/*********************************************************************//**
 * @brief		Provides the zero-padding final step to CRC-7
 * @param[in]	- old_crc: value from last crc_7()call
 * @return 		Finalized CRC-7 checksum
 **********************************************************************/
uint8_t crc_7final(uint8_t old_crc)
{
  uint8_t new_crc,x;

  new_crc = old_crc;
  for (x = 0; x < 7; x++) {
    new_crc <<= 1;
    if (GETBIT(new_crc, 7) == 1) {
      new_crc ^= 0x89; /*CRC-7's polynomial is x^7 + x^3 + 1*/
    }
  }
  return new_crc;
}
/*********************************************************************//**
 * @brief		Send/receive data over SPI bus
 * @param[in]	- tx_buf: pointer to transmit buffer.
 * 					      NULL if send 0xFF.
 * 				- rx_buf: pointer to receive buffer
 * 						  NULL if nothing to receive.
 * 			    - length: number of data to send or receive
 * @return 		the actual data sent or received.
 **********************************************************************/
uint32_t SD_SendReceiveData_Polling(void* tx_buf, void* rx_buf, uint32_t length)
{
	//uint16_t i;

	CS_Force(0);
	//for(i=0;i<1000;i++);

	xferConfig.tx_data = tx_buf;
	xferConfig.rx_data = rx_buf;
	xferConfig.length = length;
	SPI_ReadWrite(LPC_SPI, &xferConfig, SPI_TRANSFER_POLLING);

	//for(i=0;i<1000;i++);
	CS_Force(1);
	return xferConfig.counter;
}
/*********************************************************************//**
 * @brief		Send command to SD card
 * @param[in]	- cmd: SD command code
 * 			    - arg: pointer to array of 4x8 bytes, argument of command
 * @return 		n/a
 **********************************************************************/
void SD_SendCommand(uint8_t cmd, uint8_t *arg)
{
	uint8_t crc = 0x00;

	/* First byte has framing bits and command */
	sd_cmd_buf[0] = 0x40 | (cmd & 0x3f);
	sd_cmd_buf[1] = arg[0];
	sd_cmd_buf[2] = arg[1];
	sd_cmd_buf[3] = arg[2];
	sd_cmd_buf[4] = arg[3];
	//calculate CRC
	crc = crc_7(crc, sd_cmd_buf[0]);//start new crc-7
	crc = crc_7(crc, sd_cmd_buf[1]);
	crc = crc_7(crc, sd_cmd_buf[2]);
	crc = crc_7(crc, sd_cmd_buf[3]);
	crc = crc_7(crc, sd_cmd_buf[4]);
	crc = crc_7final(crc);
	sd_cmd_buf[5] = (crc << 1) | 0x01;//stop bit

	SD_SendReceiveData_Polling(sd_cmd_buf,NULL,SD_CMD_BLOCK_LENGTH);
}
/*********************************************************************//**
 * @brief		Wait for SD card R1 response
 * @param[in]	- buffer: pointer to receive buffer
 * 			    - length: length of receive data, must equal 1+actual length of data
 * 					      length = 0 if receive R1 only
 * 				- timeout: timeout for retry
 * @return 		error code
 **********************************************************************/
sd_error SD_WaitR1(uint8_t *buffer, uint32_t length, uint32_t timeout)
{
    uint32_t j;
	uint8_t dummy[2];
	uint8_t wait_idle;

	/* No null pointers allowed */
	if (buffer == NULL)	return SD_CMD_BAD_PARAMETER;

	/* Wait for start bit on R1 */
	j=0;dummy[0]=0xFF;
	while (GETBIT(dummy[0],7) == 1)
	{
		if (j>timeout)return SD_ERROR_TIMEOUT;
		dummy[0]=0x00;
		SD_SendReceiveData_Polling(NULL,dummy,1);
		j++;
	}
	*buffer=dummy[0];//store R1
	if (length > 0)//read followed data
	{
		/* Wait for start token on data portion, if any */
		dummy[0] = 0xff;j = 0;
		while (dummy[0] != 0xfe)
		{
			if (j > timeout)return SD_ERROR_TIMEOUT;
			dummy[0]=0x00;
			SD_SendReceiveData_Polling(NULL,dummy,1);
			if ((dummy[0] != 0xff) && (dummy[0] != 0xfe)) // not idle or start token?
			{
				return SD_ERROR_TOKEN;
			}
			j++;
		}
		/* Read all bytes */
		SD_SendReceiveData_Polling(NULL,(buffer+1),(length - 1));
	}
	/* Some more bit clocks to finish internal SD operations */
	dummy[0]=0x00;wait_idle=0;
	while((dummy[0]!=0xff)&&(wait_idle<20))
	{
		dummy[0]=0x00;
		SD_SendReceiveData_Polling(NULL,dummy,1);
		for(j=0;j<1000;j++);
		wait_idle++;
	}
	if(wait_idle>=20) return SD_ERROR_BUS_NOT_IDLE;
	return SD_OK;
}
/*********************************************************************//**
 * @brief		Wait for SD card idle
 * @param[in]	- num_char: number characters (8 bits clock) to wait
 * @return 		device already in idle state or need more time
 **********************************************************************/
sd_error SD_WaitDeviceIdle(uint32_t num_char)
{
	uint8_t dummy[2]={0,0};
	uint32_t i=0;

	while ((i < num_char) && (dummy[0] != 0xff))
	{
		dummy[0]=0x00;
		SD_SendReceiveData_Polling(NULL,dummy,1);
		if (dummy[0] == 0xff)
		{
			dummy[0]= 0x00;
			SD_SendReceiveData_Polling(NULL,dummy,1);
			if (dummy[0] == 0xff)
			{
				dummy[0]= 0x00;
				SD_SendReceiveData_Polling(NULL,dummy,1);
			}
		}
		i++;
	}
	if (dummy[0] != 0xff)return SD_ERROR_TIMEOUT;

	return SD_OK;
}
/*********************************************************************//**
 * @brief		Initialize SD card in SPI mode
 * @param[in]	- retries: number retry time
 * @return 		initialization successful or terminated with specific error code
 **********************************************************************/
sd_error SD_Init(uint8_t retries)
{
	uint8_t rxdata,errors;
	uint8_t SD_arg[4]={0,0,0,0};
	uint16_t i;

	// initialize SPI configuration structure to default
	SPI_ConfigStructInit(&SPI_ConfigStruct);
	// Initialize SPI peripheral with parameter given in structure above
	SPI_Init(LPC_SPI, &SPI_ConfigStruct);
	// Initialize /CS pin to GPIO function
	CS_Init();
	// check for SD card insertion
	_DBG("\n\rPlease plug-in SD card!");
	while(SD_GetCardConnectStatus()==SD_DISCONNECTED);
	_DBG("...Connected!\n\r");
	// Wait for bus idle
	if(SD_WaitDeviceIdle(160) != SD_OK) return SD_ERROR_BUS_NOT_IDLE;
	_DBG("Initialize SD card in SPI mode...");

	errors = 0;
	/* Send the CMD0_GO_IDLE_STATE while CS is asserted */
	/* This signals the SD card to fall back to SPI mode */
	while(errors < retries)
	{
		SD_SendCommand(CMD0_GO_IDLE_STATE, SD_arg);
		if(SD_WaitR1(&rxdata,0,1000)!= SD_OK)
		{
			errors++;
			continue;
		}
		if(rxdata != R1_IDLE)
		{
			errors++;
			continue;
		}
		else break;
	}
	if(errors >= retries)return SD_ERROR_CMD0;

	/* Check if the card is not MMC */
	/* Start its internal initialization process */
	while(1)
	{
		SD_SendCommand(CMD55_APP_CMD, SD_arg);
		if(SD_WaitR1(&rxdata,0,1000)!= SD_OK) return SD_ERROR_CMD55;

		SD_SendCommand(ACMD41_SEND_OP_COND, SD_arg);
		SD_WaitR1(&rxdata,0,1000);
		if (rxdata & R1_IDLE) //in_idle_state = 1
			for (i = 0; i < 1000; i++); /* wait for a while */
		else break; //in_idle_state=0 --> ready
	}
	/* Enable CRC */
	SD_arg[3] = 0x01;
	SD_SendCommand(CMD59_CRC_ON_OFF, SD_arg);
	if(SD_WaitR1(&rxdata,0,1000)!= SD_OK) return SD_ERROR_CMD59;
	if(rxdata != R1_NOERROR) return SD_ERROR_CMD59;

	return SD_OK;
}
/*********************************************************************//**
 * @brief		Get SD card's CID register
 * @param[in]	none
 * @return 		OK or NG
 * 				OK: the returned data from SD card is stored in sd_data_buf
 **********************************************************************/
sd_error SD_GetCID(void)
{
	uint8_t SD_arg[4]={0,0,0,0};

	SD_SendCommand(CMD10_SEND_CID, SD_arg);
	if(SD_WaitR1(sd_data_buf,18,1000)!= SD_OK) return SD_NG;
	if(sd_data_buf[0]!= R1_NOERROR) return SD_NG;
	return SD_OK;
}
/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main SPI program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	sd_error sd_status;
	uint8_t i;
	uint8_t tem8;
	uint32_t tem32;
	/*
	 * Initialize SPI pin connect
	 * P0.15 - SCK;
	 * P0.16 - SSEL - used as GPIO
	 * P0.17 - MISO
	 * P0.18 - MOSI
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PinCfg.Funcnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	//Initialize SD card detection pin P4.29
	PinCfg.Portnum = 4;
	PinCfg.Pinnum = 29;
	PinCfg.Funcnum = 0;//GPIO function
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(SD_DETECT_PORTNUM, (1<<SD_DETECT_PINNUM), 0);//input

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	//initialize SD card
	sd_status = SD_Init(10);
	switch(sd_status)
	{
	case SD_ERROR_CMD0:
		_DBG("Fail CMD0\n\r");
		break;
	case SD_ERROR_CMD55:
		_DBG("Fail CMD55\n\r");
		break;
	case SD_ERROR_ACMD41:
		_DBG("Fail ACMD41\n\r");
		break;
	case SD_ERROR_CMD59:
		_DBG("Fail CMD59\n\r");
		break;
	case SD_ERROR_BUS_NOT_IDLE:
		_DBG("Fail...Device is not in idle state.\n\r");
		break;
	case SD_OK:
		_DBG("Done!\n\r");
		break;
	default:
		_DBG("Fail\n\r");
		break;
	}
	if(sd_status == SD_OK)
	{
		//Clear receive buffer
		for(i=1;i<18;i++)sd_data_buf[i]=0;

		_DBG("Reading SD card's CID register...");
		if(SD_GetCID()!= SD_OK)
		{
			_DBG("Fail\n\r");
			SPI_DeInit(LPC_SPI);
		}
		else
		{
			_DBG("Done!");
			_DBG("\n\rManufacture ID: ");_DBH(sd_data_buf[1]);
			_DBG("\n\rApplication ID: ");_DBC(sd_data_buf[2]);_DBC(sd_data_buf[3]);
			_DBG("\n\rProduct name: ");
				for(i=4;i<9;i++) _DBC(sd_data_buf[i]);
			_DBG("\n\rProduct revision: ");
				tem8 = (sd_data_buf[9]&0xF0)>>4;_DBD(tem8);
				_DBG(".");
				tem8 = (sd_data_buf[9]&0x0F);_DBD(tem8);
			_DBG("\n\rProduct serial number: ");
				tem32= (sd_data_buf[13]<<24)|(sd_data_buf[12]<<16)|
						(sd_data_buf[11]<<8)|(sd_data_buf[10]<<0);
				_DBH32(tem32);
			_DBG("\n\rManufacturing date: ");
				tem8 = (sd_data_buf[15]&0x0F);_DBD(tem8);
				_DBG("/");
				tem8 = ((sd_data_buf[14]&0x0F)<<4) | ((sd_data_buf[15]&0xF0)>>4);
				_DBG("2");_DBD(tem8);
		}
	}
	else
    // DeInitialize SPI peripheral
		SPI_DeInit(LPC_SPI);
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
