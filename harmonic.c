/*
 * harmonic.c
 *
 *  Created on: 2016��8��19��
 *      Author: poiler
 */

#include "harmonic.h"

/*
 * ��һ������ƽ����ȡ����
 */
float InvSqrt(float x)      // to compute x^-0.5
{
    float xhalf = 0.5f*x;

    long i = *(long*)&x;     // get bits for floating value
    i = 0x5f375a86 - (i>>1);  // gives initial guess y0 �������:0x5f375a86
    x = *(float*)&i;         // convert bits back to float
    x = x*(1.5f-xhalf*x*x);   // Newton step, repeating increases accuracy

    return x;
}


/*
 * �ú������ڲ����ض�����ת��������,�������������
 * float *w : ָ�򴢴���ת���ӵ�����,���鳤��Ϊ2*N(��FFT�����еĳ���)
 * int n    : ��N(��FFT�����еĳ���)
 */
void Gen_Twiddle_FFT_Sp (float *w, uint16_t n)
{
    uint16_t i, j, k;
    double x_t, y_t, theta1, theta2, theta3;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cos (theta1);
            y_t = sin (theta1);
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t;

            theta2 = 4 * PI * i / n;
            x_t = cos (theta2);
            y_t = sin (theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / n;
            x_t = cos (theta3);
            y_t = sin (theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}


/*
 * 4��3��Nuttall�������ļ���.
 */
void NuttallWinCalculation(void)
{
    uint16_t i;
    for(i = 0; i < FFT_POINT; i++)  // ���Ԫ��������n=1,2,...,N-1.ֻ��1023����
    {
        nuttallwin[i]=(0.338946 \
                -0.481973 * cos(2 * PI * i / FFT_POINT) \
                +0.161054 * cos(4 * PI * i / FFT_POINT) \
                -0.018027 * cos(6 * PI * i / FFT_POINT) \
                )*EX;
    }
}


void FFT_Init(void)
{
    /* ����������ֵֻҪ����һ�ξͿ����� */
    NuttallWinCalculation();

    /* ������ת����,��Ϊ��FFT�ĵ����ǹ̶���,������ת����ֻҪ����һ�ξͿ�����*/
    Gen_Twiddle_FFT_Sp(w, FFT_POINT);
}


/*
 * ����ɢ�źŽ��мӴ�����;
 * 0.0163, 0.0004���ǵ�ѹ������ɢֵ���Եı���ϵ��;
 * �������ѹ�����Ĵ�С����У׼��220V, 1.5A.
 */
void FFT_Window(void)
{
    uint16_t i;

    for(i = 0; i < FFT_POINT; i++)
    {
        vola_win[i] = (float)vola_sample[i] * nuttallwin[i] * 0.0163;
        volb_win[i] = (float)volb_sample[i] * nuttallwin[i] * 0.0163;
        volc_win[i] = (float)volc_sample[i] * nuttallwin[i] * 0.0163;

        cura_win[i] = (float)cura_sample[i] * nuttallwin[i] * 0.0004;
        curb_win[i] = (float)curb_sample[i] * nuttallwin[i] * 0.0004;
        curc_win[i] = (float)curc_sample[i] * nuttallwin[i] * 0.0004;
    }
}


void Complex_FFT(float *pInput, uint16_t n)
{
    uint16_t i,rad;
    float *twiddle;

    /*
     * �ڼ�����ʹ����С��FFT���Ρ�
     * ���N��4�ı�������ͨ��������Ϊ4��������ͨ��������Ϊ2��
     */
    if (n == 16 || n == 64 || n == 256 || n == 1024 || n == 4096 || n == 16384)
        rad = 4;
    else if (n == 8 || n == 32 || n == 128 || n == 512 || n == 2048 || n == 8192)
        rad = 2;

    twiddle = (float *) w;

    // complex FFT
    for (i = 0; i < n; i++)
    {
        pCFFT_In[2 * i] = pInput[i];    // update Real part
        pCFFT_In[2 * i + 1] = 0;        // fill 0 for imag part
    }

    // Forward FFT Calculation..
    DSPF_sp_fftSPxSP (n, pCFFT_In, twiddle, pCFFT_Out, brev, rad, 0, n);

    CFFT_real[0] = 0;   //ֱ��?
    CFFT_imag[0] = 0;
    CFFT_phase[0] = 0;  //Ҳ��ֱ��,������0?
    for(i = 1; i < n; i++)
    {
        CFFT_real[i] = pCFFT_Out[2 * i];
        CFFT_imag[i] = pCFFT_Out[2 * i + 1];

        CFFT_amplitude[i] = CFFT_real[i]*CFFT_real[i] + CFFT_imag[i]*CFFT_imag[i];
        CFFT_amplitude[i] = InvSqrt(CFFT_amplitude[i]) * CFFT_amplitude[i];
        CFFT_phase[i] = atan2(CFFT_imag[i],CFFT_real[i]);
    }
}


float FundamentalFrequency()
{
    float wavefreq;
    unsigned long t,t1,t2,pitch;
    float maxValue1=0.0;
    float sub_max,midvalue,midvalue1,temp;

    t1 =  (FB-20)*(FFT_POINT)/(FS);
    t2 =  (FB+20)*(FFT_POINT)/(FS);
    for(t = t1+1; t <= t2; t++)
    {
        if(CFFT_amplitude[t] > maxValue1)
        {
            maxValue1 = CFFT_amplitude[t];
            pitch = t;
        }
    }
    if(CFFT_amplitude[pitch-1] > CFFT_amplitude[pitch+1])
    {
         sub_max = CFFT_amplitude[pitch-1];
         pitch=pitch-1;

    }
    else
    {
        sub_max = CFFT_amplitude[pitch+1];
        temp = sub_max;
        sub_max = maxValue1;
        maxValue1 = temp;
    }

    midvalue = (maxValue1-sub_max)/(maxValue1+sub_max);
    //midvalue1 = 3.49999999*midvalue;
    // al=2.95494514*be+0.17671943*(be^3)+0.09230694*(be^5);
    midvalue1 = 2.95494514*midvalue+0.17671943*(midvalue*midvalue*midvalue)+0.09230694*(midvalue*midvalue*midvalue*midvalue*midvalue);
    wavefreq = ((float)pitch+midvalue1+0.5)*FS/FFT_POINT;
//    asm("\tnop");

    return wavefreq;
}




void Interpolation(float fn,float result_ap[],float result_ph[] ,int intflag)
{
    int as,bs;
    int n;
    unsigned long pitch_t, t1, t2,  t;
    float sub_max, midvalue, midvalue1, temp;
    float maxValue1;

    for(n=1;n<=HARM_N;n++)    // find the max and sub_max value near every order,����n��г����ֵÿ��г������һ��
    {
        maxValue1=0.0;
        t1 = (n*fn-10)*FFT_POINT/FS;             //��Fs*t/N=(50+-10);�ó�г��Ƶ�㷶Χ
        t2 = (n*fn+10)*FFT_POINT/FS;
        as=t1;bs=t2;    //  a=fn; b=fs;

        for(t=t1+1;t<=t2;t++)
        {
            if(result_ap[t] > maxValue1)
            {
                maxValue1 = result_ap[t];
                pitch_t = t;
            }
        }

        if(result_ap[pitch_t-1] > result_ap[pitch_t+1])
        {
            sub_max = result_ap[pitch_t-1];
            pitch_t = pitch_t-1;
        }
        else
        {
            sub_max = result_ap[pitch_t+1];
            temp = sub_max;
            sub_max = maxValue1;
            maxValue1 = temp;
        }

        midvalue = (maxValue1-sub_max)/(maxValue1+sub_max);//��Y3>Y2,maxValueΪY2,sub_maxΪY1��pitch_tָ��K1
                                                           //��Y3<Y2,maxValue��ΪY2,sub_max��ΪY1��pitch_tָ��K1

        midvalue1 = 2.95494514*midvalue+0.17671943*(midvalue*midvalue*midvalue) \
                +0.09230694*(midvalue*midvalue*midvalue*midvalue*midvalue);

        result_ap[n-1] = (sub_max+maxValue1)*(3.20976143+0.9187393*(midvalue1 * midvalue1) \
                +0.14734229*(midvalue1*midvalue1*midvalue1*midvalue1))/FFT_POINT/EX;

        result_ph[n-1]= (result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5));
        // ����   (�������޸�----(result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5)))
        // ���������ʦ���������һ���ġ��������Ԫ���Ŀ�֪��
    }
}


void PhaseCalculation(float * vol_phase,float*  cur_phase,float* phase ,int coeff)
{
    short u;
    float pherro;

    pherro =((float)coeff)/32768;
    for(u=0;u<50;u++)
    {
        phase[u] = vol_phase[u] - cur_phase[u] + (u+1) * pherro;
        while((phase[u] > 2*PI) || (phase[u] < 0))
        {
            if(phase[u]>2*PI)
                phase[u] = phase[u]-2*PI;
            else
                phase[u] = phase[u]+2*PI;
        }
    }
}


/*
 * г����������ĺ���
 */
void HarmomicProcess(void)
{
    /* ����ɢ�źŵ�ѹ�������мӴ����� */
    FFT_Window();

    /*
     * A���ѹ�����ķ�ֵ,Ƶ��,��Ǽ���
     */
    Complex_FFT(vola_win,FFT_POINT);  // A���ѹ����FFT����

    vola_freq = FundamentalFrequency(); // ����A���ѹ��Ӧ�Ļ���Ƶ��

    vola_freq = vola_freq * 1.008488888665938 - 0.433567223783763; // ������ѹƵ��У׼

    Interpolation(vola_freq,vola_rms,vola_phase,1);  // ����A���ѹ�ķ�ֵ�����


    Complex_FFT(cura_win,FFT_POINT);  // A���������FFT����
//    vola_freq = FrequencyCalculation(); // ����A�������Ӧ�Ļ���Ƶ��
    cura_freq = vola_freq;  // ���õ�ѹƵ�ʴ������Ƶ�ʽ��к�������
    Interpolation(cura_freq,cura_rms,cura_phase,0);  // ����A������ķ�ֵ�����

    PhaseCalculation(vola_phase, cura_phase, phasea, calibration_coeff[2]); // A���ѹ�����нǼ���


    /*
     * B���ѹ�����ķ�ֵ,Ƶ��,��Ǽ���
     */




    /*
     * C���ѹ�����ķ�ֵ,Ƶ��,��Ǽ���
     */


    /* г������������� */
    harmonicflag = FALSE;
}






///* Number of samples for which FFT needs to be calculated */
//#define N 256
///* Number of unique sine waves in input data */
//#define NUM_SIN_WAVES 4
//
//
///* Align the tables that we have to use */
//#pragma DATA_ALIGN(x_ref, 8);
//float   x_ref [2*N];
//
//
///*
//    This function generates the input data and also updates the
//    input data arrays used by the various FFT kernels
//*/
//void generateInput (int numSinWaves)
//{
//    int   i, j;
//    float sinWaveIncFreq, sinWaveMag;
//
//    /*
//        Based on numSinWave information, create the input data. The
//        input data is first created using floating point dataType. The
//        floating point data type is then converted to the appropriate
//        fixed point notation
//    */
//
//    /* Clear the input floating point array */
//    for (i = 0; i < N; i++) {
//        x_ref[2*i]   = (float)0.0;
//        x_ref[2*i+1] = (float)0.0;
//    }
//
//    /* Calculate the incremental freq for each sin wave */
//    sinWaveIncFreq = ((float)3.142)/(numSinWaves*(float)1.0);
//
//    /* Calculate the magnitude for each sin wave */
//    sinWaveMag = (float)1.0/(numSinWaves * (float)1.0*N);
//
//    /* Create the input array as sum of the various sin wave data */
//    for (j = 0; j < numSinWaves; j++) {
//        for (i = 0; i < N; i++) {
//            x_ref[2*i]  += sinWaveMag * (float)cos(sinWaveIncFreq*j*i);
//            x_ref[2*i+1] = (float) 0.0;
//        }
//    }
//
//    /* Copy the reference input data to the various input arrays */
//    for (i = 0; i < N; i++) {
//        x_sp [2*i]   = x_ref[2*i];
//        x_sp [2*i+1] = x_ref[2*i+1];
//    }
//}
