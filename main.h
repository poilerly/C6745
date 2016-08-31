/*
 * main.h
 *
 *  Created on: 2016年4月17日
 *      Author: poiler
 */
#ifndef C6745_ADS8556_MAIN_H_
#define C6745_ADS8556_MAIN_H_

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
#include "Header/dsplib.h"
#include "Header/mathlib.h"

#define SW_BREAKPOINT	asm( " SWBP 0" );

#define TRUE    1
#define FALSE   0

#define SAMPLE_SIZE 1500    // 在每个周波中每个通道采集1500点
#define FFT_SIZE    1024    // 作FFT分析的点数
#define Harm_P      51

#define DATA1   0
#define DATA2   1

#define PI  3.141592653
#define EX  1000 // 窗函数放大系数，由于窗函数小数太多，为提高精度进行放大,  在双谱线插值处更改回原来倍数

#define F1  50
#define FS  7500

#define F_TOL   (1e-06)       // 浮点数极小值


typedef struct
{
    int16_t Ua[SAMPLE_SIZE];
    int16_t Ia[SAMPLE_SIZE];
    int16_t Ub[SAMPLE_SIZE];
    int16_t Ib[SAMPLE_SIZE];
    int16_t Uc[SAMPLE_SIZE];
    int16_t Ic[SAMPLE_SIZE];
}OriginData;

typedef struct
{
    float Ua[FFT_SIZE];
    float Ia[FFT_SIZE];
    float Ub[FFT_SIZE];
    float Ib[FFT_SIZE];
    float Uc[FFT_SIZE];
    float Ic[FFT_SIZE];
}FFTData;

typedef struct
{
    float Ua;   //A相电压基波频率
    float Ia;   //A相电流基波频率
    float Ub;   //B相电压基波频率
    float Ib;   //B相电流基波频率
    float Uc;   //C相电压基波频率
    float Ic;   //C相电流基波频率
}FreqData;

typedef struct
{
    float Ua[Harm_P];
    float Ia[Harm_P];
    float Ub[Harm_P];
    float Ib[Harm_P];
    float Uc[Harm_P];
    float Ic[Harm_P];
}HarmData;


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
void Harmonic_Pro(void);

#endif /* C6745_ADS8556_MAIN_H_ */
