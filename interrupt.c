/*
 * INTC.c
 *
 *  Created on: 2016年4月27日
 *      Author: poiler
 */

#include "main.h"

extern uint8_t harmonicflag;
extern int16_t sampledata1[];
extern int16_t sampledata2[];
extern int16_t vola_sample[];
extern int16_t volb_sample[];
extern int16_t volc_sample[];
extern int16_t cura_sample[];
extern int16_t curb_sample[];
extern int16_t curc_sample[];


//#pragma DATA_SECTION (vol_a,".DataInSdram")     // A通道电压一周波采样数据
//int16_t vol_a[WAVE_POINT];
//#pragma DATA_SECTION (vol_b,".DataInSdram")     // B通道电压一周波采样数据
//int16_t vol_b[WAVE_POINT];
//#pragma DATA_SECTION (vol_c,".DataInSdram")     // C通道电压一周波采样数据
//int16_t vol_c[WAVE_POINT];
//#pragma DATA_SECTION (cur_a,".DataInSdram")     // A通道电流一周波采样数据
//int16_t cur_a[WAVE_POINT];
//#pragma DATA_SECTION (cur_b,".DataInSdram")     // B通道电流一周波采样数据
//int16_t cur_b[WAVE_POINT];
//#pragma DATA_SECTION (cur_c,".DataInSdram")     // C通道电流一周波采样数据
//int16_t cur_c[WAVE_POINT];

#pragma DATA_SECTION (Origin,".DataInSdram")    //不知道这样是否是野指针
OriginData Origin;


uint8_t datasource = DATA1;
uint16_t fftpoint = 0;


/*
 * 把ADS8556采样一个周波的6个通道的数据分别分对应到各相电压电流
 */
void DistributeData(void)
{
    uint16_t i;
    int16_t *data;
    if(datasource == DATA1)
    {
        data = sampledata1;
        datasource = DATA2;
    }
    else if(datasource == DATA2)
    {
        data = sampledata2;
        datasource = DATA1;
    }
    for(i = 0; i < 1500; i++)
    {
        Origin->ua[i]=data[i*6];
        Origin->ub[i]=data[i*6+1];
        Origin->uc[i]=data[i*6+2];
        Origin->ia[i]=data[i*6+3];
        Origin->ib[i]=data[i*6+4];
        Origin->ic[i]=data[i*6+5];
    }
}


void INTC_Init(void)
{
	// Map EDMA3_CC0_INT1 Interrupts to DSP INT4
    INTC_INTMUX1 = (8 << 0);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4 interrupts
	IER = (1 << 1) | (1 << 4);  // 使能中断EDMA3_CC0_INT1
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
            DistributeData();

            if(harmonicflag == FALSE)
            {
                /*
                 * 对电压和电流离散数组进行10分频,提取出1024点用于FFT计算,
                 * 故采样率为75000/10=7500.
                 */
                for(i = 0; i < (WAVE_POINT / 10); i++)
                {
                    temp = i * 10;
                    vola_sample[fftpoint] = Origin->ua[temp];
                    volb_sample[fftpoint] = Origin->ub[temp];
                    volc_sample[fftpoint] = Origin->uc[temp];
                    cura_sample[fftpoint] = Origin->ia[temp];
                    curb_sample[fftpoint] = Origin->ib[temp];
                    curc_sample[fftpoint] = Origin->ic[temp];
                    fftpoint++;
                    if(fftpoint >= FFT_POINT)   // 150*6=900,150*7=1050
                    {
                        fftpoint = 0;
                        harmonicflag = TRUE;
                        break;
                    }
                }
            }

            // Clear the corresponding bit in the Interrupt Pending Interrupt
            EDMA3_ICR = (1 << 18);  //必须向ICR相应位写入1,来手动清零IPR中相应位
        }
    }
}

