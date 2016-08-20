/*
 * harmonic.c
 *
 *  Created on: 2016年8月19日
 *      Author: poiler
 */

#include "main.h"

#pragma DATA_SECTION (vva,".DataInSdram")   // A通道电压
int16_t vva[WAVE_POINT];
#pragma DATA_SECTION (vvb,".DataInSdram")   // B通道电压
int16_t vvb[WAVE_POINT];
#pragma DATA_SECTION (vvc,".DataInSdram")   // C通道电压
int16_t vvc[WAVE_POINT];
#pragma DATA_SECTION (cca,".DataInSdram")   // A通道电流
int16_t cca[WAVE_POINT];
#pragma DATA_SECTION (ccb,".DataInSdram")   // B通道电流
int16_t ccb[WAVE_POINT];
#pragma DATA_SECTION (ccc,".DataInSdram")   // C通道电流
int16_t ccc[WAVE_POINT];
