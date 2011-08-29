/*************************************************************************
 *
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_glcd.c
 *    Description : Graphic LCD driver
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28532 $
 **************************************************************************/
#include "drv_glcd.h"
#include "lpc_types.h"
#include "stdarg.h"
#include <assert.h>
#include "glcd_ll.h"

/* delay definition */
#define GLCD_DLY_10US                        80     // 10us
#define GLCD_RESET_DLY                       50     // 5ms
#define GLCD_BOOSTER_ON_DLY                1200     // 120 ms

static const GLCD_CmdCtrl_t GLCD_Cmd_Iss0[GLCD_CMD_NUMB] =
{
// (ISS=0)
// NOP - No Operation
  {0x00, GLCD_WRITE, 0, 0},
// SLPIN - Sleep in & booster off
  {0x10, GLCD_WRITE, 0, 0},
// SLPOUT - Sleep out & booster on
  {0x11, GLCD_WRITE, 0, 0},
// PTLOUT (NORON) - Partial off (Normal)
  {0x13, GLCD_WRITE, 0, 0},
// INVOFF - Display inversion off (normal)
  {0x20, GLCD_WRITE, 0, 0},
// INVON - Display inversion on
  {0x21, GLCD_WRITE, 0, 0},
// DISPOFF - Display off
  {0x28, GLCD_WRITE, 0, 0},
// DISPON - Display on
  {0x29, GLCD_WRITE, 0, 0},
// CASET - Column address set
// X_ADR start: 0h <= XS <= 83h
// X_ADR end: 0h <= XS <= 83h
  {0x2A, GLCD_WRITE, 0, 2},
// RASET - Row address set
// Y_ADR start: 0h <= YS <= 83h
// Y_ADR end: 0h <= YS <= 83h
  {0x2B, GLCD_WRITE, 0, 2},
// RAMWR - Memory write
  {0x2C, GLCD_WRITE, 0, (uint32_t)-1},
// RGBSET - Color set for 256 color display
  {0x2D, GLCD_WRITE, 0, 20},
// VSCSAD - Scroll start address of RAM
// SSA = 0, 1, 2, … , 131
  {0x37, GLCD_WRITE, 0, 1},
// IDMOFF - Idle mode off
  {0x38, GLCD_WRITE, 0, 0},
// IDMON - Idle mode on
  {0x39, GLCD_WRITE, 0, 0},
// RDID1 - Interface pixel format
  {0xDA, GLCD_READ,  0, 1},
// RDID2 - Interface pixel format
  {0xDB, GLCD_READ,  0, 1},
// RDID3 - Interface pixel format
  {0xDC, GLCD_READ,  0, 1},
// FRMSEL - External oscillator select
// Frame frequency in Temp range A
// Frame frequency in Temp range B
// Frame frequency in Temp range C
// Frame frequency in Temp range D
  {0xB4, GLCD_WRITE, 0, 4},
// FRM8SEL - External oscillator select
// Frame frequency in Temp range A
// Frame frequency in Temp range B
// Frame frequency in Temp range C
// Frame frequency in Temp range D
  {0xB5, GLCD_WRITE, 0, 4},
// TMPRNG - External oscillator select
// Temp range A
// Temp range B
// Temp range C
// Temp range D
  {0xB6, GLCD_WRITE, 0, 4},
// TMPHIS - Temp range set
  {0xB8, GLCD_WRITE, 0, 1},
// TMPREAD - Temperature read back
  {0xB7, GLCD_READ,  0, 1},
// EPWRIN - EEPROM write start
  {0xD1, GLCD_WRITE, 0, 0},
// EPWROUT - EEPROM write end
  {0xD0, GLCD_WRITE, 0, 0},
// RDEV - Read internal contrast (EV_IN)
  {0xD4, GLCD_WRITE, 0, 1},
// RDRR - Read internal resistor ratio (RR_IN)
  {0xD5, GLCD_READ,  0, 1},
// Adapted commands
// PTIN - adapted command (PTLAR + PTIN)
  {0x00, GLCD_WRITE, 0, 0},
// SCRLAR - Scroll area set (2-line unit)
// TFA = 0,1,2, … , 132
// VSA = 0,1,2, … , 132
// BFA = 0,1,2, … , 132
  {0x33, GLCD_WRITE, 0, 3},
// DATCTR - adapted command (MADCTR + COLMOD)
  {0x00, GLCD_WRITE, 0, 0},
// SETCON - Write contrast 0 - 127
  {0x25, GLCD_WRITE, 0, 1},
// Commands applicable only when ISS=1
// OSCON
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// OSCOFF
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// RMWIN
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// RMWOUT
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// VOLUP
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// VOLDOWN
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// ASCSET
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// DISCTR
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// PWRCTR
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// VOLCTR
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// Commands applicable only when ISS=0
// CLKINT - Internal oscillator select
  {0xB0, GLCD_WRITE, 0, 0},
// CLKEXT - External oscillator select
  {0xB1, GLCD_WRITE, 0, 0},
// RDDST - Read Display Status
  {0x09, GLCD_READ,  1, 4},
// SWRESET - Software reset
  {0x01, GLCD_WRITE, 0, 0},
// RDDID - Read Display ID
  {0x04, GLCD_READ,  1, 3},
// PTLAR - Partial start/end address set
// Start address (0,1,2, …, 131)
// End address (0,1,2, …, 131)
  {0x30, GLCD_WRITE, 0, 2},
// PTLON - Partial mode on
  {0x12, GLCD_WRITE, 0, 0},
// MADCTR - Tearing effect mode set & on
  {0x36, GLCD_WRITE, 0, 1},
// COLMOD - Interface pixel format
// Interface format
  {0x3A, GLCD_WRITE, 0, 1},
// APOFF - All pixel off (only for test purpose)
  {0x22, GLCD_WRITE, 0, 0},
// APON - All pixel on (only for test purpose)
  {0x23, GLCD_WRITE, 0, 0},
// BSTROFF - Booster off (only for test purpose)
  {0x02, GLCD_WRITE, 0, 0},
// BSTRON - Booster on (only for test purpose)
  {0x03, GLCD_WRITE, 0, 0},
};

