/*
 * harmonic.c
 *    Function: 快速傅里叶变换 / 快速傅里叶逆变换
 *  Created on: 2016年8月21日
 *      Author: QW & poiler
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
void Gen_Twiddle_FFT_Sp(float *w, uint16_t n)
{
    uint16_t i, j, k;
    double x_t, y_t, theta1, theta2, theta3;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2) //>>优先级高于<或<=
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cos (theta1);
            y_t = sin (theta1);     //y_t=sin(2*pi*i/n)
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t; //生成数组

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
 * 4阶3项Nuttall窗函数初始化，参数设置见卿柏元论文
 * EX=1000；为放大倍数
 */
void NuttallWin_Init(void)
{
    //窗函数初始化用
    uint16_t i;
    for(i = 0; i < FFT_SIZE; i++)  // 卿柏元的论文中n=1,2,...,N-1.只有1023个点
    {
        nuttallwin[i]=(0.338946 \
                -0.481973 * cos(2 * PI * i / FFT_SIZE) \
                +0.161054 * cos(4 * PI * i / FFT_SIZE) \
                -0.018027 * cos(6 * PI * i / FFT_SIZE) \
                )*EX;
        //放大 32768 倍以减少数据损失,在 interpolation() 内缩小32768倍;    1.85218e-4)=Vref * 0.6575/32768  /30  * 300
    }
}


void FFT_Init(void)
{
    /* 窗函数的数值只要计算一次就可以了 */
    NuttallWin_Init();

    /* 产生旋转因子,因为做FFT的点数是固定的,所以旋转因子只要产生一次就可以了*/
    Gen_Twiddle_FFT_Sp(w, FFT_SIZE);
}


/*
 * 对离散信号进行加窗处理;
 * 0.0163, 0.0004仅是电压电流离散值初略的比例系数;
 * 将各相电压电流的大小大致校准到220V, 1.5A.
 */
void FFT_Window(void)
{
    uint16_t i;

    for(i = 0; i < FFT_SIZE; i++)
    {
        fft_win.Ua[i] = fft.Ua[i] * nuttallwin[i] * 0.0163;
        fft_win.Ia[i] = fft.Ia[i] * nuttallwin[i] * 0.0004;

        fft_win.Ub[i] = fft.Ub[i] * nuttallwin[i] * 0.0163;
        fft_win.Ib[i] = fft.Ib[i] * nuttallwin[i] * 0.0004;

        fft_win.Uc[i] = fft.Uc[i] * nuttallwin[i] * 0.0163;
        fft_win.Ic[i] = fft.Ic[i] * nuttallwin[i] * 0.0004;
    }
}

/*
 * pInput: 做FFT的数据
 * w: 旋转因子数组
 * pCFFT_In: FFT输入的复数数组
 * pCFFT_Out: FFT输出的复数数组
 * result_real: FFT输出复数数组的实部
 * result_imag: FFT输出复数数组的虚部
 * result_ap: 幅值
 * result_ph: 相位
 */
void Complex_FFT(float *pInput)
{
    uint16_t i,rad;

    rad = 4;    // 确定快速傅里叶变换基

    for (i = 0; i < FFT_SIZE; i++)
    {
        pCFFT_In[2*i] = pInput[i];    // 实部
        pCFFT_In[2*i+1] = 0;        // 虚部填充为0
    }

    // 保留一份输入信号副本
    memcpy(pCFFT_InOrig,pCFFT_In,2*FFT_SIZE*sizeof(float));
    //=============================================================

    // FFT 计算
    DSPF_sp_fftSPxSP (FFT_SIZE, pCFFT_In, w, pCFFT_Out, brev, rad, 0, FFT_SIZE);


    for(i = 0; i < FFT_SIZE; i++)
    {
        result_real[i] = pCFFT_Out[2*i];
        result_imag[i] = pCFFT_Out[2*i+1];

        result_ap[i] = result_real[i]*result_real[i] + result_imag[i]*result_imag[i];
        result_ap[i] = sqrtsp(result_ap[i]);
        result_ap[i] = result_ap[i]*2/FFT_SIZE;

//        result_ap[i] = InvSqrt(result_ap[i]) * result_ap[i];
    }
    result_ap[0]=result_ap[0]/2;//直流分量

    // 计算相位
    result_ph[0] = 0;
    for(i = 1; i < FFT_SIZE; i++)
    {
        result_ph[i] = atan2(result_imag[i],result_real[i]);
        //得出结果为PI类型，即单位为"rad"；需要转换为角度，单位为"°"；
        //角度=result_ph[i]*180/pi;
    }

    //=============================================================
    //以下部分为逆变换，用于检测FFT是否运行成功，在正式运行代码时需要注释掉
    //=============================================================
    // 保留一份 FFT 结果副本
    memcpy(pCTemp,pCFFT_Out,2*FFT_SIZE*sizeof(float));

    // IFFT 计算
    DSPF_sp_ifftSPxSP(FFT_SIZE,pCFFT_Out,w,pCFFT_InvOut,brev,rad,0,FFT_SIZE);

    // 恢复 FFT 结果
    memcpy(pCFFT_Out,pCTemp,2*FFT_SIZE*sizeof(float));

    printf("\n复数 FFT 测试结果:");

    uint8_t Flag;
    for(i = 0; i < FFT_SIZE; i++)
        if(abs(pCFFT_InOrig[i] - pCFFT_InvOut[i]) > F_TOL)
            Flag = 1;

    if(Flag == 1)
        printf ("失败！\n");
    else
        printf ("成功！\n");
}


