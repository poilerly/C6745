/*
 * GPIO.c
 *
 *  Created on: 2016Äê5ÔÂ16ÈÕ
 *      Author: poiler
 */

#include "main.h"


void GPIO_ADS8556_Reset(void)
{
	GPIO_DIR23 &= 0xFEFFFFFF;			// GP3[8]/AD_STB set output.
	GPIO_DIR67 &= 0xBFFFFFFF;			// GP7[14]/RESET_AD set output.

	GPIO_OUT_DATA23 |= 0x01000000;		// GP3[8]/AD_STB output logic high.
	GPIO_OUT_DATA67 |= 0x40000000; 		// GP7[14]/RESET_AD output logic high to reset ADS8556.
}



void GPIO_ADS8556_Release(void)
{
	GPIO_OUT_DATA67 &= 0xBFFFFFFF;		// GP7[14]/RESET_AD output logic low to release ADS8556.
}



void GPIO_ADS8556_Interrupt(void)
{
	GPIO_DIR23 |= 0x00800000;			// GP3[7]/AD_BUSY/INT set input.

 	GPIO_BINTEN |= 0x00000008;			// Enable Bank 3 interrupt.

 	GPIO_SET_FAL_TRIG23 = 0x00800000;	// high-to-low transition on GP3[7] cause interrupt.
 	GPIO_CLR_RIS_TRIG23 = 0x00800000;   // low-to-high transition on GP3[7] cause no interrupt.
}

