/*
 * INTC.c
 *
 *  Created on: 2016年4月27日
 *      Author: poiler
 */

#include "main.h"

uint16_t count = 0;

void INTC_Init(void)
{
	// Map EDMA3_CC0_INT1 Interrupts to DSP INT5
    INTC_INTMUX1 = (8 << 8);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4 interrupts
	IER = (1 << 1) | (1 << 5);  // 使能中断EDMA3_CC0_INT1
}


interrupt void EDMA3_CC0_INT1_isr(void)
{
    uint32_t regIPR, IxBitMask, IxCounter;
    while(EDMA3_IPR != 0)
    {
        // Read Interrupt Pending Register
        regIPR = EDMA3_IPR;

        // Loop for Set Interrupt Pending Bit
        for(IxCounter = 0; IxCounter < 32; IxCounter++)
        {
            IxBitMask = 1 << IxCounter;
            if(regIPR & IxBitMask)
            {
                if(18 == IxCounter)
                {
                    EDMA3_ICR = IxBitMask;  // Clear Pending Interrupt
                    count++;
                    if(count == 100 )
                    {
                        SW_BREAKPOINT
                        count = 0;
                    }
                }
                break;
            }
        }
    }
}