static const GLCD_CmdCtrl_t GLCD_Cmd_Iss1[GLCD_CMD_NUMB] =
{
// (ISS=1)
// NOP - No Operation
  {0x25, GLCD_WRITE, 0, 0},
// SLPIN - Sleep in & booster off
  {0x95, GLCD_WRITE, 0, 0},
// SLPOUT - Sleep out & booster on
  {0x94, GLCD_WRITE, 0, 0},
// PTLOUT (NORON) - Partial off (Normal)
  {0xA9, GLCD_WRITE, 0, 0},
// INVOFF - Display inversion off (normal)
  {0xA6, GLCD_WRITE, 0, 0},
// INVON - Display inversion on
  {0xA7, GLCD_WRITE, 0, 0},
// DISPOFF - Display off
  {0xAE, GLCD_WRITE, 0, 0},
// DISPON - Display on
  {0xAF, GLCD_WRITE, 0, 0},
// CASET - Column address set
// X_ADR start: 0h <= XS <= 83h
// X_ADR end: 0h <= XS <= 83h
  {0x15, GLCD_WRITE, 0, 2},
// RASET - Row address set
// Y_ADR start: 0h <= YS <= 83h
// Y_ADR end: 0h <= YS <= 83h
  {0x75, GLCD_WRITE, 0, 2},
// RAMWR - Memory write
  {0x5C, GLCD_WRITE, 0, (uint32_t)-1},
// RGBSET - Color set for 256 color display
  {0xCE, GLCD_WRITE, 0, 20},
// VSCSAD - Scroll start address of RAM
// SSA = 0, 1, 2, … , 131
  {0xAB, GLCD_WRITE, 0, 1},
// IDMOFF - Idle mode off
  {0x38, GLCD_WRITE, 0, 0},
// IDMON - Idle mode on
  {0x39, GLCD_WRITE, 0, 0},
// RDID1 - Interface pixel format
  {0xDA, GLCD_READ,  0, 1},
// RDID2 - Interface pixel format
  {0xDB, GLCD_READ,  0, 1},
// RDID3 - Interface pixel format
  {0xDC, GLCD_READ,  0, 1},
// FRMSEL - External oscillator select
// Frame frequency in Temp range A
// Frame frequency in Temp range B
// Frame frequency in Temp range C
// Frame frequency in Temp range D
  {0xB4, GLCD_WRITE, 0, 4},
// FRM8SEL - External oscillator select
// Frame frequency in Temp range A
// Frame frequency in Temp range B
// Frame frequency in Temp range C
// Frame frequency in Temp range D
  {0xB5, GLCD_WRITE, 0, 4},
// TMPRNG - External oscillator select
// Temp range A
// Temp range B
// Temp range C
// Temp range D
  {0xB6, GLCD_WRITE, 0, 4},
// TMPHIS - Temp hysteresis range set
  {0xB8, GLCD_WRITE, 0, 1},
// TMPREAD - Temperature read back
  {0xB7, GLCD_READ,  0, 1},
// EPWRIN - EEPROM write start
  {0xCD, GLCD_WRITE, 0, 0},
// EPWROUT - EEPROM write end
  {0xCC, GLCD_WRITE, 0, 0},
// RDEV - Read internal contrast (EV_IN)
  {0x7C, GLCD_WRITE, 0, 1},
// RDRR - Read internal resistor ratio (RR_IN)
  {0x7D, GLCD_READ,  0, 1},
// Adapted commands
// PTIN - Partial area set & on
// Partial start line address
// PSL7-0 Partial start line address
// PEL7-0 Partial end line address
  {0xA8, GLCD_WRITE, 0, 2},
// SCRLAR(ASCSET) - Scroll area set (2-line unit) Mode-0
// SSL7-0 - Scroll start line
// SEL7-0 - Scroll end line
// SFL7-0 - Scroll specified line
// SMD2-0 - Area scroll mode
  {0xAA, GLCD_WRITE, 0, 4},
// DATCTR - adapted command (MADCTR + COLMOD)
// MV MX MY	 		- Row/column address control
// MRGB					- RGB
// GS2 GS1 GS0	- Gray-scale setup
  {0xBC, GLCD_WRITE, 0, 3},
// SETCON - Write contrast 0 - 127
  {0x00, GLCD_WRITE, 0, 1},
// Commands applicable only when ISS=1
// OSCON - Oscillator on
  {0xD1, GLCD_WRITE, 0, 0},
// OSCOFF - Oscillator off
  {0xD2, GLCD_WRITE, 0, 0},
// RMWIN - Read & modify write on
  {0xE0, GLCD_WRITE, 0, 0},
// RMWOUT - Read & modify write off
  {0xEE, GLCD_WRITE, 0, 0},
// VOLUP - EV increment
  {0xD6, GLCD_WRITE, 0, 0},
// VOLDOWN - EV decrement
  {0xD7, GLCD_WRITE, 0, 0},
// ASCSET - Scroll area set
// SSL7-0 - Scroll start line
// SEL7-0 - Scroll end line
// SFL7-0 - Scroll specified line
// SMD1-0 - Area scroll mode
  {0xAA, GLCD_WRITE, 0, 4},
// DISCTR - Display control
// P1
// P2
// P3
  {0xCA, GLCD_WRITE, 0, 3},
// PWRCTR - Power control set
// P13-P10
  {0x20, GLCD_WRITE, 0, 1},
// VOLCTR
  {0x81, GLCD_WRITE, 0, 2},
// Commands applicable only when ISS=0
// CLKINT - Internal oscillator select
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// CLKEXT - External oscillator select
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// RDDST - Read Display Status
  {GLCD_INV_CMD, GLCD_READ,  0, 0},
// SWRESET - Software reset
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// RDDID - Read Display ID
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// PTLAR - Partial start/end address set
// Start address (0,1,2, …, 131)
// End address (0,1,2, …, 131)
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// PTLON - Partial mode on
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// MADCTR - Tearing effect mode set & on
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// COLMOD - Interface pixel format
// Interface format
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// APOFF - All pixel off (only for test purpose)
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// APON - All pixel on (only for test purpose)
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// BSTROFF - Booster off (only for test purpose)
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
// BSTRON - Booster on (only for test purpose)
  {GLCD_INV_CMD, GLCD_WRITE, 0, 0},
};

