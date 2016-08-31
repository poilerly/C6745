/*
 * harmonic.h
 *
 *  Created on: 2016��8��29��
 *      Author: poiler
 */

#ifndef HARMONIC_H_
#define HARMONIC_H_

#include "main.h"
#include "dsplib/DSPF_sp_fftSPxSP.h"

extern uint8_t harmonicflag;

#pragma DATA_SECTION (vola_sample,".DataInSdram")     // Aͨ����ѹFFT����
int16_t vola_sample[FFT_POINT];
#pragma DATA_SECTION (volb_sample,".DataInSdram")     // Bͨ����ѹFFT����
int16_t volb_sample[FFT_POINT];
#pragma DATA_SECTION (volc_sample,".DataInSdram")     // Cͨ����ѹFFT����
int16_t volc_sample[FFT_POINT];
#pragma DATA_SECTION (cura_sample,".DataInSdram")     // Aͨ������FFT����
int16_t cura_sample[FFT_POINT];
#pragma DATA_SECTION (curb_sample,".DataInSdram")     // Bͨ������FFT����
int16_t curb_sample[FFT_POINT];
#pragma DATA_SECTION (curc_sample,".DataInSdram")     // Cͨ������FFT����
int16_t curc_sample[FFT_POINT];

#pragma DATA_SECTION (volb_win,".DataInSdram")     // Aͨ����ѹ�Ӵ��������
float vola_win[FFT_POINT];
#pragma DATA_SECTION (volb_win,".DataInSdram")     // Bͨ����ѹ�Ӵ��������
float volb_win[FFT_POINT];
#pragma DATA_SECTION (vola_win,".DataInSdram")     // Cͨ����ѹ�Ӵ��������
float volc_win[FFT_POINT];
#pragma DATA_SECTION (cura_win,".DataInSdram")     // Aͨ�������Ӵ��������
float cura_win[FFT_POINT];
#pragma DATA_SECTION (curb_win,".DataInSdram")     // Bͨ�������Ӵ��������
float curb_win[FFT_POINT];
#pragma DATA_SECTION (curc_win,".DataInSdram")     // Cͨ�������Ӵ��������
float curc_win[FFT_POINT];

#pragma DATA_SECTION (nuttallwin,".DataInSdram")       //Nuttall��������ֵ����
float nuttallwin[FFT_POINT];


/* ��ѹ�����Ļ���Ƶ�� */
float vola_freq;
float volb_freq;
float volc_freq;
float cura_freq;
float cura_freq;
float cura_freq;

/* ����������г����ѹ�����ľ�����ֵ */
#pragma DATA_SECTION (vola_rms,".DataInSdram")
float vola_rms[HARM_N];
float volb_rms[HARM_N];
float volc_rms[HARM_N];
float cura_rms[HARM_N];
float curb_rms[HARM_N];
float curc_rms[HARM_N];

/* ����������г����ѹ��������� */
#pragma DATA_SECTION (vola_phase,".DataInSdram")
float vola_phase[HARM_N];
float volb_phase[HARM_N];
float volc_phase[HARM_N];
float cura_phase[HARM_N];
float curb_phase[HARM_N];
float curc_phase[HARM_N];

/* ����������г����ѹ�����֮��ļн� */
float phasea[HARM_N];
float phaseb[HARM_N];
float phasec[HARM_N];

#pragma DATA_ALIGN (w, 8)           // �����ת����
float w[2 * FFT_POINT];
#pragma DATA_ALIGN(pCFFT_In, 8)     // The alignment is very important
float pCFFT_In[2 * FFT_POINT + 4];
#pragma DATA_ALIGN(pCFFT_Out, 8)    // for input/ouput and twiddle pointers
float pCFFT_Out[2 * FFT_POINT + 4];

#pragma DATA_SECTION (CFFT_real,".DataInSdram")          // ���FFT�����ʵ��
float CFFT_real[FFT_POINT];
#pragma DATA_SECTION (CFFT_imag,".DataInSdram")          // ���FFT������鲿
float CFFT_imag[FFT_POINT];
#pragma DATA_SECTION (CFFT_amplitude,".DataInSdram")     // ��FFT������ķ�ֵ
float CFFT_amplitude[FFT_POINT];
#pragma DATA_SECTION (CFFT_phase,".DataInSdram")         // ��FFT����������
float CFFT_phase[FFT_POINT];


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



/* У׼���֣���ʱδ�ã���ǰг�����ܱ����õ�����ЩУ׼ϵ���� */
int calibration_coeff[9]={
        -5347,   //    Uarms_coeff=0;  //a���ѹУ��ϵ�� 10000
        -4782,      //         Iarms_coeff = 0;     //a�����У��ϵ��10000
        0,//-13,//197,      //           Gama_a   = 0;     //a��ǲ���ȣ� �Ŵ�10000�����Ա������洢

        -5369,      //    Ubrms_coeff =0;      //b���ѹУ��ϵ��  10000
        -4802,      //     Ibrms_coeff = 0;        //b�����У��ϵ��10000
        0,//-9,//197,       //         Gama_b   = 0;     //b��ǲ� �����ȣ�*10000

        -5354,     //         Ucrms_coeff=0;     //c���ѹУ��ϵ��10000
        -4751,     //   Icrms_coeff = 0;     //c�����У��ϵ��10000
        0//-19//197     //   Gama_c    = 0;        //c�� �ǲ���ȣ�*10000       ={0,0,0,0,0,0,0,0,0};/*
};

#endif /* HARMONIC_H_ */
