/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
 *      Author: poiler
 */

#include "main.h"

//extern uint16_t AD_ParameterH;
//extern uint16_t AD_ParameterL;
//uint16_t count = 0;

void INTC_Init(void)
{
	// Map EDMA3_CC0_INT1/GPIO_B3INT Interrupts to DSP INT5/4
	INTC_INTMUX1 = (8 << 8) | (52 << 0);
//    INTC_INTMUX1 = (8 << 8);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4-INT5 interrupts
	IER = (1 << 1) | (1 << 4) | (1 << 5);
//	IER = (1 << 1) | (1 << 5);  // ֮ǰ��û��ʹ���ж�EDMA3_CC0_INT1
	//�ѹ�һֱû�н�ȥ,����Ҳ��ʹ��,��ͨ��������.
}


interrupt void EDMA3_CC0_INT1_isr(void)
{
//    uint32_t regIPR, IxBitMask, IxCounter;
//    while(EDMA3_IPR != 0)
//    {
//        // Read Interrupt Pending Register
//        regIPR = EDMA3_IPR;
//
//        // Loop for Set Interrupt Pending Bit
//        for(IxCounter = 0; IxCounter < 32; IxCounter++)
//        {
//            IxBitMask = 1 << IxCounter;
//            if(regIPR & IxBitMask)
//            {
//                if(19 == IxCounter)
//                {
//
//                }
//
//                if(18 == IxCounter)
//                {
//                    EDMA3_ICR = IxBitMask;  // Clear Pending Interrupt
////                    count++;
////                    if(count == 100 )
////                    {
////                        SW_BREAKPOINT
////                        count = 0;
////                    }
//                }
//                break;
//            }
//        }
//    }
}


interrupt void GPIO_B3INT_isr (void)
{
    _disable_interrupts();
    GPIO_BINTEN &= ~0x00000008; // Bank3�ж�ʹ�����
    INTC_EVTCLR1 |= 0x00100000; // �¼�52(GPIO_B3INT)�жϱ�־λ���

//    EDMA3_ESR = (1 << 19);     // CPU����EDMA3 �����¼�19

    GPIO_BINTEN |= 0x00000008;  // ����Bank3�ж�ʹ��
    _enable_interrupts();
}

