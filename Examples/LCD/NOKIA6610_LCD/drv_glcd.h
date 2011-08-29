/*************************************************************************
 *
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_glcd.h
 *    Description : Graphic LCD driver include file
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28532 $
 **************************************************************************/
#include "drv_glcd_cnfg.h"
#include "lpc_types.h"

#ifndef  __DRV_GLCD_H
#define  __DRV_GLCD_H

#define DEF_X_LEFT         	0
#define DEF_Y_UP           	0
#define DEF_X_RIGHT        	GLCD_HORIZONTAL_SIZE
#define DEF_Y_DOWN         	GLCD_VERTICAL_SIZE
#define DEF_FONT_COLOUR    	0x000000
#define DEF_BACKGND_COLOUR 	0x00FF0
#define TEXT_DEF_TAB_SIZE  	4

#define MAX_GLCD_STR_SIZE   256
// External function
#define TEXT_BEL1_FUNC()         // generate BEL signal

// Set window
#define LCD_SET_WINDOW(xl,xr,yu,yd) \
{\
uint32_t Temp;\
  Temp = xl | (xr<<8);\
  GLCD_SendCmd(CASET,(unsigned char*)&Temp,0);\
  Temp = (yu+2) | ((yd+2)<<8);\
  GLCD_SendCmd(RASET,(unsigned char*)&Temp,0);\
}

// Send pixel
#define LCD_WRITE_PIXEL(c) GLCD_SendCmd(RAMWR,(unsigned char*)&c,2)

// Flush pixels
#define LCD_FLUSH_PIXELS() GLCD_SendCmd(NOP,NULL,0)

#define GLCD_MANF_ID 0x45
#define GLCD_MOD_ID  0x03

#define WAITED_PIXEL 0x80000000

#define PIXEL_CONV(Pixel,Order) (!Order)?(((Pixel >> 4) & 0xFF) | ((Pixel << 12) & 0xF000)):(((Pixel << 16) & 0xFF0000) | ((Pixel) & 0xF00))
// PCF8833 status register
#define PCF8833_ST_BOOSTER_ON     (1UL <<  7)
#define PCF8833_ST_Y_ADDR_ODR     (1UL <<  6)
#define PCF8833_ST_X_ADDR_ODR     (1UL <<  5)
#define PCF8833_ST_ADDR_MODE      (1UL <<  4)
#define PCF8833_ST_LINE_ADDR_ORD  (1UL <<  3)
#define PCF8833_ST_RGB_ORD        (1UL <<  2)
#define PCF8833_ST_PIXEL_FORM     (7UL << 12)
#define PCF8833_ST_IDLE_MODE      (1UL << 11)
#define PCF8833_ST_PARTIAL_MODE   (1UL << 10)
#define PCF8833_ST_SLEEP_MODE     (1UL <<  9)
#define PCF8833_ST_NORM_MODE      (1UL <<  8)
#define PCF8833_ST_V_SCROLL_MODE  (1UL << 23)
#define PCF8833_ST_INV_MODE       (1UL << 21)
#define PCF8833_ST_ALL_PIXELS_ON  (1UL << 20)
#define PCF8833_ST_ALL_PIXELS_OFF (1UL << 19)
#define PCF8833_ST_DISPLAY_MODE   (1UL << 18)
#define PCF8833_ST_TEARING_MODE   (1UL << 17)

// GLCD command interpreter type
typedef enum _GLCD_ISS_t {
	GLCD_ISS_0 = 0, GLCD_ISS_1
} GLCD_ISS_t;

// execution status
typedef enum _GLCD_Status_t {
	GLCD_OK = 0, GLCD_ID_ERROR, GLCD_UNSUPPORTED, GLCD_ERROR
} GLCD_Status_t;

// GCLD command set
typedef enum _GLCD_Cmd_t {
	// Equal commands
	NOP, SLPIN, SLPOUT, PTLOUT, INVOFF, INVON,
	DISPOFF, DISPON, CASET, RASET, RAMWR, RGBSET, VSCSAD,
	IDMOFF, IDMON, RDID1, RDID2, RDID3, FRMSEL, FRM8SEL, TMPRNG,
	TMPHIS, TMPREAD, EPWRIN, EPWROUT, RDEV, RDRR,
  // Adapted commands
  PTIN, 	// ISS=0 equal on PTLAR + PTIN
  SCRLAR,	// ISS=1 ASCSET mode 0
  DATCTR,	// ISS=0 equal on MADCTR + COLMOD
  SETCON, //
  // Commands applicable only when ISS=1
  OSCON, OSCOFF, RMWIN, RMWOUT, VOLUP, VOLDOWN, ASCSET, DISCTR,
  PWRCTR, VOLCTR,
  // Commands applicable only when ISS=0
  CLKINT, CLKEXT, RDDST, SWRESET, RDDID, PTLAR, PTLON, MADCTR,
  COLMOD, APOFF, APON, BSTROFF, BSTRON,
	GLCD_CMD_NUMB
} GLCD_Cmd_t;

// Command direction
typedef enum _GLCD_CmdDir_t {
	GLCD_WRITE = 0, GLCD_READ
} GLCD_CmdDir_t;

// Data flag
#define GLCD_DATA				      0x100

