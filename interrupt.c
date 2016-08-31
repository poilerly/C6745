/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
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


//#pragma DATA_SECTION (vol_a,".DataInSdram")     // Aͨ����ѹһ�ܲ���������
//int16_t vol_a[WAVE_POINT];
//#pragma DATA_SECTION (vol_b,".DataInSdram")     // Bͨ����ѹһ�ܲ���������
//int16_t vol_b[WAVE_POINT];
//#pragma DATA_SECTION (vol_c,".DataInSdram")     // Cͨ����ѹһ�ܲ���������
//int16_t vol_c[WAVE_POINT];
//#pragma DATA_SECTION (cur_a,".DataInSdram")     // Aͨ������һ�ܲ���������
//int16_t cur_a[WAVE_POINT];
//#pragma DATA_SECTION (cur_b,".DataInSdram")     // Bͨ������һ�ܲ���������
//int16_t cur_b[WAVE_POINT];
//#pragma DATA_SECTION (cur_c,".DataInSdram")     // Cͨ������һ�ܲ���������
//int16_t cur_c[WAVE_POINT];

#pragma DATA_SECTION (Origin,".DataInSdram")    //��֪�������Ƿ���Ұָ��
OriginData Origin;


uint8_t datasource = DATA1;
uint16_t fftpoint = 0;


/*
 * ��ADS8556����һ���ܲ���6��ͨ�������ݷֱ�ֶ�Ӧ�������ѹ����
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
            DistributeData();

            if(harmonicflag == FALSE)
            {
                /*
                 * �Ե�ѹ�͵�����ɢ�������10��Ƶ,��ȡ��1024������FFT����,
                 * �ʲ�����Ϊ75000/10=7500.
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
            EDMA3_ICR = (1 << 18);  //������ICR��Ӧλд��1,���ֶ�����IPR����Ӧλ
        }
    }
}