static GLCD_ISS_t Glcd_Iss;
static Bool OddPixel = FALSE;
static uint32_t PixelHold = 0;


static pFontType_t pCurrFont = NULL;
static LdcPixel_t TextColour;
static LdcPixel_t TextBackgndColour;

static uint32_t TextX_Pos = 0;
static uint32_t TextY_Pos = 0;

static uint32_t XL_Win = DEF_X_LEFT;
static uint32_t YU_Win = DEF_Y_UP;
static uint32_t XR_Win = DEF_X_RIGHT;
static uint32_t YD_Win = DEF_Y_DOWN;

static uint32_t TabSize = TEXT_DEF_TAB_SIZE;

/*************************************************************************
 * Function Name: GLCD_SendCmd
 * Parameters: GLCD_SendCmd Cmd, unsigned char *pData, uint32_t Size
 * Return: GLCD_Status_t
 *
 * Description: Send command to the Graphic LCD
 *
 *************************************************************************/
GLCD_Status_t GLCD_SendCmd (GLCD_Cmd_t Cmd, unsigned char *pData, uint32_t Size)
{
	static Bool Write2_DRAM = FALSE; // write to DRAM mode
	static uint32_t ResRatio;             // Hold resistor ratio init by VOLCTR
                                    // and used by SETCON adapted command
	pDATCTR_Data_t pDATCTR_Data;
	uint8_t ColourSch;
	pASCSET_Data_t pASCSET_Data;
	MADCTR_Data_t DataMode = {0};
	// Pointer to current set of commands
	pGLCD_CmdCtrl_t pGLCD_CmdCtrl = (pGLCD_CmdCtrl_t)(((Glcd_Iss==GLCD_ISS_0)?
									GLCD_Cmd_Iss0:GLCD_Cmd_Iss1) + Cmd);

	uint32_t Contrast;

	if(Write2_DRAM)
	{
		// Select LCD
		GLCD_SPI_ChipSelect(1);
		if(Cmd != RAMWR)
		{
			Write2_DRAM = FALSE;
			if(PixelHold & WAITED_PIXEL)
			{
				// Flush pixel
				GLCD_SPI_TranserByte(PixelHold);
			}
			// Release CS
			GLCD_SPI_ChipSelect(0);
		}
		else
		{
			// pixels collection
			uint32_t Pixel;
			while(Size > 1)
			{
				Pixel  =  *pData++;
				Pixel |= (*pData++)<<8;
				PixelHold |= WAITED_PIXEL;
				if(!OddPixel)
				{
					// Get even pixel
					PixelHold |= PIXEL_CONV(Pixel,0);
				}
				else
				{
					// Apply odd pixel
					PixelHold |= PIXEL_CONV(Pixel,1);
					// and send together both pixels
					GLCD_SPI_SendBlock((unsigned char *)&PixelHold,3);
					PixelHold = 0;
				}
				// update the even/odd flag
				OddPixel ^= 1;
				// update the counter of the remaining bytes
				Size -= 2;
			}
			// Release CS
			GLCD_SPI_ChipSelect(0);
			return(GLCD_OK);
		}
	}

	// sanitary check of command
	if(pGLCD_CmdCtrl->Cmd == GLCD_INV_CMD)
	{
		return(GLCD_UNSUPPORTED);
	}

	// Implement adapted commands
	if(Glcd_Iss==GLCD_ISS_0)
	{
		switch(Cmd)
		{
		case PTIN:
			// PTLAR + PTIN
			GLCD_SendCmd(PTLAR,pData,0);
			GLCD_SendCmd(PTLON,NULL,0);
			return(GLCD_OK);
		case DATCTR:
			pDATCTR_Data = (pDATCTR_Data_t)pData;
				// MADCTR + COLMOD
				// Data reconstruction
			DataMode.MY = pDATCTR_Data->MY;
			DataMode.MX = pDATCTR_Data->MX;
			DataMode.MV = pDATCTR_Data->MV;
			DataMode.ML = pDATCTR_Data->ML;
			DataMode.RGB = !pDATCTR_Data->RGB;
			GLCD_SendCmd(MADCTR,(unsigned char*)&DataMode,0);
			// 4K colors Type A
			ColourSch = 3;
			return(GLCD_SendCmd(COLMOD,&ColourSch,0));
		}
	}
	else
	{
		switch(Cmd)
		{
		case SCRLAR:
			pASCSET_Data = (pASCSET_Data_t)pData;
			pASCSET_Data->Smd = 0;	// Center screen scroll mode
			GLCD_SendCmd(ASCSET,pData,0);
			return(GLCD_OK);
		case DATCTR:
			pDATCTR_Data = (pDATCTR_Data_t)pData;
			pDATCTR_Data->ML  = 0;
			pDATCTR_Data->MX = !pDATCTR_Data->MX;
			// 4K colors Type A
			pDATCTR_Data->GS  = 2;
			break;
		case SETCON:
			Contrast = *pData | (ResRatio << 8);
			return(GLCD_SendCmd(VOLCTR,(unsigned char *)&Contrast,0));
		case VOLCTR:
			ResRatio = *(pData+1);
			break;
			}
		}

	if(pGLCD_CmdCtrl->Param != (uint32_t)-1)
	{
		// Command with fixed size of parameters
		Size = pGLCD_CmdCtrl->Param;
	}

	// Select LCD
	GLCD_SPI_ChipSelect(1);

	GLCD_SPI_TranserByte(pGLCD_CmdCtrl->Cmd);
	if(Cmd == RAMWR)
	{
		// enter in write to DRAM mode
		OddPixel = FALSE;
		PixelHold = 0;
		Write2_DRAM = TRUE;
		return(GLCD_SendCmd(RAMWR,pData,Size));
	}

	// Send/Receive parameters
	if(pGLCD_CmdCtrl->Dir == GLCD_READ)
	{
		if(pGLCD_CmdCtrl->Pulse)
		{
			if(Size)
			{
				*pData++ = GLCD_SPI_TranserByte(0xFFFF);
				--Size;
			}
		}
		GLCD_SPI_SetWordWidth(8);
	}

	if(pGLCD_CmdCtrl->Dir == GLCD_READ)
	{
		GLCD_SPI_ReceiveBlock(pData,Size);
	}
	else
	{
		GLCD_SPI_SendBlock(pData,Size);
	}

	// Release CS
	GLCD_SPI_ChipSelect(0);
	if(pGLCD_CmdCtrl->Dir == GLCD_READ)
	{
		GLCD_SPI_SetWordWidth(9);
	}
	return(GLCD_OK);
}