// Invalid command (some commands don't have analog in other
// command mode interpreter)
#define GLCD_INV_CMD					((uint32_t)(-1))

#define GLCD_MAX_COTRAST   63
#define GLCD_MIN_COTRAST  -64

typedef struct _GLCD_CmdCtrl_t {
	uint32_t        Cmd;
	GLCD_CmdDir_t   Dir;
	Bool 		Pulse;
	uint32_t 		Param;
} GLCD_CmdCtrl_t, *pGLCD_CmdCtrl_t;

#pragma pack(1)
typedef struct _ASCSET_Data_t {
	uint8_t Ssl;
	uint8_t Sel;
	uint8_t Sfl;
	uint8_t Smd;
} ASCSET_Data_t, *pASCSET_Data_t;

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct _DATCTR_Data_t {
	union 
	{
		uint8_t RC_AddMode;
		struct 
		{
			uint8_t MY : 1;
			uint8_t MX : 1;
			uint8_t MV : 1;
			uint8_t ML : 1;
			uint8_t : 4;
		};
	};
	union  
	{
		uint8_t RGB_Mode;
		struct 
		{
			uint8_t RGB : 1;
			uint8_t : 7;
		};
	};
	union 
	{
		uint8_t GrayScale;
		struct  
		{
			uint8_t GS  : 3;
			uint8_t Reversed: 5;
		};
	};
} DATCTR_Data_t, *pDATCTR_Data_t;

typedef struct _MADCTR_Data_t {
	union
	{
		uint8_t MemDataAccCtrl;
		struct 
		{
			uint8_t Reversed: 3;
			uint8_t RGB : 1;
			uint8_t ML  : 1;
			uint8_t MV  : 1;
			uint8_t MX  : 1;
			uint8_t MY  : 1;
		};
	};
} MADCTR_Data_t, *pMADCTR_Data_t;


typedef struct _COLMOD_Data_t {
	union
	{
		uint8_t ColourMode;
		struct
		{
			uint8_t Mode 	: 3;
			uint8_t Reversed : 5;
		};
	};
} COLMOD_Data_t, *pCOLMOD_Data_t;
#pragma pack()

typedef uint32_t LdcPixel_t, *pLdcPixel_t;

typedef struct _FontType_t {
  uint32_t H_Size;
  uint32_t V_Size;
  uint32_t CharacterOffset;
  uint32_t CharactersNuber;
  unsigned char *pFontStream;
  unsigned char *pFontDesc;
} FontType_t, *pFontType_t;

typedef struct _Bmp_t {
  uint32_t  H_Size;
  uint32_t  V_Size;
  uint32_t  BitsPP;
  uint32_t  BytesPP;
  unsigned int *pPalette;
  unsigned int *pPicStream;
  unsigned char *pPicDesc;
} Bmp_t, *pBmp_t;

extern void Dly100us(void *arg);

/*************************************************************************
 * Function Name: GLCD_SendCmd
 * Parameters: GLCD_Cmd_t Cmd, unsigned char* pData, uint32_t Size
 * Return: GLCD_Status_t
 *
 * Description: Send command to the Graphic LCD
 *
 *************************************************************************/
GLCD_Status_t GLCD_SendCmd (GLCD_Cmd_t Cmd, unsigned char *pData, uint32_t Size);

/*************************************************************************
 * Function Name: GLCD_PowerUpInit
 * Parameters: unsigned char* pInitData
 * Return: GLCD_Status_t
 *         GLCD_OK - init pass
 *			 	 GLCD_ID_ERROR - unsupported driver type
 * Description: Power-up initialization of Graphic LCD
 *
 *************************************************************************/
//GLCD_Status_t GLCD_PowerUpInit (unsigned char *pInitData);
GLCD_Status_t GLCD_PowerUpInit (unsigned char *pInitData);

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
void GLCD_SetFont(pFontType_t pFont,
                  LdcPixel_t Color, LdcPixel_t BackgndColor);

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
                    uint32_t X_Right, uint32_t Y_Down);

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
void GLCD_TextSetPos(uint32_t X, uint32_t Y);

/*************************************************************************
 * Function Name: GLCD_TextSetTabSize
 * Parameters: uint32_t Size
 *
 * Return: none
 *
 * Description: Set text tab size in characters
 *
 *************************************************************************/
void GLCD_TextSetTabSize(uint32_t Size);

/*************************************************************************
 * Function Name: GLCD_TextCalcWindow
 * Parameters: pInt32U pXL, pInt32U pXR,
 *             pInt32U pYU, pInt32U pYD,
 *             pInt32U pH_Size, pInt32U pV_Size
 *
 * Return: Boolean
 *          FALSE - out of window coordinate aren't valid
 *          TRUE  - the returned coordinate are valid
 *
 * Description: Calculate character window
 *
 *************************************************************************/
static Bool GLCD_TextCalcWindow (unsigned int *pXL, unsigned int *pXR,
		unsigned int *pYU, unsigned int *pYD,
		unsigned int *pH_Size, unsigned int *pV_Size);

/*************************************************************************
 * Function Name: GLCD_putchar
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
int GLCD_putchar (int c);

/*************************************************************************
 * Function Name: GLCD_print
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
void GLCD_print (const char *fmt);

#endif  /* __DRV_GLCD_H */
