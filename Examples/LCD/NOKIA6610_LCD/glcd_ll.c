/*************************************************************************
 *
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : glcd_ll.c
 *    Description : GLCD low level functions
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 30123 $
 **************************************************************************/
#include "glcd_ll.h"
#include "lpc_types.h"
#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"

/*************************************************************************
 * Function Name: GLCD_SetReset
 * Parameters: Boolean State
 * Return: none
 *
 * Description: Set reset pin state
 *
 *************************************************************************/
void GLCD_SetReset (Bool State)
{
	if(State)
	{
		LPC_GPIO3->FIOSET |= (1UL<<25);
	}
	else
	{
		LPC_GPIO3->FIOCLR |= (1<<25);
	}
}

/*************************************************************************
 * Function Name: GLCD_Backlight
 * Parameters: Int8U Light
 * Return: none
 *
 * Description: Set backlight pin state
 *
 *************************************************************************/
void GLCD_Backlight (uint8_t Light)
{
	LPC_PWM1->MR3 = BACKLIGHT_OFF + Light;
	LPC_PWM1->LER |= (1<<3);
}

/*************************************************************************
 * Function Name: GLCD_LLInit
 * Parameters: none
 * Return: none
 *
 * Description: Init Reset and Backlight control outputs
 *
 *************************************************************************/
void GLCD_LLInit (void)
{
	// LCD Reset output
	LPC_GPIO3->FIODIR |= (1UL<<25);

	GLCD_SetReset(0);

	// LCD backlight PWM 8bit init
	LPC_PINCON->PINSEL7 |= (3<<20) ;    // assign P3.26 to PWM1.3
	LPC_SC->PCONP |= (1<<6);     		// enable clock of PWM1
	LPC_PWM1->TCR &= ~(1<<3);
	LPC_PWM1->TCR &= ~(1<<0);			// disable counting
	LPC_PWM1->TCR |= (1<<1);			// reset
	LPC_PWM1->CTCR &= ~(3<<0);			// from prescaler
	LPC_PWM1->MCR = 2 ;					// Reset on PWMMR0
	LPC_PWM1->PCR &= ~(1<<3);			// Selects single edge controlled mode for PWM3
	LPC_PWM1->PCR |= (1<<11);			// The PWM3 output enabled

	LPC_PWM1->PR = 0;
	LPC_PWM1->MR0 = 0xFF;				// 8bit resolution
	LPC_PWM1->LER |= (1<<0);
	LPC_PWM1->MR3 = 0;
	LPC_PWM1->LER |= (1<<3);
	LPC_PWM1->TCR |= (1<<3);			// enable PWM function
	LPC_PWM1->TCR &= ~(1<<1);			// release reset
	LPC_PWM1->TCR |= (1<<0);			// enable counting
	GLCD_Backlight(0);
}

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
void GLCD_SPI_ChipSelect (Bool Select)
{
	if (Select)
	{
		LPC_GPIO1->FIOCLR |= (1<<21);
	}
	else
	{
		LPC_GPIO1->FIOSET |= (1UL<<21);
	}
}

/*************************************************************************
 * Function Name: GLCD_SPI_SetWordWidth
 * Parameters: Int32U Width
 * Return: Boolean
 *
 * Description: Set SSP 0 word width
 *
 *************************************************************************/
Bool GLCD_SPI_SetWordWidth (uint32_t Width)
{
	if(4 > Width || Width > 16)
	{
		return(FALSE);
	}

	LPC_SSP0->CR0 &= ~(0x0F);
	LPC_SSP0->CR0 |= (Width - 1) & 0x0F;
	return(TRUE);
}

/*************************************************************************
 * Function Name: GLCD_SPI_SetClockFreq
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Set SSP 0 clock
 *
 *************************************************************************/
uint32_t GLCD_SPI_SetClockFreq (uint32_t Frequency)
{
	uint32_t Fspi = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_SSP0);
	uint32_t Div = 2;
	while((Div * Frequency) < Fspi)
	{
		if((Div += 2) == 254)
		{
			break;
		}
	}
	LPC_SSP0->CPSR = Div;
	return(Fspi/Div);
}