/*************************************************************************
 * Function Name: GLCD_PowerUpInit
 * Parameters: unsigned char *pInitData
 * Return: GLCD_Status_t
 *         GLCD_OK - init pass
 *         GLCD_ID_ERROR - unsupported driver type
 * Description: Power-up initialization of Graphic LCD
 *
 *************************************************************************/
//GLCD_Status_t GLCD_PowerUpInit (unsigned char *pInitData)
GLCD_Status_t GLCD_PowerUpInit (unsigned char *pInitData)
{
	uint32_t Data;
	volatile uint32_t i;
	pDATCTR_Data_t pDATCTR_Data;
	// Init SPI and IO
	// Init low level
	GLCD_SPI_Init(GLCD_DEF_CLOCK,GLCD_DEF_WIDTH);
	GLCD_LLInit();

	// H/W Reset
	// Rest L > 10us H
	GLCD_SetReset(0);
	for(i = GLCD_DLY_10US; i ; --i);
	GLCD_SetReset(1);

	// Wait 5 ms
	Dly100us((void *)GLCD_RESET_DLY);
	// Identify LCD and command interpreter
	Glcd_Iss = GLCD_ISS_0;
	GLCD_SendCmd(RDDID,(unsigned char *)&Data,0);
	if(((Data & 0xFF) != GLCD_MANF_ID) ||(((Data >> 16)& 0xFF) != GLCD_MOD_ID))
	{
		// Iss = 1 don't support RDDID command
		Glcd_Iss = GLCD_ISS_1;
	}
	// Power Supply Set
	if(Glcd_Iss == GLCD_ISS_1)
	{
		// Support only 4K colors
		assert(GLCD_DEF_DPL_ML == 0);
		//  DISCTR (Display Control)
		//	 - F1/F2 switching period set
		//	 - Line inversion set
		Data = 0 | (0x20UL << 8) | (0x11UL << 16);
		GLCD_SendCmd(DISCTR,(unsigned char *)&Data,0);
		//  OSCON (Oscillator ON)
		GLCD_SendCmd(OSCON,NULL,0);
		// Set contrast
		Data = 0x20 | (0x0UL << 8);
		GLCD_SendCmd(VOLCTR,(unsigned char *)&Data,0);
	}
	else
	{
		// Set contrast
		Data = 0x30;
		GLCD_SendCmd(SETCON,(unsigned char *)&Data,0);
	}
	// SLPOUT (Sleep mode OFF & OSC/Booster On)
	GLCD_SendCmd(SLPOUT,NULL,0); // Sleep out
	if(Glcd_Iss == GLCD_ISS_0)
	{
		GLCD_SendCmd(BSTRON,NULL,0); // Booster voltage on
	}
	// Display Environment Set
	// Display inversion setting
	GLCD_SendCmd(GLCD_DEF_DPL_INV?INVON:INVOFF,NULL,0);
	// Display color scheme setting
	GLCD_SendCmd(GLCD_DEF_DPL_8_COLOR?IDMON:IDMOFF,NULL,0);
	// Display partial setting
	if(GLCD_DEF_DPL_PARTIAL_MODE)
	{
		GLCD_SendCmd(PTLOUT,NULL,0);
	}
	else
	{
#if GLCD_DEF_DPL_PARTIAL_MODE > 0
#error "PARTIAL_MODE not supported yet"
#endif // GLCD_DEF_DPL_PARTIAL_MODE > 0
	}
	// Memory Data Access Control
	Data = 0;
	pDATCTR_Data = (pDATCTR_Data_t)&Data;
	pDATCTR_Data->RGB = GLCD_DEF_DPL_RGB_ORD; 		// BGR / RGB order
	/* Because NXP_logo data was generated by reverse row direction so have to set MY = 1 */
	pDATCTR_Data->MY = 1; 							// no mirror Y
	pDATCTR_Data->MX = GLCD_DEF_DPL_MX; 			// no mirror X
	pDATCTR_Data->MV = GLCD_DEF_DPL_V; 				// vertical RAM write in Y/X direction
	pDATCTR_Data->ML = GLCD_DEF_DPL_ML; 			// line address order (bottom to top) / (top to bottom)
	// support only 4k type A colored scheme
	GLCD_SendCmd(DATCTR,(unsigned char *)&Data,0);
	// Display Data Write & Display On
	// COLUMN ADDRESS SET and PAGE ADDRESS SET
	Data = 0 | ((GLCD_HORIZONTAL_SIZE - 1)<<8);
	GLCD_SendCmd(RASET,(unsigned char *)&Data,0);
	GLCD_SendCmd(CASET,(unsigned char *)&Data,0);

	// Init display memory
	if(pInitData == NULL)
	{
		Data = DEF_BACKGND_COLOUR;
		for(i = 0; i < (GLCD_HORIZONTAL_SIZE * GLCD_VERTICAL_SIZE); ++i)
    {
			LCD_WRITE_PIXEL(Data);
    }
		LCD_FLUSH_PIXELS();
	}
	else
	{
		GLCD_SendCmd(RAMWR,pInitData,GLCD_HORIZONTAL_SIZE * GLCD_VERTICAL_SIZE * 2);
	}
	if(Glcd_Iss == GLCD_ISS_1)
	{
		// PWRCTR
		Data = 0x1F;
		GLCD_SendCmd(PWRCTR,(unsigned char *)&Data,0);
	}
	// Memory Data Access Control
	/* Reset default control mode after drawing */
	pDATCTR_Data = (pDATCTR_Data_t)&Data;
	pDATCTR_Data->RGB = GLCD_DEF_DPL_RGB_ORD; 		// BGR / RGB order
	pDATCTR_Data->MY = GLCD_DEF_DPL_MY; 			// no mirror Y
	pDATCTR_Data->MX = GLCD_DEF_DPL_MX; 			// no mirror X
	pDATCTR_Data->MV = GLCD_DEF_DPL_V; 				// vertical RAM write in Y/X direction
	pDATCTR_Data->ML = GLCD_DEF_DPL_ML; 			// line address order (bottom to top) / (top to bottom)
	// support only 4k type A colored scheme
	GLCD_SendCmd(DATCTR,(unsigned char *)&Data,0);

	// Wait for more than 120ms after power control command
	Dly100us((void *)GLCD_BOOSTER_ON_DLY);
	GLCD_SendCmd(GLCD_DISPLAY_STATE?DISPON:DISPOFF,NULL,0);
	return(GLCD_OK);
}

