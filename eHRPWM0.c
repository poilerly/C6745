/*
 * eHRPWM0.c
 *
 *  Created on: 2016��5��15��
 *      Author: poiler
 */

#include "main.h"

/*	TBģ�������ʱ��Ƶ��	TBCLK = SYSCLKOUT/(HSPCLKDIV * CLKDIV)������HSPCLKDIVΪ14��SYSCLKOUTΪ180MHz	*/
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

	// ��ʱʱ��Ϊ180MHz/10/16��Լ����1.125MHz
	eHRPWM0_TBCTL &= ~0x1F80; // clkdiv��hspclkdiv
	eHRPWM0_TBCTL |= 0x0280; // hspclkdiv����Ϊ5h��/10
	eHRPWM0_TBCTL |= ( clkdivset << 0xA ); // ����clkdivλ

	eHRPWM0_TBCTL |= 0x0008; // ��ʹ��ӳ��Ĵ���
	eHRPWM0_TBCTL &= ~0x0003; // ��������ģʽ

	eHRPWM0_TBPRD = 14; // 1125000/15 = 75000Hz;  1125000/22=51200HZ

	eHRPWM0_TBCTL &= ~0x0004; // ��������ͬ���ź�
	eHRPWM0_TBCTL |= 0x0030; // ����EPWMxSYNCO�źţ�ͬ�������
	eHRPWM0_TBCTL |= 0xc000; // �ڷ����¼�ʱePWMʱ���������Ķ�����Free�𣿣���
}

/*	�������Ƚ���(TB���TBCNT����CMPA��CMPB���ϱȽϣ���������һ��ʱ�����һ����Ӧ�¼�)��ʼ��	*/
void CC_init()
{
	eHRPWM0_CMPCTL |= 0x0010; // CMPA�Ĵ�����������ģʽ��ʹ����Ч�Ƚ�A�Ĵ���
//	eHRPWM0_CMPCTL |= 0x0003; // ��λ��SHDWAMODE = 1ʱ����������
	eHRPWM0_CMPA = 0x0006; // duty = 40%������EPWMxA��Ƶ�ʺ�ռ�ձ�
	                      // ��鿴��������36ҳ
	eHRPWM0_CMPCTL |= 0x0040; // CMPA�Ĵ�����������ģʽ��ʹ����Ч�Ƚ�A�Ĵ���
//	eHRPWM0_CMPCTL |= 0x000C; // ��λ��SHDWBMODE = 1ʱ����������
	eHRPWM0_CMPB = 0;
}

/*		AQģ���ʱ��֮���û���κι�ϵ����ֻ�ں�������¼�	*/
void AQ_init()
{
	eHRPWM0_AQCTLA &= ~0x0fff;
	eHRPWM0_AQCTLA |= 0x003C; // ������CAU��������ʱ��ת��PRD��ת
	eHRPWM0_AQCTLB &= ~0x0fff; // �޶��������ǻ��ǻᴥ���ж�

	eHRPWM0_AQSFRC &= ~0x001B; // �������ǿ��A��B�޶������鿴���ϲ����ڵ�����ʽ�µ��¼�˳��
}

/*
 * 	eHRPWM0A���Բ���75KHz��PWM����SYSCLK2Ϊ180MHz�������κ��жϵȲ���
 */
void eHRPWM0(void)
{
	TB_init();
	CC_init();
	AQ_init();

	eHRPWM0_DBCTL &= ~0x0003; // DBģ��ʹ��bypassģʽ(��鿴50ҳ�������ϣ�DBCTL[OUT_MODE]ȫ��Ϊ0����EPWMxA/B��·)
	eHRPWM0_PCCTL &= ~0x0001; // ����ն����ģ��(��鿴53ҳ���ϣ�PCCTL[CHPEN]���Կ���PWMն����ģ��ʹ�ܻ��ֹ)

//	eHRPWM0_TZSEL &= ~0x0100; // ��ֹ��������¼�
//	eHRPWM0_TZSEL &= ~0x0001; // ��ֹ��������¼�
	eHRPWM0_TZSEL &= ~0x0101;
	eHRPWM0_ETSEL &= ~0x000f; // ��ο���63ҳ�ṹͼ���������ETSEL[INT]λ����Ϊ�ж�ʹ��λ��ֹ�ж��¼�����
	eHRPWM0_HRCNFG &= ~0x0003; // ��ֹ�߾�����ģ��
}
