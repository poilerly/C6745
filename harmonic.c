/*
 * harmonic.c
 *    Function: ���ٸ���Ҷ�任 / ���ٸ���Ҷ��任
 *  Created on: 2016��8��21��
 *      Author: QW & poiler
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
 * 4��3��Nuttall��������ʼ�����������ü����Ԫ����
 * EX=1000��Ϊ�Ŵ���
 */
void NuttallWin_Init(void)
{
    //��������ʼ����
    uint16_t i;
    for(i = 0; i < FFT_SIZE; i++)  // ���Ԫ��������n=1,2,...,N-1.ֻ��1023����
    {
        nuttallwin[i]=(0.338946 \
                - 0.481973 * cossp(2*PI*i / FFT_SIZE) \
                + 0.161054 * cossp(4*PI*i / FFT_SIZE) \
                - 0.018027 * cossp(6*PI*i / FFT_SIZE) \
                )*EX;
        //�Ŵ� 32768 ���Լ���������ʧ,�� interpolation() ����С32768��;    1.85218e-4)=Vref * 0.6575/32768  /30  * 300
    }
}


/*
 * �ú������ڲ����ض�����ת��������,�������������
 * float *w : ָ�򴢴���ת���ӵ�����,���鳤��Ϊ2*N(��FFT�����еĳ���)
 * int n    : ��N(��FFT�����еĳ���)
 */
