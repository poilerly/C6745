/*
 * init.h
 *
 *  Created on: 2016年4月17日
 *      Author: poiler
 */
#ifndef MCASP_EDMA3_MAIN_H_
#define MCASP_EDMA3_MAIN_H_

//Include Header
#include <stdio.h>
#include <stdint.h>
#include <c6x.h>
#include "Header/SYSCFG.h"
#include "Header/PLL.h"
#include "Header/PSC.h"
#include "Header/EMIFB.h"
#include "Header/Cache.h"
#include "Header/INTC.h"
#include "Header/GPIO.h"
#include "Header/EDMA3.h"
#include "Header/McASP.h"
#include "Header/eHRPWM.h"
#include "Header/SPI.h"

#define SW_BREAKPOINT	asm( " SWBP 0" );
#define WAVE_POINT 1500 // 在每个周波中每个通道采集1500点

#define DATA1 0
#define DATA2 1


//Init Functions
void Init_Data_Cache(void);
void Init_System_Config(void);
void PSC0_lPSC_enable(unsigned int PD, unsigned int LPSC_num);
void PSC1_lPSC_enable(unsigned int PD, unsigned int LPSC_num);
void Init_PSC(void);
void Init_PLL(void);
void Init_EMIFB(void);
void C6745_Init(void);


//Interrupt Functions
void INTC_Init(void);
extern void intcVectorTable (void);
interrupt void EDMA3_CC0_INT1_isr (void);


//User Functions
void GPIO_ADS8556_Reset(void);
void GPIO_ADS8556_Release(void);
void GPIO_ADS8556_Interrupt(void);
void eHRPWM0(void);
void SPI1_Init(void);
void EDMA3_SPI1_Init(void);

void delay(uint32_t x);

#endif /* MCASP_EDMA3_MAIN_H_ */
