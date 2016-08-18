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
	// Map EDMA3_CC0_INT1/GPIO_B3INT Interrupts to DSP INT5/4
//	INTC_INTMUX1 = (8 << 8) | (52 << 0);
    INTC_INTMUX1 = (8 << 8);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4-INT5 interrupts
//	IER = (1 << 1) | (1 << 4) | (1 << 5);
	IER = (1 << 1) | (1 << 5);  // 之前就没有使能中断EDMA3_CC0_INT1
	//难怪一直没有进去,现在也不使能,调通了再试试.
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
                if(19 == IxCounter)
                {

                }

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


//interrupt void GPIO_B3INT_isr (void)
//{
//    _disable_interrupts();
//    GPIO_BINTEN &= ~0x00000008; // Bank3中断使能清除
//    INTC_EVTCLR1 |= 0x00100000; // 事件52(GPIO_B3INT)中断标志位清除
//
////    EDMA3_ESR = (1 << 19);     // CPU启动EDMA3 触发事件19
//
//    GPIO_BINTEN |= 0x00000008;  // 设置Bank3中断使能
//    _enable_interrupts();
//}

