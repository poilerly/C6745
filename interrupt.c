/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
 *      Author: poiler
 */

#include "main.h"

extern uint8_t harmonicflag;
extern int16_t sampledata[];
extern FFTData fft;

/* ������ԭʼ���� */
#pragma DATA_SECTION (origin,".DataInSdram")
OriginData origin;
uint16_t fftpoint = 0;

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
 * 3. ��ICR����Ӧλд1�����IPR����Ӧλ
 * 4. �ٴζ�ȡIPR:
 *    (a) ���IPR����0�����صڶ���(˵����2��4�������µ��¼�����)
 *    (b) ���IPR��0����Ҫȷ������ʹ���ж��Ǵ��ڼ���״̬
 */
interrupt void EDMA3_CC0_INT1_isr(void)
{
    uint32_t regIPR;
    uint16_t i,temp;
    while(EDMA3_IPR != 0)
    {
        // Read EDMA3 Interrupt Pending Register
        regIPR = EDMA3_IPR;

        if(regIPR & (1 << 18))
        {
            /*
             * ��ADS8556����һ���ܲ���6��ͨ�������ݷֱ�ֶ�Ӧ�������ѹ����
             */
            origin.Ua[i] = sampledata[i*6];
            origin.Ia[i] = sampledata[i*6+1];
            origin.Ub[i] = sampledata[i*6+2];
            origin.Ib[i] = sampledata[i*6+3];
            origin.Uc[i] = sampledata[i*6+4];
            origin.Ic[i] = sampledata[i*6+5];

            if(FALSE == harmonicflag)
            {
                /*
                 * �Ե�ѹ�͵�����ɢ�������10��Ƶ,��ȡ��1024������FFT����,
                 * �ʲ�����Ϊ75000/10=7500.
                 */
                for(i = 0; i < (SAMPLE_SIZE / 10); i++)
                {
                    temp = i * 10;
                    fft.Ua[fftpoint] = (float)origin.Ua[temp];
                    fft.Ia[fftpoint] = (float)origin.Ia[temp];
                    fft.Ub[fftpoint] = (float)origin.Ub[temp];
                    fft.Ib[fftpoint] = (float)origin.Ib[temp];
                    fft.Uc[fftpoint] = (float)origin.Uc[temp];
                    fft.Ic[fftpoint] = (float)origin.Ic[temp];
                    fftpoint++;
                    if(fftpoint >= FFT_SIZE)
                    {
                        fftpoint = 0;
                        harmonicflag = TRUE;
                        break;
                    }
                }
            }

            // Clear the corresponding bit in the Interrupt Pending Interrupt
            EDMA3_ICR = (1 << 18);  //������ICR��Ӧλд��1,���ֶ�����IPR����Ӧλ
        }
    }
}

