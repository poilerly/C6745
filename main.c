/*
 * main.c
 *
 *  Created on: 2016��4��10��
 *      Author: poiler & QW
 */

/************************************************************************
 * ��������:                                                            *
 * ����������˹��������������������һ������ʱ��ÿ�����ʵ�����ĸ��д;  *
 * ����������Сд;                                                      *
 * �궨��ȫ����д��                                                     *
 *                                                                      *
 ************************************************************************/

#include "main.h"

uint8_t harmonicflag = FALSE;

int main(void)
{
	C6745_Init();

    FFT_Init();                // FFT��ʼ����Nuttall����������ת����

	GPIO_ADS8556_Reset();		// AD_STB��Ϊ�ߵ�ƽ����λADS8556

	GPIO_ADS8556_Interrupt();	// ����AD_BUSY/INT ΪGP3[7]���ж�

	INTC_Init();				// �ж����ü���Ӧ�жϷ������

	SPI1_Init();				// SPI1��ʼ��

	eHRPWM0();					// ���PWM��(CONVST_x signal)

	_enable_interrupts();     // ʹ��ȫ���ж�

	GPIO_ADS8556_Release();   // ADS8556���븴λ

	EDMA3_ESR = (1 << 23);      // �ֶ������¼�23,CPU����EDMA3

    while(1)
    {
        if(TRUE == harmonicflag)    //���Կ�ʼFFT����
        {
            Harmonic_Pro();
            harmonicflag = FALSE;   //���г����־
            SW_BREAKPOINT
        }
    }
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
