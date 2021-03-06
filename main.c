/*
 * main.c
 *
 *  Created on: 2016年4月10日
 *      Author: poiler & QW
 */

/************************************************************************
 * 命名规则:                                                            *
 * 函数采用帕斯卡命名法：多个单词组成一个名称时，每个单词的首字母大写;  *
 * 变量都采用小写;                                                      *
 * 宏定义全部大写。                                                     *
 *                                                                      *
 ************************************************************************/

#include "main.h"

uint8_t harmonicflag = FALSE;

int main(void)
{
	C6745_Init();

    FFT_Init();                // FFT初始化：Nuttall窗函数与旋转因子

	GPIO_ADS8556_Reset();		// AD_STB置为高电平并复位ADS8556

	GPIO_ADS8556_Interrupt();	// 设置AD_BUSY/INT 为GP3[7]的中断

	INTC_Init();				// 中断设置及相应中断服务程序

	SPI1_Init();				// SPI1初始化

	eHRPWM0();					// 输出PWM波(CONVST_x signal)

	_enable_interrupts();     // 使能全局中断

	GPIO_ADS8556_Release();   // ADS8556脱离复位

	EDMA3_ESR = (1 << 23);      // 手动触发事件23,CPU启动EDMA3

    while(1)
    {
        if(TRUE == harmonicflag)    //可以开始FFT计算
        {
            Harmonic_Pro();
            harmonicflag = FALSE;   //清除谐波标志
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
