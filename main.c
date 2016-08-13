/*
 * main.c
 *
 *  Created on: 2016年4月10日
 *      Author: poiler
 */
#include "main.h"

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

	//手动产生事件23:GPIO BANK 3 中断
    EDMA3_ESR = 0x00800000; //CPU通过写事件置位寄存器(ESR)启动一个EDMA3通道
    EDMA3_ICR = 0x00800000; //清除挂起的中断(由事件23产生)

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
