/*
 * INTC.c
 *
 *  Created on: 2016��4��27��
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
	INTC_EVTCLR1 |= 0x000008000;     // ����¼�43(SPI1_INT)���жϱ�־λ
    while(SPI1_SPIBUF & 0x80000000);
    sampledata[data_count++] = (uint16_t)SPI1_SPIBUF;
	//�ڷ���ģʽ��,��ȡSPIEMU�Ĵ�����������RXINTFLG��־
	//ʵ������ʹ��SPI0_SPIBUF

	if(data_count >  899)
	{
		SW_BREAKPOINT;
		data_count = 0;
	}
	_enable_interrupts();
}


interrupt void GPIO_B3INT_isr (void)
{
	_disable_interrupts();
	GPIO_BINTEN &= ~0x00000008; // Bank3�ж�ʹ�����
	INTC_EVTCLR1 |= 0x00100000; // �¼�52(GPIO_B3INT)�жϱ�־λ���

	while(!(SPI1_SPIFLG & 0x00000200));
	SPI1_SPIDAT1 = AD_Parameter0;
    while(!(SPI1_SPIFLG & 0x00000200));
    SPI1_SPIDAT1 = AD_Parameter1;

	GPIO_BINTEN |= 0x00000008;  // ����Bank3�ж�ʹ��
	_enable_interrupts();
}

