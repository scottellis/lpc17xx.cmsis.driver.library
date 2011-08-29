/*************************************************************************
 *
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_glcd_cnfg.h
 *    Description : Graphic LCD config file
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28532 $
 **************************************************************************/

#ifndef  __DRV_GLCD_CNFG_H
#define  __DRV_GLCD_CNFG_H

// GLCD DRV Default setup data
// SPI0 default clock
#define GLCD_DEF_CLOCK	          (1000000UL)
// SPI0 default data width
#define GLCD_DEF_WIDTH	           9
// Horizontal Size of display [Pixels]
#define GLCD_HORIZONTAL_SIZE       132
// Vertical Size of display [Pixels]
#define GLCD_VERTICAL_SIZE   	     132
// Display On/Off 1/0
#define GLCD_DISPLAY_STATE         1
// Default Display inversion setting
#define GLCD_DEF_DPL_INV           1
// Default Display color scheme
#define GLCD_DEF_DPL_8_COLOR       0
// Default Display Partial mode
#define GLCD_DEF_DPL_PARTIAL_MODE  0
// Default Display Row direction
#define GLCD_DEF_DPL_MY            0
// Default Display column direction
#define GLCD_DEF_DPL_MX            1
// Default Display address direction
#define GLCD_DEF_DPL_V             0
// Default Display scan direction
#define GLCD_DEF_DPL_ML            0
// Default Display RGB order
#define GLCD_DEF_DPL_RGB_ORD       0

#endif  /* __DRV_GLCD_CNFG_H */
