/*
 * harmonic.h
 *
 *  Created on: 2016年8月29日
 *      Author: QW & poiler
 */

#ifndef C6745_ADS8556_HARMONIC_H_
#define C6745_ADS8556_HARMONIC_H_

#include "Header/vars.h"
#include "main.h"


/* 要做FFT分析的数据 */
#pragma DATA_SECTION (fft,".DataInSdram")
FFTData fft;

/* 要做FFT分析并加窗后的数据 */
#pragma DATA_SECTION (fft_win,".DataInSdram")
FFTData fft_win;

/* Nuttall窗函数数值计算 */
#pragma DATA_SECTION (nuttallwin,".DataInSdram")
float nuttallwin[FFT_SIZE];

/* FFT后求得的电压流的基波频率 */
#pragma DATA_SECTION (funfreq,".DataInSdram")
FreqData funfreq;

/* 基波及各次谐波电压电流的均方根值 */
#pragma DATA_SECTION (rms,".DataInSdram")
HarmData rms;

/* 基波及各次谐波电压电流的相角 */
#pragma DATA_SECTION (phase,".DataInSdram")
HarmData phase;


/* 基波及各次谐波电压与电流之间的夹角 */
float phasea[Harm_P];
float phaseb[Harm_P];
float phasec[Harm_P];

#pragma DATA_ALIGN (w, 8)           // 存放旋转因子
float w[2*FFT_SIZE];
#pragma DATA_ALIGN(pCFFT_In, 8)     // 存放FFT计算原始数据的中间变量
float pCFFT_In[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCFFT_Out, 8)    // 存放FFT计算结果
float pCFFT_Out[2*FFT_SIZE+4];
#pragma DATA_SECTION (result_real,".DataInSdram")   // 存放FFT结果的实部
float result_real[FFT_SIZE];
#pragma DATA_SECTION (result_imag,".DataInSdram")   // 存放FFT结果的虚部
float result_imag[FFT_SIZE];
#pragma DATA_SECTION (result_ap,".DataInSdram")     // 存放FFT算出来的幅值
float result_ap[FFT_SIZE];
#pragma DATA_SECTION (result_ph,".DataInSdram")     // 存放FFT算出来的相角
float result_ph[FFT_SIZE];

#pragma DATA_ALIGN(pCFFT_InvOut, 8)
float pCFFT_InvOut[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCFFT_InOrig, 8)
float pCFFT_InOrig[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCTemp, 8)
float pCTemp[2*FFT_SIZE+4];

/* 二进制位翻转 */
#pragma DATA_ALIGN (brev , 8)
unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};


//三相电流电压矫正系数
 Correct_cor correct ={-710.8638, -644.012, 1.0602, 0, 0.7609, 0};

float abd=0.1,abdd=0.1;
int as=1,bs=1;
int ds=1,cs=1;

#endif /* C6745_ADS8556_HARMONIC_H_ */
