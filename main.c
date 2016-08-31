/*
 * main.c
 *
 *  Created on: 2016��4��10��
 *      Author: poiler
 */

/************************************************************************
 * ��������:                                                            *
 * ����������˹��������������������һ������ʱ��ÿ�����ʵ�����ĸ��д;  *
 * ����������Сд;                                                      *
 * �궨��ȫ����                                                       *
 *                                                                      *
 ************************************************************************/

#include "main.h"

uint8_t harmonicflag = FALSE;

int main(void)
{
	C6745_Init();

	GPIO_ADS8556_Reset();		// AD_STB��Ϊ�ߵ�ƽ����λADS8556

	GPIO_ADS8556_Interrupt();	// ����AD_BUSY/INT ΪGP3[7]���ж�

	INTC_Init();				// �ж����ü���Ӧ�жϷ������

	SPI1_Init();				// SPI1��ʼ��

	eHRPWM0();					// ���PWM��(CONVST_x signal)

	FFT_Init(); // 4��3��Nuttall�������ļ���

	_enable_interrupts();     // ʹ��ȫ���ж�

	GPIO_ADS8556_Release();   // ADS8556���븴λ

	EDMA3_ESR = (1 << 23);      // �ֶ������¼�23,CPU����EDMA3

    while(1)
    {
        if(harmonicflag == TRUE)
        {
            HarmomicProcess();
            ParameterCalculation();
        }
    }
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
