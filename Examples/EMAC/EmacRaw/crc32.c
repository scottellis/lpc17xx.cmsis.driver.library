/**********************************************************************
* $Id$		crc32.c  				2006-09-01
*//**
* @file		crc32.c
* @brief	Ethernet CRC module file for NXP LPC230x Family Microprocessors
* @version	1.0
* @date		01. September. 2006
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2006, NXP Semiconductor
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
#include "lpc17xx_emac.h"                        /* LPC23xx/24xx definitions */
#include "crc32.h"

/******************************************************************************
** Function name:		CRC_init
**
** Descriptions:		Begin CRC calculation.
**
** parameters:			pointer to the CRC area.
** Returned value:		None
**
******************************************************************************/
void crc32_init(uint32_t *pCRC)
{
    *pCRC = 0xffffffff;
}

/******************************************************************************
** Function name:		CRC32_add
**
** Descriptions:		Calculate CRC value one at a time
**
** parameters:			pointer to the CRC area, and passing value to get the CRC
** Returned value:		None
**
******************************************************************************/
void crc32_add(uint32_t *pCRC, uint8_t val8)
{
    uint32_t i, poly;
    uint32_t entry;
    uint32_t crc_in;
    uint32_t crc_out;

    crc_in = *pCRC;
    poly = 0xEDB88320L;
    entry = (crc_in ^ ((uint32_t) val8)) & 0xFF;
    for (i = 0; i < 8; i++)
    {
        if (entry & 1)
            entry = (entry >> 1) ^ poly;
        else
            entry >>= 1;
    }
    crc_out = ((crc_in>>8) & 0x00FFFFFF) ^ entry;
    *pCRC = crc_out;
    return;
}

/******************************************************************************
** Function name:		CRC32_end
**
** Descriptions:		Finish CRC calculation
**
** parameters:			pointer to the CRC area.
** Returned value:		None
**
******************************************************************************/
void crc32_end(uint32_t *pCRC)
{
    *pCRC ^= 0xffffffff;
}

/******************************************************************************
** Function name:		CRC32_bfr
**
** Descriptions:		Get the CRC value based on size of the string.
**
** parameters:			Pointer to the string, size of the string.
** Returned value:		CRC value
**
******************************************************************************/
uint32_t crc32_bfr(void *pBfr, uint32_t size)
{
    uint32_t crc32;
    uint8_t  *pu8;

    crc32_init(&crc32);
    pu8 = (uint8_t *) pBfr;
    while (size-- != 0)
    {
        crc32_add(&crc32, *pu8);
        pu8++ ;
    }
    crc32_end(&crc32);
    return ( crc32 );
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
