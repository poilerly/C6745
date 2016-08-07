/*
 * INTC.c
 *
 *  Created on: 2016年4月27日
 *      Author: poiler
 */

#include "main.h"

extern uint16_t sampledata[6144];
extern uint16_t AD_Parameter0;
extern uint16_t AD_Parameter1;
extern uint32_t data_count;

void INTC_Init(void)
{
	// Map SP1_INT/GPIO_B3INT Interrupts to DSP INT5/4
	INTC_INTMUX1 = (43 << 8) | (52 << 0);

	// Assign the address of the IST to the IST pointer
	ISTP = (unsigned int)intcVectorTable;

	// Clear all CPU maskable interrupts
	ICR = 0xFFF0;

	// Enable NMI, INT4-INT5 interrupts
	IER = (1 << 1) | (1 << 4) | (1 << 5);
}


interrupt void SPI1_INT_isr (void)
{
	_disable_interrupts();
	INTC_EVTCLR1 |= 0x00000020;     // 清除事件37(SPI0_INT)的中断标志位
	if(SPI1_SPIFLG & 0x00000100)
		sampledata[data_count++] = (uint16_t)SPI0_SPIBUF;
	//在仿真模式下,读取SPIEMU寄存器不会清零RXINTFLG标志

	if(data_count >  899)
	{
		SW_BREAKPOINT
		data_count = 0;
	}
	_enable_interrupts();
}


interrupt void GPIO_B3INT_isr (void)
{
	_disable_interrupts();
	GPIO_BINTEN &= ~0x00000008; // Bank3中断使能清除
	INTC_EVTCLR1 |= 0x00100000; // 事件52(GPIO_B3INT)中断标志位清除

	while(!(SPI1_SPIFLG & 0x00000200));
	SPI1_SPIDAT0 = AD_Parameter0;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT0 = AD_Parameter1;

	GPIO_BINTEN |= 0x00000008;  // 设置Bank3中断使能
	_enable_interrupts();
}

