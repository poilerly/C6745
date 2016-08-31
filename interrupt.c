/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
 *      Author: poiler
 */

#include "main.h"

uint16_t count = 0;

void INTC_Init(void)
{
	// Map EDMA3_CC0_INT1 Interrupts to DSP INT4
    INTC_INTMUX1 = (8 << 0);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4 interrupts
	IER = (1 << 1) | (1 << 4);  // ʹ���ж�EDMA3_CC0_INT1
}


interrupt void EDMA3_CC0_INT1_isr(void)
{
    uint32_t regIPR;
    while(EDMA3_IPR != 0)
    {
        // Read EDMA3 Interrupt Pending Register
        regIPR = EDMA3_IPR;

        if(regIPR & (1 << 18))
        {
            // Clear the corresponding bit in the Interrupt Pending Interrupt
            EDMA3_ICR = (1 << 18);  //������ICR��Ӧλд��1,���ֶ�����IPR����Ӧλ
            count++;
            if(99 == count)
            {
                SW_BREAKPOINT
                count = 0;
            }
        }
    }
}