/*************************************************************************
 * Function Name: GLCD_SPI_Init
 * Parameters: uint32_t Clk, uint32_t Width
 * Return: none
 *
 * Description: Init SSP0
 *
 *************************************************************************/
void GLCD_SPI_Init(uint32_t Clk, uint32_t Width)
{
	uint32_t i;
	volatile uint32_t Dummy;
// 	Assign GPIO to SSP0 - SCK, MOSI, MISO
	LPC_PINCON->PINSEL3 |= (3<<8);  // SCK0 - P1.20
	LPC_PINCON->PINSEL3 |= (3<<14);	// MISO0 - P1.23
	LPC_PINCON->PINSEL3 |= (3<<16);	// MOSI0 - P1.24

  // Chip select
	LPC_GPIO1->FIODIR |= (1UL<<21); //SSEL0 used as GPIOP1.21

	GLCD_SPI_ChipSelect(FALSE);

  // Spi init
	LPC_SC->PCONP |= (1<<21);	// SSP0 clock enable
    LPC_SSP0->CR1  &= ~(1<<1); 	// Disable module
	LPC_SSP0->CR1  &= ~(1<<0);	// Disable Loop Back Mode
	LPC_SSP0->CR1  &= ~(1<<2);	// Master mode
	LPC_SSP0->CR0  &= ~(3<<4);	// SPI
	LPC_SSP0->CR0 &= ~(1<<6);	// CPOL = 0
	LPC_SSP0->CR0 &= ~(1<<7);	// CPHA = 0
	LPC_SSP0->IMSC = 0;			// disable all interrupts
	LPC_SSP0->DMACR = 0;		// disable DMA
	LPC_SSP0->CR1  |= (1<<1);	// Enable module
	for (i = 0; i < 8; i++ )
	{
		Dummy = LPC_SSP0->DR; // clear the RxFIFO
	}
	// Set SSP clock frequency
	GLCD_SPI_SetClockFreq(Clk);
	// Set data width
	GLCD_SPI_SetWordWidth(Width);
}

/*************************************************************************
 * Function Name: GLCD_SPI_TranserByte
 * Parameters: uint32_t Data
 * Return: uint32_t
 *
 * Description: Transfer byte from SSP0
 *
 *************************************************************************/
uint32_t GLCD_SPI_TranserByte (uint32_t Data)
{
	while(!(LPC_SSP0->SR & (1<<1))); //check bit TNF
	LPC_SSP0->DR = Data;
	while((LPC_SSP0->SR & (1<<4))); //check bit BSY
	return(LPC_SSP0->DR);
}

/*************************************************************************
 * Function Name: GLCD_SPI_SendBlock
 * Parameters: pInt8U pData, uint32_t Size
 *
 * Return: void
 *
 * Description: Write block of data to SSP
 *
 *************************************************************************/
void GLCD_SPI_SendBlock (unsigned char *pData, uint32_t Size)
{
	volatile uint32_t Dummy;
	uint32_t OutCount = Size;
	while (OutCount)
	{
		while((LPC_SSP0->SR & (1<<1)) && OutCount)
    	{
			LPC_SSP0->DR = *pData++ | 0x100;  // Data
			--OutCount;
    	}
	}
	while(LPC_SSP0->SR & (1<<4)); //check bit BSY
  // draining RX Fifo
	while (LPC_SSP0->SR &(1<<2)) // check bit RNE
	{
		Dummy = LPC_SSP0->DR;
	}
}

/*************************************************************************
 * Function Name: GLCD_SPI_ReceiveBlock
 * Parameters: unsigned char * pData, uint32_t Size
 *
 * Return: void
 *
 * Description: Read block of data from SSP
 *
 *************************************************************************/
void GLCD_SPI_ReceiveBlock (unsigned char *pData, uint32_t Size)
{
	uint32_t Delta = 0;
	while (Size || Delta)
  	{
		while((LPC_SSP0->SR & (1<<1)) && (Delta < SSP_FIFO_SIZE) && Size)
		{
			LPC_SSP0->DR = 0xFFFF;
			--Size; ++Delta;
		}
		while (LPC_SSP0->SR & (1<<2)) //check bit RNE
		{
			*pData++ = LPC_SSP0->DR;
			--Delta;
		}
  	}
}