void Gen_Twiddle_FFT_Sp(float *w, uint16_t n)
{
    uint16_t i, j, k;
    double x_t, y_t, theta1, theta2, theta3;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2) //>>���ȼ�����<��<=
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cossp (theta1);
            y_t = sinsp (theta1);     //y_t=sin(2*pi*i/n)
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t; //��������

            theta2 = 4 * PI * i / n;
            x_t = cossp (theta2);
            y_t = sinsp (theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / n;
            x_t = cossp (theta3);
            y_t = sinsp (theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}


void FFT_Init(void)
{
    /* ����������ֵֻҪ����һ�ξͿ����� */
    NuttallWin_Init();

    /* ������ת����,��Ϊ��FFT�ĵ����ǹ̶���,������ת����ֻҪ����һ�ξͿ�����*/
    Gen_Twiddle_FFT_Sp(w, FFT_SIZE);
}


/*
 * ����ɢ�źŽ��мӴ�����;
 * 0.0163, 0.0004���ǵ�ѹ������ɢֵ���Եı���ϵ��;
 * �������ѹ�����Ĵ�С����У׼��220V, 1.5A.
 */
void FFT_Window(void)
{
    uint16_t i;

    for(i = 0; i < FFT_SIZE; i++)
    {
        fft_win.Ua[i] = fft.Ua[i] * nuttallwin[i];
        fft_win.Ia[i] = fft.Ia[i] * nuttallwin[i];

        fft_win.Ub[i] = fft.Ub[i] * nuttallwin[i] * 0.0163;
        fft_win.Ib[i] = fft.Ib[i] * nuttallwin[i] * 0.0004;

        fft_win.Uc[i] = fft.Uc[i] * nuttallwin[i] * 0.0163;
        fft_win.Ic[i] = fft.Ic[i] * nuttallwin[i] * 0.0004;
    }
}


/*
 * pInput: ��FFT������
 * w: ��ת��������
 * pCFFT_In: FFT����ĸ�������
 * pCFFT_Out: FFT����ĸ�������
 * result_real: FFT������������ʵ��
 * result_imag: FFT�������������鲿
 * result_ap: ��ֵ
 * result_ph: ��λ
 */
void Complex_FFT(float *pInput)
{
    uint16_t i,rad;

    rad = 4;    // ȷ�����ٸ���Ҷ�任��

    for (i = 0; i < FFT_SIZE; i++)
    {
        pCFFT_In[2*i] = pInput[i];  // ʵ��
        pCFFT_In[2*i+1] = 0;        // �鲿���Ϊ0
    }

    // ����һ�������źŸ���
    memcpy(pCFFT_InOrig,pCFFT_In,2*FFT_SIZE*sizeof(float));
    //=============================================================

    // FFT ����
    DSPF_sp_fftSPxSP (FFT_SIZE, pCFFT_In, w, pCFFT_Out, brev, rad, 0, FFT_SIZE);

    for(i = 0; i < FFT_SIZE; i++)
    {
        result_real[i] = pCFFT_Out[2*i];
        result_imag[i] = pCFFT_Out[2*i+1];

        result_ap[i] = result_real[i]*result_real[i] + result_imag[i]*result_imag[i];
        result_ap[i] = sqrtsp(result_ap[i]);

//        result_ap[i] = InvSqrt(result_ap[i]) * result_ap[i];
    }
    result_ap[0] = result_ap[0]/FFT_SIZE;   //ֱ������

    // ������λ
    result_ph[0] = 0;
    for(i = 1; i < FFT_SIZE; i++)
    {
        result_ph[i] = atan2sp(result_imag[i],result_real[i]);
        //�ó����ΪPI���ͣ�����λΪ"rad"����Ҫת��Ϊ�Ƕȣ���λΪ"��"��
        //�Ƕ�=result_ph[i]*180/pi;
    }

    //=============================================================
    //���²���Ϊ��任�����ڼ��FFT�Ƿ����гɹ�������ʽ���д���ʱ��Ҫע�͵�
    //=============================================================
    // ����һ�� FFT �������
    memcpy(pCTemp,pCFFT_Out,2*FFT_SIZE*sizeof(float));

    // IFFT ����
    DSPF_sp_ifftSPxSP(FFT_SIZE,pCFFT_Out,w,pCFFT_InvOut,brev,rad,0,FFT_SIZE);

    // �ָ� FFT ���
    memcpy(pCFFT_Out,pCTemp,2*FFT_SIZE*sizeof(float));

//    printf("\n���� FFT ���Խ��:");

    uint8_t Flag;
    for(i = 0; i < FFT_SIZE; i++)
        if(abs(pCFFT_InOrig[i] - pCFFT_InvOut[i]) > F_TOL)
            Flag = 1;

    if(Flag == 1)
        printf ("ʧ�ܣ�\n");
    else
        printf ("�ɹ���\n");
}


/*
 * ����Ƶ�ʼ��㺯��
 */
float Fundament_Freq()
{
    float funfreq;
    int t,t1,t2;
    unsigned long pitch;
    float maxValue1=0.0;
    float sub_max,midvalue,midvalue1,temp;

    t1 =  (F1-20)*(FFT_SIZE)/(FS);      // ��Fs*t/N=(50+-20);�ó���Ƶ�㷶Χ
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
 * ˫���߲�ֵ�㷨
 * �������壺
 * fn��֮ǰ����Ļ�ƵƵ�ʣ�result_rms����ֵ��result_phase�����
 */
void DoubleLine_Inter(float fn,float result_rms[],float result_phase[])
{
    int n;
    unsigned long pitch_t, t1, t2, t;
    float sub_max, midvalue, midvalue1, temp;
    float maxValue1;

    for(n = 1;n <= Harm_P; n++)     // find the max and sub_max value near every order,����n��г����ֵÿ��г������һ��
    {
        maxValue1=0.0;
        t1 = (n*fn-10)*FFT_SIZE/FS; //��Fs*t/N=(50+-10);�ó�г��Ƶ�㷶Χ
        t2 = (n*fn+10)*FFT_SIZE/FS;

        // ��������ʹ��
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

        midvalue = (maxValue1-sub_max)/(maxValue1+sub_max);//��Y3>Y2,maxValueΪY2,sub_maxΪY1��pitch_tָ��K1
                                                           //��Y3<Y2,maxValue��ΪY2,sub_max��ΪY1��pitch_tָ��K1

        midvalue1 = 2.95494514*midvalue+0.17671943*(midvalue*midvalue*midvalue) \
                +0.09230694*(midvalue*midvalue*midvalue*midvalue*midvalue);

        result_rms[n-1] = (sub_max+maxValue1)*(3.20976143+0.9187393*(midvalue1*midvalue1) \
                +0.14734229*(midvalue1*midvalue1*midvalue1*midvalue1))/FFT_SIZE/EX;

        result_phase[n-1] = (result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5));
        // ����   (�������޸�----(result_ph[pitch_t]+PI/2-PI*(midvalue1+0.5)))
        // ���������ʦ���������һ���ġ��������Ԫ���Ŀ�֪��
    }
}


/*
 * г����������ĺ���
 */
void Harmonic_Pro(void)
{
    uint16_t i;
    for(i = 0; i < FFT_SIZE; i++)  //ֱ��ƫ��У
        fft.Ua[i] = (fft.Ua[i]) + correct.DCbias_Ua;

    for(i = 0; i < FFT_SIZE; i++)  //�Ȳ�У׼,������ɵ�ѹʵ��ֵ220V����
        fft.Ua[i] = 323.5294 * (fft.Ua[i] * correct.ratio_Uaf)/32767;

    /* ��A��B��C �����ѹ�����źŽ��мӴ����� */
    FFT_Window();

    /* A���ѹ����FFT���� */
    Complex_FFT(fft_win.Ua);

    /* ����A���ѹ�Ļ���Ƶ�� */
    funfreq.Ua = Fundament_Freq();

    /* У׼A���ѹ�Ļ���Ƶ�� */
//    funfreq.Ua = funfreq.Ua * 1.008488888665938 - 0.433567223783763;

    /* ����A���ѹ����г���ķ�ֵ����� */
    DoubleLine_Inter(funfreq.Ua,rms.Ua,phase.Ua);
}
