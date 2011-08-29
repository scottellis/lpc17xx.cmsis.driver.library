/*************************************************************************
 *
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : glcd_ll.h
 *    Description : GLCD low level include file
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28532 $
 **************************************************************************/
#include "lpc_types.h"

#ifndef __GLCD_LL_H
#define __GLCD_LL_H


#define BACKLIGHT_OFF   0x40
#define BACKLIGHT_ON    0x20
#define SSP_FIFO_SIZE   8

/*************************************************************************
 * Function Name: GLCD_SetReset
 * Parameters: Boolean State
 * Return: none
 *
 * Description: Set reset pin state
 *
 *************************************************************************/
void GLCD_SetReset (Bool State);

/*************************************************************************
 * Function Name: GLCD_Backlight
 * Parameters: Int8U Light
 * Return: none
 *
 * Description: Set backlight level
 *
 *************************************************************************/
void GLCD_Backlight (uint8_t Light);

/*************************************************************************
 * Function Name: GLCD_LLInit
 * Parameters: none
 * Return: none
 *
 * Description: Init Reset and Backlight control outputs
 *
 *************************************************************************/
void GLCD_LLInit (void);

/*************************************************************************
 * Function Name: GLCD_SPI_ChipSelect
 * Parameters: Boolean Select
 * Return: none
 *
 * Description: SSP0 Chip select control
 * Select = true  - Chip is enable
 * Select = false - Chip is disable
 *
 *************************************************************************/
void GLCD_SPI_ChipSelect(Bool Select);

/*************************************************************************
 * Function Name: GLCD_SPI_SetWordWidth
 * Parameters: uint32_t Width
 * Return: Boolean
 *
 * Description: Set SSP 0 word width
 *
 *************************************************************************/
Bool GLCD_SPI_SetWordWidth (uint32_t Width);

/*************************************************************************
 * Function Name: GLCD_SPI_SetClockFreq
 * Parameters: uint32_t Frequency
 * Return: uint32_t
 *
 * Description: Set SSP 0 clock
 *
 *************************************************************************/
uint32_t GLCD_SPI_SetClockFreq (uint32_t Frequency);

/*************************************************************************
 * Function Name: GLCD_SPI_Init
 * Parameters: uint32_t Clk, uint32_t Width
 * Return: none
 *
 * Description: Init SSP0
 *
 *************************************************************************/
void GLCD_SPI_Init(uint32_t Clk, uint32_t Width);

/*************************************************************************
 * Function Name: GLCD_SPI_TranserByte
 * Parameters: uint32_t Data
 * Return: uint32_t
 *
 * Description: Transfer byte from SSP0
 *
 *************************************************************************/
uint32_t GLCD_SPI_TranserByte (uint32_t Data);

/*************************************************************************
 * Function Name: GLCD_SPI_SendBlock
 * Parameters: pInt8U pData, uint32_t Size
 *
 * Return: void
 *
 * Description: Read byte from SSP
 *
 *************************************************************************/
void GLCD_SPI_SendBlock (unsigned char *pData, uint32_t Size);

/*************************************************************************
 * Function Name: GLCD_SPI_ReceiveBlock
 * Parameters: unsigned char *pData, uint32_t Size
 *
 * Return: void
 *
 * Description: Read byte from SSP
 *
 *************************************************************************/
void GLCD_SPI_ReceiveBlock (unsigned char *pData, uint32_t Size);

#endif // __GLCD_LL_H
