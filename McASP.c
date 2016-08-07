/*
 * McASP.c
 *
 *  Created on: 2016年5月10日
 *      Author: poiler
 */

#include "main.h"


void McASP0_Init(void)
{
	/* 1. Reset McASP to default values */
	McASP0_GBLCTL 	= 0;

	/* 2. Configure McASP Audio FIFO if need */

	/* 3a. Receive Register */
	McASP0_RMASK	= 0xFFFFFFFF; // 相应接收数据直接通过返还给CPU或者DMA
	McASP0_RFMT		= 0x000080F8; // 0-bit delay; MSB first; Slot size is 32 bits; peripheral port
	McASP0_AFSRCTL 	= 0x00000002; // Burst mode; FRWID=single bit; Internally frame sync; Rrsing edgen begins a frame
	McASP0_ACLKRCTL = 0x00000020; // 下降沿采样数据; Internal receive clokc source; ACLKR=AHCLKR/1
	McASP0_AHCLKRCTL= 0x00008000; // 内部时钟; 不反相; AHCLKR=AUXCLK/1=24MHz 实际作为串行位时钟使用
	McASP0_RTDM 	= 0x00000001; // Slots 0 active
	McASP0_RINTCTL 	= 0x00000020; // Enable receive data ready interrupt(RINT)
	McASP0_RCLKCHK 	= 0x00FF0008; // Leave at default

	/* 3b. Transmit Register */
	McASP0_XMASK 	= 0xFFFFFFFF; // 相应接收数据直接通过返还给CPU或者DMA
	McASP0_XFMT 	= 0x000080F8; // 0-bit delay; MSB first; Slot size is 32 bits; peripheral port
	McASP0_AFSXCTL	= 0x00000002; // Burst mode; FRWID=single bit; Internally frame sync; Rrsling edge begins a frame
	McASP0_ACLKXCTL = 0x00000020; // 下升沿发送数据; Synchronous; Internal receive clokc source; ACLKX=AHCLKX/1
	McASP0_AHCLKXCTL= 0x00008000; // 内部时钟; 不反相; AHCLKX=AUXCLK/1=24MHz 实际作为串行位时钟使用
	McASP0_XTDM 	= 0x00000001; // Slots 0 active
	McASP0_XINTCTL 	= 0x00000000; // Interrupt is disabled
	McASP0_XCLKCHK 	= 0x00FF0008; // Leave at default

	/* 3c. Serializer registers */
	McASP0_SRCTL0	= 0x00000001; // AXR[0]输出
	McASP0_SRCTL1	= 0x00000002; // AXR[1]输入
	McASP0_SRCTL2	= 0x00000002; // AXR[2]输入
	McASP0_SRCTL3	= 0x00000002; // AXR[3]输入

	/* 3d. Global registers */
	McASP0_PFUNC	= 0x03FFFFF0; // AFSR,AHCLKR,ACLKR,AFSX,AHCLKX,ACLKX,AXR3~AXR0 as McASP pin
	McASP0_PDIR		= 0xFC000001; // AFSR,AHCLKR,ACLKR,AFSX,AHCLKX,ACLKX,AXR0 is OUTPUT.AXR3~AXR1 is INPUT
	McASP0_DITCTL	= 0x00000000; // 不使用DIT mode
	McASP0_DLBCTL	= 0x00000000; // 不使用自发自收
	McASP0_AMUTE	= 0x00000000; // 静音控制寄存器

	/* 3e. DIT registers if need */

	/* 4. Start the respective high-frequency serial clocks AHCLKX and AHCLKR */
	McASP0_XGBLCTL |= (1 << 9);
	while((McASP0_XGBLCTL & (1 << 9)) != (1 << 9));
	McASP0_RGBLCTL |= (1 << 1);
	while((McASP0_RGBLCTL & (1 << 1)) != (1 << 1));

	/* 5. Start the respective serial clocks ACLKX and ACLKR */
	McASP0_XGBLCTL |= (1 << 8);
	while((McASP0_XGBLCTL & (1 << 8)) != (1 << 8));
	McASP0_RGBLCTL |= (1 << 0);
	while((McASP0_RGBLCTL & (1 << 0)) != (1 << 0));

	/* 6. EDMA3 or Interrupt set in here */
	//McASP0_REVTCTL	= 0x00000000; // Enable receive data DMA requset
	//McASP0_XEVTCTL	= 0x00000000; // Enable transmit data DMA requset

	//EDMA3_McASP0_Init();		// McASP0的DMA端口设置
	//McASP0_RINTCTL 	= 0x00000020; // Enable receive data ready interrupt(RINT)


	/* 7a. Before starting, clear the respective transmitter and receiver status registers */
	McASP0_XSTAT	= 0x0000FFFF;
	McASP0_RSTAT	= 0x0000FFFF;

	/* 7b. Take the respective serializers out of reset */
	McASP0_XGBLCTL |= (1 << 10);
	while((McASP0_XGBLCTL & (1 << 10)) != (1 << 10));
	McASP0_RGBLCTL |= (1 << 2);
	while((McASP0_RGBLCTL & (1 << 2)) != (1 << 2));

	/* 8. Verify that all transmit buffers are serviced */
	McASP0_XBUF0	= 0;
	McASP0_RBUF1	= 0;
	McASP0_RBUF2	= 0;
	McASP0_RBUF3	= 0;

	/* 9. Release state machines from reset */
	McASP0_XGBLCTL |= (1 << 11);
	while((McASP0_XGBLCTL & (1 << 11)) != (1 << 11));
	McASP0_RGBLCTL |= (1 << 3);
	while((McASP0_RGBLCTL & (1 << 3)) != (1 << 3));

	/* 10. Release frame sync generators from reset */
	McASP0_XGBLCTL |= (1 << 12);
	while((McASP0_XGBLCTL & (1 << 12)) != (1 << 12));
	McASP0_RGBLCTL |= (1 << 4);
	while((McASP0_RGBLCTL & (1 << 4)) != (1 << 4));

	/* 11. Upon the first frame sync signal, McASP transfers begin */
}

