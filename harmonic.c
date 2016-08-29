/*
 * harmonic.c
 *
 *  Created on: 2016年8月19日
 *      Author: poiler
 */

#include "harmonic.h"

/*
 * 将一个数开平方并取倒数
 */
float InvSqrt(float x)      // to compute x^-0.5
{
    float xhalf = 0.5f*x;

    long i = *(long*)&x;     // get bits for floating value
    i = 0x5f375a86 - (i>>1);  // gives initial guess y0 卡马克数:0x5f375a86
    x = *(float*)&i;         // convert bits back to float
    x = x*(1.5f-xhalf*x*x);   // Newton step, repeating increases accuracy

    return x;
}


/*
 * 该函数用于产生特定的旋转因子序列,有两个输入参数
 * float *w : 指向储存旋转因子的数组,数组长度为2*N(做FFT的序列的长度)
 * int n    : 即N(做FFT的序列的长度)
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
 * 4项3阶Nuttall窗函数的计算.
 */
void NuttallWinCalculation(void)
{
    uint16_t i;
    for(i = 0; i < FFT_POINT; i++)  // 卿柏元的论文中n=1,2,...,N-1.只有1023个点
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
    /* 窗函数的数值只要计算一次就可以了 */
    NuttallWinCalculation();

    /* 产生旋转因子,因为做FFT的点数是固定的,所以旋转因子只要产生一次就可以了*/
    Gen_Twiddle_FFT_Sp(w, FFT_POINT);
}


/*
 * 对离散信号进行加窗处理;
 * 0.0163, 0.0004仅是电压电流离散值初略的比例系数;
 * 将各相电压电流的大小大致校准到220V, 1.5A.
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
     * 在计算中使用最小的FFT蝶形。
     * 如果N是4的倍数，这通常被设置为4。否则，它通常被设置为2。
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

    CFFT_real[0] = 0;   //直流?
    CFFT_imag[0] = 0;
    CFFT_phase[0] = 0;  //也是直流,所以置0?
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

    for(n=1;n<=HARM_N;n++)    // find the max and sub_max value near every order,计算n次谐波的值每次谐波计算一次
    {
        maxValue1=0.0;
        t1 = (n*fn-10)*FFT_POINT/FS;             //由Fs*t/N=(50+-10);得出谐波频点范围
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

        midvalue = (maxValue1-sub_max)/(maxValue1+sub_max);//若Y3>Y2,maxValue为Y2,sub_max为Y1。pitch_t指向K1
                                                           //若Y3<Y2,maxValue变为Y2,sub_max变为Y1。pitch_t指向K1

        midvalue1 = 2.95494514*midvalue+0.17671943*(midvalue*midvalue*midvalue) \
                +0.09230694*(midvalue*midvalue*midvalue*midvalue*midvalue);

        result_ap[n-1] = (sub_max+maxValue1)*(3.20976143+0.9187393*(midvalue1 * midvalue1) \
                +0.14734229*(midvalue1*midvalue1*midvalue1*midvalue1))/FFT_POINT/EX;

        result_ph[n-1]= (result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5));
        // 弧度   (进行了修改----(result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5)))
        // 这里和唐老师仿真程序是一样的。对照卿柏元论文可知。
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
 * 谐波分析处理的函数
 */
void HarmomicProcess(void)
{
    /* 对离散信号电压电流进行加窗处理 */
    FFT_Window();

    /*
     * A相电压电流的幅值,频率,相角计算
     */
    Complex_FFT(vola_win,FFT_POINT);  // A相电压进行FFT计算

    vola_freq = FundamentalFrequency(); // 计算A相电压对应的基波频率

    vola_freq = vola_freq * 1.008488888665938 - 0.433567223783763; // 基波电压频率校准

    Interpolation(vola_freq,vola_rms,vola_phase,1);  // 计算A相电压的幅值及相角


    Complex_FFT(cura_win,FFT_POINT);  // A相电流进行FFT计算
//    vola_freq = FrequencyCalculation(); // 计算A相电流对应的基波频率
    cura_freq = vola_freq;  // 采用电压频率代替电流频率进行后续计算
    Interpolation(cura_freq,cura_rms,cura_phase,0);  // 计算A相电流的幅值及相角

    PhaseCalculation(vola_phase, cura_phase, phasea, calibration_coeff[2]); // A相电压电流夹角计算


    /*
     * B相电压电流的幅值,频率,相角计算
     */




    /*
     * C相电压电流的幅值,频率,相角计算
     */


    /* 谐波分析处理完成 */
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
