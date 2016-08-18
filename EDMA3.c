/*
 * EDMA3.c
 *
 *  Created on: 2016年8月12日
 *      Author: poiler
 */

#include "main.h"

#pragma DATA_SECTION (sampledata,".Daydream")
uint16_t sampledata[6144];  // 采集1024点,6个通道,每个通道16位
#pragma DATA_SECTION (AD_Parameter,".Daydream")
uint16_t AD_Parameter[6] = {0x0BFF,0xFC05,0x0BFF,0xFC05,0x0BFF,0xFC05};
uint16_t AD_test;
//uint16_t AD_Parameter[6] = {0xFC05,0x0BFF,0xFC05,0x0BFF,0xFC05,0x0BFF};

/*
 * 把地址0x01C04000强制转化成EDMA3CC_PaRAMSetOvly型,
 * 再声明一个EDMA3CC_PaRAMSetOvly变量,然后赋值。
 */
static EDMA3CC_PaRAMSetOvly EDMA3CC_PARAMSET = (EDMA3CC_PaRAMSetOvly) EDMA3PaRAM_BASE;

void PaRAM_Set23_Transmit(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[23].OPT = 0x00000000;

    // Config PaRAM OPT:
    // 这里使用通道连接,TCCHEN=1,ITCCHEN=0,TCC = 19.当通道23传输完第一个16位时,启动通道19
    // 但是有个新的想法:GPIO BANK3中断事件发生,传输完第一个16位后,当传输完成会不会自动产生
    // SPI1发生中断事件,EDMA3会不会接着传输剩下的5个16位呢?可以不用通道连接?
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.
    // 试试二维数据传输如何?
    EDMA3CC_PARAMSET->PaRAMSet[23].OPT = ((23 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[23].SRC = (uint32_t)&AD_Parameter;
    EDMA3CC_PARAMSET->PaRAMSet[23].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[23].BCNT_ACNT = (1 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[23].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[23].DSTBIDX_SRCBIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[23].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[23].BCNTRLD_LINK = (0 << 16) | 0x4F60;
}


void PaRAM_Set123_Transmit_23Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[123].OPT = 0x00000000;

    // Config PaRAM OPT:
    // 这里使用通道连接,TCCHEN=1,ITCCHEN=0,TCC = 19.当通道23传输完第一个16位时,启动通道19
    // 但是有个新的想法:GPIO BANK3中断事件发生,传输完第一个16位后,当传输完成会不会自动产生
    // SPI1发生中断事件,EDMA3会不会接着传输剩下的5个16位呢?可以不用通道连接?
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.
    EDMA3CC_PARAMSET->PaRAMSet[123].OPT = ((23 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[123].SRC = (uint32_t)&AD_Parameter;
    EDMA3CC_PARAMSET->PaRAMSet[123].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[123].BCNT_ACNT = (1 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[123].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[123].DSTBIDX_SRCBIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[123].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[123].BCNTRLD_LINK = (0 << 16) | 0x4F60;
}


void PaRAM_Set19_Transmit(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[19].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // 传输完成中断使能(1 << 20),传输结束代码为(19 << 12)
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.但这样的话SRC和BCNT都不会更新了,不对.
    EDMA3CC_PARAMSET->PaRAMSet[19].OPT = ((19 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[19].SRC = (((uint32_t)&AD_Parameter) + 2);
    EDMA3CC_PARAMSET->PaRAMSet[19].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[19].BCNT_ACNT = (5 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[19].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[19].DSTBIDX_SRCBIDX = (0 << 16) | (2 << 0);

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[19].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
//    EDMA3CC_PARAMSET->PaRAMSet[19].BCNTRLD_LINK = (0 <<16) | 0x4F80;
    EDMA3CC_PARAMSET->PaRAMSet[19].BCNTRLD_LINK = (0 << 16) | 0x4FA0;
}


void PaRAM_Set124_Transmit_19Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[124].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // 传输完成中断使能(1 << 20),传输结束代码为(19 << 12)
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.但这样的话SRC和BCNT都不会更新了,不对.
    EDMA3CC_PARAMSET->PaRAMSet[124].OPT = ((19 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[124].SRC = (((uint32_t)&AD_Parameter) + 2);
    EDMA3CC_PARAMSET->PaRAMSet[124].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[124].BCNT_ACNT = (5 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[124].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[124].DSTBIDX_SRCBIDX = (0 << 16) | (2 << 0);

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[124].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
//    EDMA3CC_PARAMSET->PaRAMSet[124].BCNTRLD_LINK = (0 <<16) | 0x4F80;
    EDMA3CC_PARAMSET->PaRAMSet[124].BCNTRLD_LINK = (0 << 16) | 0x4FA0;
}


void PaRAM_Set125_Transmit_19Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[125].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // 传输完成中断使能(1 << 20),传输结束代码为(19 << 12)
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.但这样的话SRC和BCNT都不会更新了,不对.
    EDMA3CC_PARAMSET->PaRAMSet[125].OPT = ((19 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[125].SRC = (((uint32_t)&AD_Parameter) + 2);
    EDMA3CC_PARAMSET->PaRAMSet[125].DST = (uint32_t)&AD_test;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[125].BCNT_ACNT = (1 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[125].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[125].DSTBIDX_SRCBIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[125].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[125].BCNTRLD_LINK = (0 << 16) | 0x4F80;
}


void PaRAM_Set18_Receive(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[18].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // 传输完成中断使能(1 << 20),传输结束代码为(23 << 12)
    // OPT.3 STATIC=1时,参数RAM静态,不根据其他值来更新.但这样的话SRC和BCNT都不会更新了,不对.
    EDMA3CC_PARAMSET->PaRAMSet[18].OPT = (1 << 20) | ((18 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[18].SRC = (uint32_t)&SPI1_SPIBUF;
    EDMA3CC_PARAMSET->PaRAMSet[18].DST = (uint32_t)&sampledata;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[18].BCNT_ACNT = (6144 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[18].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[18].DSTBIDX_SRCBIDX = (2 << 16) | 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[18].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[18].BCNTRLD_LINK = (0 << 16) | 0x4FE0;
}


void PaRAM_Set127_Receive_18Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[127].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // 传输完成中断使能(1 << 20),传输结束代码为(23 << 12)
    EDMA3CC_PARAMSET->PaRAMSet[127].OPT = (1 << 20) | ((18 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[127].SRC = (uint32_t)&SPI1_SPIBUF;
    EDMA3CC_PARAMSET->PaRAMSet[127].DST = (uint32_t)&sampledata;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[127].BCNT_ACNT = (6144 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[127].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[127].DSTBIDX_SRCBIDX = (2 << 16) | 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[127].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[127].BCNTRLD_LINK = (0 << 16) | 0x4FE0;
}


void EDMA3_Reset(void)
{
    // Disable all the event
    EDMA3_EECR = 0xFFFFFFFF;

    // Clear Event Registers
    EDMA3_ECR = 0xFFFFFFFF;

    // Clear Secondary Event Register
    EDMA3_SECR = 0xFFFFFFFF;

    // Clear Event Missed Register
    EDMA3_EMCR = 0xFFFFFFFF;

    // Clear EDMA3CC Error Register
    EDMA3_CCERRCLR = 0x00010003;

    // Disable all the transfer completion interrupt
    EDMA3_IECR = 0xFFFFFFFF;

    // Clear Interrupt Pending Register
    EDMA3_ICR = 0xFFFFFFFF;
}


void EDMA3_SPI1_Init(void)
{
    // Event 18 -> SPI1 接收中断
    // Event 19 -> SPI1 发送中断
    // Event 23 -> GPIO Bank3 中断

    uint16_t i;
    uint16_t *dataPointer;

    EDMA3_Reset();

    // Enable Channel 18 & 19 & 23 to DSP (Region 1)
    EDMA3_DRAE1 |= (1 << 18) | (1 << 19) | (1 << 23);

    // Assign Channel 18 to Queue 0, Assign Channel 19 & 23 to Queue 1
    EDMA3_DMAQNUM2 = 0x10001000;

    PaRAM_Set23_Transmit();
    PaRAM_Set123_Transmit_23Linking();

    PaRAM_Set19_Transmit();
    PaRAM_Set124_Transmit_19Linking();
    PaRAM_Set125_Transmit_19Linking();

    PaRAM_Set18_Receive();
    PaRAM_Set127_Receive_18Linking();

    // Enable Event 18 & 19 & 23
    EDMA3_EESR |= (1 << 18) | (1 << 19) | (1 << 23);

    // Enable Interrupt 18 when transfer completion
    EDMA3_IESR |= (1 << 18);

    // Initialize sampledata Buffers
    dataPointer = (uint16_t*)sampledata;
    for(i = 0; i < (1024 * 6); i++)
        *dataPointer++ = 1;
}
