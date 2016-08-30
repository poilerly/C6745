/*
 * eHRPWM0.c
 *
 *  Created on: 2016年5月15日
 *      Author: poiler
 */

#include "main.h"

/*	TB模块的输入时钟频率	TBCLK = SYSCLKOUT/(HSPCLKDIV * CLKDIV)、设置HSPCLKDIV为14、SYSCLKOUT为180MHz	*/
void TB_init()
{
	uint32_t clkdiv = 0;
	uint32_t clkdivset = 0;
	clkdiv = 180/10;
	while(clkdiv > 1)
	{
		clkdiv = clkdiv >> 1;
		clkdivset++;
	}

	// 此时时钟为180MHz/10/16，约等于1.125MHz
	eHRPWM0_TBCTL &= ~0x1F80; // clkdiv、hspclkdiv
	eHRPWM0_TBCTL |= 0x0280; // hspclkdiv设置为5h，/10
	eHRPWM0_TBCTL |= ( clkdivset << 0xA ); // 设置clkdiv位

	eHRPWM0_TBCTL |= 0x0008; // 不使用映像寄存器
	eHRPWM0_TBCTL &= ~0x0003; // 递增计数模式

	eHRPWM0_TBPRD = 14; // 1125000/15 = 75000Hz;  1125000/22=51200HZ

	eHRPWM0_TBCTL &= ~0x0004; // 禁用输入同步信号
	eHRPWM0_TBCTL |= 0x0030; // 屏蔽EPWMxSYNCO信号（同步输出）
	eHRPWM0_TBCTL |= 0xc000; // 在仿真事件时ePWM时基计数器的动作：Free吗？？？
}

/*	计数器比较器(TB输出TBCNT，与CMPA和CMPB不断比较，等于任意一个时会产生一个相应事件)初始化	*/
void CC_init()
{
	eHRPWM0_CMPCTL |= 0x0010; // CMPA寄存器采用立即模式仅使用有效比较A寄存器
//	eHRPWM0_CMPCTL |= 0x0003; // 该位在SHDWAMODE = 1时，不起作用
	eHRPWM0_CMPA = 0x0006; // duty = 40%，设置EPWMxA的频率和占空比
	                      // 请查看中文资料36页
	eHRPWM0_CMPCTL |= 0x0040; // CMPA寄存器采用立即模式仅使用有效比较A寄存器
//	eHRPWM0_CMPCTL |= 0x000C; // 该位在SHDWBMODE = 1时，不起作用
	eHRPWM0_CMPB = 0;
}

/*		AQ模块和时钟之类的没有任何关系，他只在乎输入的事件	*/
void AQ_init()
{
	eHRPWM0_AQCTLA &= ~0x0fff;
	eHRPWM0_AQCTLA |= 0x003C; // 设置了CAU，增计数时反转，PRD反转
	eHRPWM0_AQCTLB &= ~0x0fff; // 无动作，但是还是会触发中断

	eHRPWM0_AQSFRC &= ~0x001B; // 单次软件强制A、B无动作，查看资料查明在递增方式下的事件顺序
}

/*
 * 	eHRPWM0A用以产生75KHz的PWM波，SYSCLK2为180MHz，无需任何中断等操作
 */
void eHRPWM0(void)
{
	TB_init();
	CC_init();
	AQ_init();

	eHRPWM0_DBCTL &= ~0x0003; // DB模块使用bypass模式(请查看50页中文资料，DBCTL[OUT_MODE]全部为0，则EPWMxA/B旁路)
	eHRPWM0_PCCTL &= ~0x0001; // 禁用斩波子模块(请查看53页资料，PCCTL[CHPEN]可以控制PWM斩波子模块使能或禁止)

//	eHRPWM0_TZSEL &= ~0x0100; // 禁止错误控制事件
//	eHRPWM0_TZSEL &= ~0x0001; // 禁止错误控制事件
	eHRPWM0_TZSEL &= ~0x0101;
	eHRPWM0_ETSEL &= ~0x000f; // 请参看第63页结构图，清楚看出ETSEL[INT]位，作为中断使能位禁止中断事件发生
	eHRPWM0_HRCNFG &= ~0x0003; // 禁止高精度子模块
}