/*
 * 基波频率计算函数
 */
float Fundament_Freq()
{
    float funfreq;
    int t,t1,t2;
    unsigned long pitch;
    float maxValue1=0.0;
    float sub_max,midvalue,midvalue1,temp;

    t1 =  (F1-20)*(FFT_SIZE)/(FS);      // 由Fs*t/N=(50+-20);得出基频点范围
    t2 =  (F1+20)*(FFT_SIZE)/(FS);      // t1=4,  t2=9
    for(t = t1+1; t <= t2; t++)
    {
        if(result_ap[t] > maxValue1)
        {
            maxValue1 = result_ap[t];
            pitch = t;
        }
        abd=maxValue1;
    }
    if(result_ap[pitch-1] > result_ap[pitch+1])
    {
         sub_max = result_ap[pitch-1];
         pitch=pitch-1;

    }
    else
    {
        sub_max = result_ap[pitch+1];
        temp = sub_max;
        sub_max = maxValue1;
        maxValue1 = temp;
    }

    midvalue = (maxValue1-sub_max)/(maxValue1+sub_max);

    //midvalue1 = 3.49999999*midvalue;
    // al=2.95494514*be+0.17671943*(be^3)+0.09230694*(be^5);
    midvalue1 = 2.95494514*midvalue+0.17671943*(midvalue*midvalue*midvalue)+0.09230694*(midvalue*midvalue*midvalue*midvalue*midvalue);
//    abdd=midvalue1;
    funfreq = ((float)pitch+midvalue1+0.5)*FS/FFT_SIZE;
//    asm("\tnop");

    return funfreq;
}


/*
 * 双谱线插值算法
 * 参数含义：
 * fn：之前计算的基频频率，result_rms：校正后幅值，result_phase：校正后相角，intflag：含义待定
 */
void DoubleLine_Inter(float fn,float result_rms[],float result_phase[])
{
    int n;
    unsigned long pitch_t, t1, t2,  t;
    float sub_max, midvalue, midvalue1, temp;
    float maxValue1;

    for(n = 1;n <= Harm_P; n++)    // find the max and sub_max value near every order,计算n次谐波的值每次谐波计算一次
    {
        maxValue1=0.0;
        t1 = (n*fn-10)*FFT_SIZE/FS;             //由Fs*t/N=(50+-10);得出谐波频点范围
        t2 = (n*fn+10)*FFT_SIZE/FS;
        as=t1;bs=t2;

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

        result_rms[n-1] = (sub_max+maxValue1)*(3.20976143+0.9187393*(midvalue1*midvalue1) \
                +0.14734229*(midvalue1*midvalue1*midvalue1*midvalue1))/FFT_SIZE/EX;

        result_phase[n-1] = (result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5));
        // 弧度   (进行了修改----(result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5)))
        // 这里和唐老师仿真程序是一样的。对照卿柏元论文可知。
    }
}


void PhaseCalculation(float* vol_phase,float* cur_phase,float* phase ,int coeff)
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
void Harmonic_Pro(void)
{
    uint16_t i;
    for(i = 0; i < FFT_SIZE; i++)  //直流偏置校
        fft.Ua[i] = (fft.Ua[i]) + Correct.DCbias_Ua;

    for(i = 0; i < FFT_SIZE; i++)  //比差校准,并换算成电压实际值220V来算
        fft.Ua[i] = 323.5294 * (fft.Ua[i] * Correct.ratio_Uaf)/32767;

    /* 对A、B、C 三相电压电流信号进行加窗处理 */
    FFT_Window();

    /* A相电压进行FFT计算 */
    Complex_FFT(fft_win.Ua);

    /* 计算A相电压的基波频率 */
    funfreq.Ua = Fundament_Freq();

    /* 校准A相电压的基波频率 */
    funfreq.Ua = funfreq.Ua * 1.008488888665938 - 0.433567223783763;

    /* 计算A相电压的幅值及相角, 矫正各次谐波的幅值相位 */
    DoubleLine_Inter(funfreq.Ua,rms.Ua,phase.Ua);
}