/*************************************************************************
 * Function Name: GLCD_SetFont
 * Parameters: pFontType_t pFont, LdcPixel_t Color
 *              LdcPixel_t BackgndColor
 *
 * Return: none
 *
 * Description: Set current font, font color and background color
 *
 *************************************************************************/
void GLCD_SetFont(pFontType_t pFont, LdcPixel_t Color, LdcPixel_t BackgndColor)
{
	pCurrFont = pFont;
	TextColour = Color;
	TextBackgndColour = BackgndColor;
}

/*************************************************************************
 * Function Name: GLCD_SetWindow
 * Parameters: uint32_t X_Left, uint32_t Y_Up,
 *             uint32_t X_Right, uint32_t Y_Down
 *
 * Return: none
 *
 * Description: Set draw window XY coordinate in pixels
 *
 *************************************************************************/
void GLCD_SetWindow(uint32_t X_Left, uint32_t Y_Up,
                    uint32_t X_Right, uint32_t Y_Down)
{
	assert(X_Right < GLCD_HORIZONTAL_SIZE);
	assert(Y_Down < GLCD_VERTICAL_SIZE);
	assert(X_Left < X_Right);
	assert(Y_Up < Y_Down);
	XL_Win = X_Left;
	YU_Win = Y_Up;
	XR_Win = X_Right;
	YD_Win = Y_Down;
}

