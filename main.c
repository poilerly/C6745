/*
 * main.c
 *
 *  Created on: 2016��4��10��
 *      Author: poiler
 */
#include "main.h"

int main(void)
{
	C6745_Init();

	GPIO_ADS8556_Reset();		// AD_STB��Ϊ�ߵ�ƽ����λADS8556

	GPIO_ADS8556_Interrupt();	// ����AD_BUSY/INT ΪGP3[7]���ж�

	INTC_Init();				// �ж����ü���Ӧ�жϷ������

	SPI1_Init();				// SPI1��ʼ��

	eHRPWM0();					// ���PWM��(CONVST_x signal)

	GPIO_ADS8556_Release();	// ADS8556���븴λ

	_enable_interrupts();		// ʹ��ȫ���ж�

	//�ֶ������¼�23:GPIO BANK 3 �ж�
//	EDMA3_ESR = 0x00800000; //CPUͨ��д�¼���λ�Ĵ���(ESR)����һ��EDMA3ͨ��
//    EDMA3_ICR = 0x00800000; //���������ж�(���¼�23����)
	EDMA3_SHADOW0_ESR = 0x00800000;
	EDMA3_SHADOW0_ICR = 0x00800000;

    while(1){}

    /*
     * ADS8556(Reset);
     * ADS8556(OutReset);
     * eHRPWM0(75000);75000Hz
     */
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
