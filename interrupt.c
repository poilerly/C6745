/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
 *      Author: poiler
 */

#include "main.h"

extern uint16_t AD_ParameterH;
extern uint16_t AD_ParameterL;

void INTC_Init(void)
{
	// Map EDMA3_CC0_INT1/GPIO_B3INT Interrupts to DSP INT5/4
	INTC_INTMUX1 = (8 << 8) | (52 << 0);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4-INT5 interrupts
	IER = (1 << 1) | (1 << 4) | (0 << 5);
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
                // Exit Example on Correct Interrupt
//                if(23 == IxCounter)
//                {
//                    EDMA3_ICR = IxBitMask;  // Clear Pending Interrupt
//                    printf("\ttransfer succeed...\n");
//                }

                // Exit Example on Correct Interrupt
                if(18 == IxCounter)
                {
                    EDMA3_ICR = IxBitMask;  // Clear Pending Interrupt
                    printf("\treceive succeed...\n");
                }
                break;
            }
        }
    }
}


interrupt void GPIO_B3INT_isr (void)
{
    _disable_interrupts();
    GPIO_BINTEN &= ~0x00000008; // Bank3�ж�ʹ�����
    INTC_EVTCLR1 |= 0x00100000; // �¼�52(GPIO_B3INT)�жϱ�־λ���

    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterH;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterL;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterH;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterL;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterH;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_ParameterL;

    GPIO_BINTEN |= 0x00000008;  // ����Bank3�ж�ʹ��
    _enable_interrupts();
}

