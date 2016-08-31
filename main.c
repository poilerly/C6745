/*
 * main.c
 *
 *  Created on: 2016��4��10��
 *      Author: poiler
 */
#include "main.h"

//extern uint16_t sampledata[6144];
//uint16_t data_count = 0;
//uint16_t AD_ParameterH = 0x0BFF;
//uint16_t AD_ParameterL = 0xFC05;

int main(void)
{
	C6745_Init();

	GPIO_ADS8556_Reset();		// AD_STB��Ϊ�ߵ�ƽ����λADS8556

	GPIO_ADS8556_Interrupt();	// ����AD_BUSY/INT ΪGP3[7]���ж�

	INTC_Init();				// �ж����ü���Ӧ�жϷ������

	SPI1_Init();				// SPI1��ʼ��

	eHRPWM0();					// ���PWM��(CONVST_x signal)

	_enable_interrupts();     // ʹ��ȫ���ж�

	GPIO_ADS8556_Release();   // ADS8556���븴λ

	EDMA3_ESR = (1 << 19);      // CPU����EDMA3 �����¼�23

    while(1)
    {

    }
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