/*************************************************************************
 * Function Name: GLCD_TextSetPos
 * Parameters: uint32_t X_UpLeft, uint32_t Y_UpLeft,
 *             uint32_t X_DownLeft, uint32_t Y_DownLeft
 *
 * Return: none
 *
 * Description: Set text X,Y coordinate in characters
 *
 *************************************************************************/
void GLCD_TextSetPos(uint32_t X, uint32_t Y)
{
	TextX_Pos = X;
	TextY_Pos = Y;
}

/*************************************************************************
 * Function Name: GLCD_TextSetTabSize
 * Parameters: uint32_t Size
 *
 * Return: none
 *
 * Description: Set text tab size in characters
 *
 *************************************************************************/
void GLCD_TextSetTabSize(uint32_t Size)
{
	TabSize = Size;
}

/*************************************************************************
 * Function Name: GLCD_TextCalcWindow
 * Parameters: pInt32U pXL, pInt32U pXR,
 *             pInt32U pYU, pInt32U pYD,
 *             pInt32U pH_Size, pInt32U pV_Size
 *
 * Return: Bool
 *          FALSE - out of window coordinate aren't valid
 *          TRUE  - the returned coordinate are valid
 *
 * Description: Calculate character window
 *
 *************************************************************************/
static Bool GLCD_TextCalcWindow (unsigned int *pXL, unsigned int *pXR,
                             unsigned int *pYU, unsigned int *pYD,
                             unsigned int *pH_Size, unsigned int *pV_Size)
{
	*pH_Size = pCurrFont->H_Size;
	*pV_Size = pCurrFont->V_Size;
	*pXL = XL_Win + (TextX_Pos*pCurrFont->H_Size);
	if(*pXL > XR_Win)
	{
		return(FALSE);
	}
	*pYU = YU_Win + (TextY_Pos*pCurrFont->V_Size);
	if(*pYU > YD_Win)
	{
		return(FALSE);
	}

	*pXR   = XL_Win + ((TextX_Pos+1)*pCurrFont->H_Size) - 1;
	if(*pXR > XR_Win)
	{
		*pH_Size -= *pXR - XR_Win;
		*pXR = XR_Win;
	}

	*pYD = YU_Win + ((TextY_Pos+1)*pCurrFont->V_Size) - 1;
	if(*pYD > YD_Win)
	{
		*pV_Size -= *pYD - YD_Win;
		*pYD = YD_Win;
	}

	return(TRUE);
}

