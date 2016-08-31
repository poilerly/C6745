/*
 * main.c
 *
 *  Created on: 2016��4��10��
 *      Author: poiler
 */
#include "main.h"

#pragma DATA_SECTION (sampledata,".Daydream")
uint16_t sampledata[6144];				// �ɼ�1024��,6��ͨ��,ÿ��ͨ��16λ

uint16_t AD_ParameterL = 0xFC05;        //ADS8556 Control Register setting high
//uint16_t AD_ParameterL = 0x0BFF;		//ADS8556 Control Register setting low
uint16_t AD_ParameterH = 0x0BFF;        //ADS8556 Control Register setting low
uint32_t data_count=0;

int main(void)
{
	uint32_t i;
	uint16_t *dataPointer;

	C6745_Init();

	dataPointer = (uint16_t*)sampledata;
	for(i = 0; i < (1024 * 6); i++)
		*dataPointer++ = 1;

	GPIO_ADS8556_Reset();		// AD_STB��Ϊ�ߵ�ƽ����λADS8556

	GPIO_ADS8556_Interrupt();	// ����AD_BUSY/INT ΪGP3[7]���ж�

	INTC_Init();				// �ж����ü���Ӧ�жϷ������

	SPI1_Init();				// SPI1��ʼ��

	eHRPWM0();					// ���PWM��(CONVST_x signal)

    _enable_interrupts();       // ʹ��ȫ���ж�

	GPIO_ADS8556_Release();	// ADS8556���븴λ

//	_enable_interrupts();		// ʹ��ȫ���ж�

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

	while(1){
	    if(SPI1_SPIFLG & (1 << 8))
	        sampledata[data_count++] = (uint16_t)SPI1_SPIBUF;
	    if(data_count >= 899)
	    {
	        //SW_BREAKPOINT
	        data_count = 0;
	    }
	}

	//_disable_interrupts();
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
