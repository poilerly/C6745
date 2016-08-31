/*
 * EDMA3.c
 *
 *  Created on: 2016��8��12��
 *      Author: poiler
 */

#include "main.h"

#pragma DATA_SECTION (sampledata,".Daydream")
uint16_t sampledata[6144];  // �ɼ�1024��,6��ͨ��,ÿ��ͨ��16λ
uint16_t AD_Parameter[6] = {0x0BFF,0xFC05,0x0BFF,0xFC05,0x0BFF,0xFC05};

/*
 * �ѵ�ַ0x01C04000ǿ��ת����EDMA3CC_PaRAMSetOvly��,
 * ������һ��EDMA3CC_PaRAMSetOvly����,Ȼ��ֵ��
 */
static EDMA3CC_PaRAMSetOvly EDMA3CC_PARAMSET = (EDMA3CC_PaRAMSetOvly) EDMA3PaRAM_BASE;

void PaRAM_Set23_Transmit(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[23].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[23].OPT = (1 << 20) | (1 << 8) | ((23 << 12) & 0x0003F000);
    //����ʹ��ͨ������,TCCHEN=1,ITCCHEN=0,TCC = 19.��ͨ��23�������һ��16λʱ,����ͨ��19
    EDMA3CC_PARAMSET->PaRAMSet[23].OPT = (0 << 23) | (1 << 22) | (1 << 20)  | ((19 << 12) & 0x0003F000);

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
    EDMA3CC_PARAMSET->PaRAMSet[23].BCNTRLD_LINK = (0 <<16) | 0x4F60;
}


void PaRAM_Set123_Transmit_23Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[123].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[123].OPT = (1 << 20) | (1 << 8) | ((23 << 12) & 0x0003F000);
    //����ʹ��ͨ������,TCCHEN=1,ITCCHEN=0,TCC = 19.��ͨ��23�������һ��16λʱ,����ͨ��19
    EDMA3CC_PARAMSET->PaRAMSet[123].OPT = (0 << 23) | (1 << 22) | (1 << 20)  | ((19 << 12) & 0x0003F000);

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
    EDMA3CC_PARAMSET->PaRAMSet[123].BCNTRLD_LINK = (0 <<16) | 0x4F60;
}


void PaRAM_Set19_Transmit_23chaining(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[19].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[19].OPT = (1 << 20)  | (1 << 8) | ((23 << 12) & 0x0003F000);
    EDMA3CC_PARAMSET->PaRAMSet[19].OPT = (1 << 20)  | ((19 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[19].SRC = (((uint32_t)&AD_Parameter) + 2);
    EDMA3CC_PARAMSET->PaRAMSet[19].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[19].BCNT_ACNT = (5 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[19].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[19].DSTBIDX_SRCBIDX = (0 << 16) | 2 << 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[19].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[19].BCNTRLD_LINK = (0 <<16) | 0x4F80;
}


void PaRAM_Set124_Transmit_19Linking(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[124].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[124].OPT = (1 << 20)  | (1 << 8) | ((23 << 12) & 0x0003F000);
    EDMA3CC_PARAMSET->PaRAMSet[124].OPT = (1 << 20)  | ((19 << 12) & 0x0003F000);

    // Initialize EDMA Event Src and Dst Addresses
    EDMA3CC_PARAMSET->PaRAMSet[124].SRC = (((uint32_t)&AD_Parameter) + 2);
    EDMA3CC_PARAMSET->PaRAMSet[124].DST = (uint32_t)&SPI1_SPIDAT1;

    // Set EDMA Event PaRAM A,B,C CNT (ACNT in bytes)
    EDMA3CC_PARAMSET->PaRAMSet[124].BCNT_ACNT = (5 << 16) | 2;
    EDMA3CC_PARAMSET->PaRAMSet[124].Rsvd_CCNT = 1;

    // Set EDMA Event PaRAM SRC/DST BIDX bytes
    EDMA3CC_PARAMSET->PaRAMSet[124].DSTBIDX_SRCBIDX = (0 << 16) | 2 << 0;

    // Set EDMA Event PaRAM SRC/DST CIDX
    EDMA3CC_PARAMSET->PaRAMSet[124].DSTCIDX_SRCCIDX = (0 << 16) | 0;

    // Set EDMA Event PaRAM LINK and BCNTRLD
    EDMA3CC_PARAMSET->PaRAMSet[124].BCNTRLD_LINK = (0 <<16) | 0x4F80;
}


void PaRAM_Set18_Receive(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[18].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[18].OPT = (1 << 20) | (1 <<8) | ((18 << 12) & 0x0003F000);
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
    EDMA3CC_PARAMSET->PaRAMSet[18].BCNTRLD_LINK = (0 <<16) | 0x4FE0;
}


void PaRAM_Set127_Receive_18Link(void)
{
    // Reset EDMA2 PaRAM OPT Register
    EDMA3CC_PARAMSET->PaRAMSet[127].OPT = 0x00000000;

    // Config PaRAM OPT: Enable TC Interrupt; Set Transfer complete code(TCC)
    // FIFO�Ŀ��Ϊ16-bit(1 << 8),��������ж�ʹ��(1 << 20),�����������Ϊ(23 << 12)
//    EDMA3CC_PARAMSET->PaRAMSet[1].OPT = (1 << 20) | (1 <<8) | ((18 << 12) & 0x0003F000);
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
    EDMA3CC_PARAMSET->PaRAMSet[127].BCNTRLD_LINK = (0 <<16) | 0x4FE0;
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
    uint16_t i;
    uint16_t *dataPointer;

    EDMA3_Reset();

    // Event 18 -> SPI1 �����ж�
    // Event 19 -> SPI1 �����ж�
    // Event 23 -> GPIO Bank3 �ж�
    // Enable Channel 18 & 19 & 23 to DSP (Region 1)
    EDMA3_DRAE1 |= (1 << 18) | (1 << 19) | (1 << 23);
//    EDMA3_DRAE1 |= (1 << 18);

    // Assign Channel 18 to Queue 0
    // Assign Channel 19 to Queue 1
    // Assign Channel 23 to Queue 1
    EDMA3_DMAQNUM2 = 0x10001000;

    PaRAM_Set23_Transmit();
    PaRAM_Set123_Transmit_23Linking();
    PaRAM_Set19_Transmit_23chaining();
    PaRAM_Set124_Transmit_19Linking();
    PaRAM_Set18_Receive();
    PaRAM_Set127_Receive_18Link();

    // Enable Event 18 & 19 & Event 23
    EDMA3_EESR |= (1 << 18) | (1 << 19) | (1 << 23);
//    EDMA3_EESR |= (1 << 18);

    // Enable Interrupt 18 & Interrupt 23 when transfer completion
    EDMA3_IESR |= (1 << 18) | (1 << 19) | (1 << 23);
//    EDMA3_IESR |= (1 << 18);

    // Initialize sampledata Buffers
    dataPointer = (uint16_t*)sampledata;
    for(i = 0; i < (1024 * 6); i++)
        *dataPointer++ = 1;
}
