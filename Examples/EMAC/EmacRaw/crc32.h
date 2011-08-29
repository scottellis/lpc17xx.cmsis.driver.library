/**********************************************************************
* $Id$		crc32.h  				2006-09-01
*//**
* @file		crc32.h
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
#ifndef __CRC32_H
#define __CRC32_H

#include "lpc_types.h"

void   crc32_init(uint32_t *pCRC);
void   crc32_add(uint32_t *pCRC, uint8_t val8);
void   crc32_end(uint32_t *pCRC);
uint32_t  crc32_bfr(void *pBfr, uint32_t size);
uint32_t do_crc_behav( long long Addr );

#endif

/*-----------------------------------------------------------*/
