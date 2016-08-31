/*
 * INTC.c
 *
 *  Created on: 2016年4月27日
 *      Author: poiler
 */

#include "main.h"

extern uint8_t harmonicflag;
extern int16_t sampledata[];
extern FFTData fft;

/* 采样的原始数据 6个通道,每个一个周波1500点,*/
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
	IER = (1 << 1) | (1 << 4);
}


/*
 * EDMA3 Interrupt Servicing
 * 1. 读取中断挂起寄存器(IPR)
 * 2. 执行所需要的操作
 * 3. 对ICR中相应位写1来清除IPR的相应位
 * 4. 再次读取IPR:
 *    (a) 如果IPR不是0，返回第二步(说明在2到4步中有新的事件到来)
 *    (b) 如果IPR是0，需要确认所有使能中断是处于激活状态
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

            /* 把ADS8556采样一个周波的6个通道的数据分别分对应到各相电压电流 */
            for(i = 0; i < SAMPLE_SIZE; i++)
            {
                origin.Ua[i] = sampledata[i*6];
                origin.Ia[i] = sampledata[i*6+1];
                origin.Ub[i] = sampledata[i*6+2];
                origin.Ib[i] = sampledata[i*6+3];
                origin.Uc[i] = sampledata[i*6+4];
                origin.Ic[i] = sampledata[i*6+5];
            }

            /*
             * 对电压和电流离散数组进行10分频,提取出1024点用于FFT计算,
             * 故采样率为75000/10=7500.
             */
            if(FALSE == harmonicflag)
            {
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
            EDMA3_ICR = (1 << 18);
        }
    }
}

