/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
 *      Author: poiler
 */

#include "main.h"

extern int16_t sampledata1[1500*6];
extern int16_t sampledata2[1500*6];
extern int16_t vva[CYCLE_SIZE];
extern int16_t vvb[CYCLE_SIZE];
extern int16_t vvc[CYCLE_SIZE];
extern int16_t cca[CYCLE_SIZE];
extern int16_t ccb[CYCLE_SIZE];
extern int16_t ccc[CYCLE_SIZE];

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

/*
 * EDMA3 Interrupt Servicing
 * 1. ��ȡ�жϹ���Ĵ���(IPR)
 * 2. ִ������Ҫ�Ĳ���
 * 3. ��ICR д1 �����IPR ����Ӧλ
 * 4. �ٴζ�ȡIPR:
 *    (a) ���IPR ����0�����صڶ���(˵����2 ��4 �������µ��¼�����)
 *    (b) ���IPR ��0����Ҫȷ������ʹ���ж��Ǵ��ڼ���״̬
 */
interrupt void EDMA3_CC0_INT1_isr(void)
{
    uint32_t regIPR;
    uint32_t i;
    while(EDMA3_IPR != 0)
    {
        // Read EDMA3 Interrupt Pending Register
        regIPR = EDMA3_IPR;

        if(regIPR & (1 << 18))
        {
            for(i = 0; i < 1500; i++)
            {
                vva[i]=sampledata1[i*6];
                vvb[i]=sampledata1[i*6+1];
                vvc[i]=sampledata1[i*6+2];
                cca[i]=sampledata1[i*6+3];
                ccb[i]=sampledata1[i*6+4];
                ccc[i]=sampledata1[i*6+5];
            }

            // Clear the corresponding bit in the Interrupt Pending Interrupt
            EDMA3_ICR |= (1 << 18);  //������ICR��Ӧλд��1,���ֶ�����IPR����Ӧλ
            count++;
            if(99 == count)
            {
                //SW_BREAKPOINT
                count = 0;
            }
        }
        if(regIPR & (1 << 19))
        {
            for(i = 0; i < 1500; i++)
            {
                vva[i]=sampledata2[i*6];
                vvb[i]=sampledata2[i*6+1];
                vvc[i]=sampledata2[i*6+2];
                cca[i]=sampledata2[i*6+3];
                ccb[i]=sampledata2[i*6+4];
                ccc[i]=sampledata2[i*6+5];
            }

            // Clear the corresponding bit in the Interrupt Pending Interrupt
            EDMA3_ICR |= (1 << 19);  //������ICR��Ӧλд��1,���ֶ�����IPR����Ӧλ
            count++;
            if(99 == count)
            {
                //SW_BREAKPOINT
                count = 0;
            }
        }

    }
}
