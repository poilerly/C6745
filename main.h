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
#include <math.h>

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

#define TRUE 1
#define FALSE 0

#define WAVE_POINT  1500    // 在每个周波中每个通道采集1500点
#define FFT_POINT   1024    // 作FFT分析的点数
#define HARM_N      21

#define DATA1 0
#define DATA2 1

#define PI 3.141592653589793
#define EX 1000 //窗函数放大系数，由于窗函数小数太，为提高精度进行放大,  在双谱线插值处更改回原来倍数

#define FB  50
#define FS  7500

typedef struct
{
    float ua[FFT_POINT];
    float ub[FFT_POINT];
    float uc[FFT_POINT];
    float ia[FFT_POINT];
    float ib[FFT_POINT];
    float ic[FFT_POINT];
}*HarmData;

typedef struct
{
    int16_t ua[WAVE_POINT];
    int16_t ub[WAVE_POINT];
    int16_t uc[WAVE_POINT];
    int16_t ia[WAVE_POINT];
    int16_t ib[WAVE_POINT];
    int16_t ic[WAVE_POINT];
}*OriginData;


//Init Functions
void Init_Data_Cache(void);
void Init_System_Config(void);
void PSC0_lPSC_enable(unsigned int PD, unsigned int LPSC_num);
void PSC1_lPSC_enable(unsigned int PD, unsigned int LPSC_num);
void Init_PSC(void);
void Init_PLL(void);
void Init_EMIFB(void);
void C6745_Init(void);
void GPIO_ADS8556_Reset(void);
void GPIO_ADS8556_Release(void);
void GPIO_ADS8556_Interrupt(void);
void eHRPWM0(void);
void SPI1_Init(void);
void EDMA3_SPI1_Init(void);

//Interrupt Functions
void INTC_Init(void);
extern void intcVectorTable (void);
interrupt void EDMA3_CC0_INT1_isr (void);


//User Functions
void delay(uint32_t x);

void FFT_Init(void);
void HarmomicProcess(void);
void ParameterCalculation(void);

#endif /* MCASP_EDMA3_MAIN_H_ */
