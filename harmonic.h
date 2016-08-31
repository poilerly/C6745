/*
 * harmonic.h
 *
 *  Created on: 2016��8��29��
 *      Author: QW & poiler
 */

#ifndef C6745_ADS8556_HARMONIC_H_
#define C6745_ADS8556_HARMONIC_H_

#include "Header/vars.h"
#include "main.h"


/* Ҫ��FFT���������� */
#pragma DATA_SECTION (fft,".DataInSdram")
FFTData fft;

/* Ҫ��FFT�������Ӵ�������� */
#pragma DATA_SECTION (fft_win,".DataInSdram")
FFTData fft_win;

/* Nuttall��������ֵ���� */
#pragma DATA_SECTION (nuttallwin,".DataInSdram")
float nuttallwin[FFT_SIZE];

/* FFT����õĵ�ѹ���Ļ���Ƶ�� */
#pragma DATA_SECTION (funfreq,".DataInSdram")
FreqData funfreq;

/* ����������г����ѹ�����ľ�����ֵ */
#pragma DATA_SECTION (rms,".DataInSdram")
HarmData rms;

/* ����������г����ѹ��������� */
#pragma DATA_SECTION (phase,".DataInSdram")
HarmData phase;


/* ����������г����ѹ�����֮��ļн� */
float phasea[Harm_P];
float phaseb[Harm_P];
float phasec[Harm_P];

#pragma DATA_ALIGN (w, 8)           // �����ת����
float w[2*FFT_SIZE];
#pragma DATA_ALIGN(pCFFT_In, 8)     // ���FFT����ԭʼ���ݵ��м����
float pCFFT_In[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCFFT_Out, 8)    // ���FFT������
float pCFFT_Out[2*FFT_SIZE+4];
#pragma DATA_SECTION (result_real,".DataInSdram")   // ���FFT�����ʵ��
float result_real[FFT_SIZE];
#pragma DATA_SECTION (result_imag,".DataInSdram")   // ���FFT������鲿
float result_imag[FFT_SIZE];
#pragma DATA_SECTION (result_ap,".DataInSdram")     // ���FFT������ķ�ֵ
float result_ap[FFT_SIZE];
#pragma DATA_SECTION (result_ph,".DataInSdram")     // ���FFT����������
float result_ph[FFT_SIZE];

#pragma DATA_ALIGN(pCFFT_InvOut, 8)
float pCFFT_InvOut[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCFFT_InOrig, 8)
float pCFFT_InOrig[2*FFT_SIZE+4];
#pragma DATA_ALIGN(pCTemp, 8)
float pCTemp[2*FFT_SIZE+4];

/* ������λ��ת */
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


//���������ѹ����ϵ��
 Correct_cor correct ={-710.8638, -644.012, 1.0602, 0, 0.7609, 0};

float abd=0.1,abdd=0.1;
int as=1,bs=1;
int ds=1,cs=1;

#endif /* C6745_ADS8556_HARMONIC_H_ */
