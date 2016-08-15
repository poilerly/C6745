/*
 * main.c
 *
 *  Created on: 2016年4月10日
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

	GPIO_ADS8556_Reset();		// AD_STB置为高电平并复位ADS8556

	GPIO_ADS8556_Interrupt();	// 设置AD_BUSY/INT 为GP3[7]的中断

	INTC_Init();				// 中断设置及相应中断服务程序

	SPI1_Init();				// SPI1初始化

	eHRPWM0();					// 输出PWM波(CONVST_x signal)

	GPIO_ADS8556_Release();	// ADS8556脱离复位

	_enable_interrupts();		// 使能全局中断

//	EDMA3_ESR = 0x00800000;     // CPU启动EDMA3 触发事件23
//	EDMA3_ICR = 0x00800000;

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
