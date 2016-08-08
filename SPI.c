/*
 * AD_SPI.c
 *
 *  Created on: 2016年7月10日
 *      Author: poiler
 */

#include "main.h"

void SPI1_Init(void)
{
	uint32_t spidat1;

	/* 1.Reset the SPI */
	SPI1_SPIGCR0 = 0;

	/* 2.Take the SPI out of reset */
	delay(255);
	SPI1_SPIGCR0 = 1;

	/* 3.Configure the SPI to master mode */
	SPI1_SPIGCR1 = 0
			| (0 << 24)		//Disable SPI
			| (0 << 16)		//Internal loop-back test mode disabled
			| (0 <<  8)		//The SPI is in active mode
			| (3 <<  0);	//Slave mode = 0, Master mode = 3

	/* 4.SPI 4-pin with chip select mode */
	SPI1_SPIPC0  = 0
			| (1 << 11)		//SOMI SPI functional pin
			| (1 << 10)		//SIMO SPI functional pin
			| (1 <<  9)		//CLK  SPI functional pin
			| (0 <<  8)		//SPIx_ENA pin is a GPIO pin
			| (1 <<  0);	//SCS[0] SPI functional pin

	/* 5.Chose the SPI data format register 0 (SPIFMT0) */
	spidat1 = 0
			| (0 << 28)		//The chip select signal is held active at the end of
			                //a transfer until a control field with new data and
							//control information is loaded into SPIDAT1.
			| (0 << 26)		//No delay will be inserted.
			| (0 << 24)		//Data word format 0 is selected
			| (0 << 16)		//SPIx_SCS[0] pin is driven low
			| (0 <<  0);	//When written, these bits will be copied to the shift
							//register if it is empty. If the shift register is not
							//empty, the TXBUF will hold the written values.
	SPI1_SPIDAT1 = spidat1;

	/* 6.Configure the SPI data rate, character length, shift direction, phase,
	 * polarity and other format options using SPIFMTn selected in step 5.
	 */
	SPI1_SPIFMT0 = 0


	        | (0 << 24)		//Delay in between transmissions
			| (0 << 23)		//Parity polarity
			| (0 << 22)		//Parity disable
			| (0 << 21)		//The master not waits for SPIx_ENA signal from slave
			| (0 << 20)		//Most significant bit is shifted out first
			| (1 << 18)		//No C2TDELAY or T2CDELAY is inserted in the chip select timings.
			| (1 << 17)		//SPI clock signal polarity   正是更改了这两个地方所以有数据
			| (1 << 16)		//SPI clock signal phase
			| (5 <<  8)		//SPI clock frequency = SPI module clock/(PRESCALE + 1) = 180M/6 = 30M
			| (16 << 0);	//SPI data word length = 16 bit

	/* 7. If SPI master, then configure the master delay options using the SPI delay
	 * register (SPIDELAY). In slave mode, SPIDELAY is not relevant.
	 */
	SPI1_SPIDELAY = 0
			| (0 << 24)		//C2TDELAY Chip-select-active-to-transmit-start-delay.
			| (0 << 16)		//T2CDELAY Transmit-end-to-chip-select-inactive-delay.
			| (0 <<  8)		//T2EDELAY Transmit-data-finished-to-SPIx_ENA-pin-inactive-time-out.
			| (0 <<  0);	//C2EDELAY Chip-select-active-to-SPIx_ENA-signal-active-time-out.

	SPI1_SPIDEF = 0
	        | (1 <<  0);    //SPIx_SCS[n] pin is driven high
	/* 8. Select the error interrupt notifications by configuring the SPI interrupt
	 * register (SPIINT0) and the SPIinterrupt level register (SPILVL).
	 */
	SPI1_SPIINT0 = 0
			| (0 << 24)		//SPIx_ENA pin is pulled high when not active
			| (0 << 16)		//DMA is not used
			| (0 <<  9)     //No interrupt will be generated upon SPIFLG.TXINTFLG being set to 1
			| (1 <<  8);	//Receive interrupt will be generated

	SPI1_SPILVL = 0;


	/* 9. Enable the SPI communication by setting the SPIGCR1.ENABLE to 1. */
	SPI1_SPIGCR1 |= (1 << 24); //Activates SPI

	/* 10. Setup and enable the DMA for SPI data handling and then enable the DMA
	 * servicing for the SPI data requests by setting the SPIINT0.DMAREQEN to 1.
	 */


	/* 11. Handle SPI data transfer requests using DMA and service any SPI error
	 * conditions using the interrupt service routine.
	 */

}
