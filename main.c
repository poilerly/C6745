/*
 * main.c
 *
 *  Created on: 2016年4月10日
 *      Author: poiler
 */
#include "main.h"

#pragma DATA_SECTION (sampledata,".Daydream")
uint16_t sampledata[6144];				// 采集1024点,6个通道,每个通道16位

uint16_t AD_Parameter0 = 0x0BFF;		//ADS8556 Control Register setting 0
uint16_t AD_Parameter1 = 0xFC05;        //ADS8556 Control Register setting 1
uint32_t data_count=0;

int main(void)
{
	uint32_t i;
	uint16_t *dataPointer;

	C6745_Init();

	dataPointer = (uint16_t*)sampledata;
	for(i = 0; i < (1024 * 6); i++)
		*dataPointer++ = 1;

	GPIO_ADS8556_Reset();		// AD_STB置为高电平并复位ADS8556

	GPIO_ADS8556_Interrupt();	// 设置AD_BUSY/INT 为GP3[7]的中断

	INTC_Init();				// 中断设置及相应中断服务程序

	SPI1_Init();					// SPI0初始化

	eHRPWM0();					// 输出PWM波(CONVST_x signal)

	GPIO_ADS8556_Release();	// ADS8556脱离复位

	_enable_interrupts();		// 使能全局中断

    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT0 = AD_Parameter0;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT0 = AD_Parameter1;

	while(1){}

	//_disable_interrupts();
}


void delay(uint32_t x)
{
	uint32_t i,j;
	for(i=100;i>0;i--)
		for(j=x;j>0;j--);
}