/*************************************************************************
 * Function Name: GLCD_putchar
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
int GLCD_putchar (int c)
{
	unsigned char *pSrc;
	uint32_t H_Line, i, j, k;
	uint32_t xl,xr,yu,yd,Temp,V_Size, H_Size, SrcInc = 1;
	uint32_t WhiteSpaceNumb;
	if(pCurrFont == NULL)
	{
		return(-1);
	}
	H_Line = (pCurrFont->H_Size / 8) + ((pCurrFont->H_Size % 8)?1:0);
	switch(c)
	{
	case '\n':  // go to start of next line (NewLine)
		++TextY_Pos;
		break;
	case '\r':  // go to start of this line (Carriage Return)
		// clear from current position to end of line
		while(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
		{
			LCD_SET_WINDOW(xl,xr,yu,yd);
			for(i = 0; i < V_Size; ++i)
			{
				for(j = 0; j < H_Size; ++j)
				{
					LCD_WRITE_PIXEL(TextBackgndColour);
				}
			}
			++TextX_Pos;
		}
		TextX_Pos = 0;
		break;
	case '\b': // go back one position (BackSpace)
		if(TextX_Pos)
		{
			--TextX_Pos;
			// del current position
			if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
			{
				LCD_SET_WINDOW(xl,xr,yu,yd);
				for(i = 0; i < V_Size; ++i)
				{
					for(j = 0; j < H_Size; ++j)
					{
						LCD_WRITE_PIXEL(TextBackgndColour);
					}
				}
			}
		}
		break;
	case '\t':  // go to next Horizontal Tab stop
		WhiteSpaceNumb = TabSize - (TextX_Pos%TabSize);
		for(k = 0; k < WhiteSpaceNumb; ++k)
		{
			LCD_SET_WINDOW(xl,xr,yu,yd);
			if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
			{
				for(i = 0; i < V_Size; ++i)
				{
					for(j = 0; j < H_Size; ++j)
					{
						LCD_WRITE_PIXEL(TextBackgndColour);
					}
				}
				++TextX_Pos;
			}
			else
			{
				break;
			}
		}
		break;
	case '\f':  // go to top of page (Form Feed)
		// clear entire window
		H_Size = XR_Win - XL_Win;
		V_Size = YD_Win - YU_Win;
		// set character window X left, Y right
		LCD_SET_WINDOW(XL_Win,XR_Win,YU_Win,YD_Win);
		// Fill window with background font color
		for(i = 0; i <= V_Size; ++i)
		{
			for(j = 0; j <= H_Size; ++j)
			{
				LCD_WRITE_PIXEL(TextBackgndColour);
			}
		}

		TextX_Pos = TextY_Pos = 0;
		break;
	case '\a':  // signal an alert (BELl)
		TEXT_BEL1_FUNC();
		break;
	default:
		// Calculate the current character base address from stream
		// and the character position
		if((c <  pCurrFont->CharacterOffset) && (c >= pCurrFont->CharactersNuber))
		{
			c = 0;
		}
		else
		{
			c -= pCurrFont->CharacterOffset;
		}
		pSrc = pCurrFont->pFontStream + (H_Line * pCurrFont->V_Size * c);
		// Calculate character window and fit it in the text window
		if(GLCD_TextCalcWindow(&xl,&xr,&yu,&yd,&H_Size,&V_Size))
		{
			// set character window X left, Y right
			LCD_SET_WINDOW(xl,xr,yu,yd);
			// Send char data
			for(i = 0; i < V_Size; ++i)
			{
				SrcInc = H_Line;
				for(j = 0; j < H_Size; ++j)
				{
					Temp = (*pSrc & (1UL << (j&0x7)))?TextColour:TextBackgndColour;
					LCD_WRITE_PIXEL(Temp);
					if((j&0x7) == 7)
					{
						++pSrc;
						--SrcInc;
					}
				}
				// next line of character
				pSrc += SrcInc;
			}
		}
		++TextX_Pos;
	}
	LCD_FLUSH_PIXELS();
	return(c);
}

/*************************************************************************
 * Function Name: GLCD_print
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
void GLCD_print (const char *fmt)
{
	while(0 != *fmt)
	{
		if(-1 == GLCD_putchar(*fmt++))
		{
			break;
		}
	}
}
