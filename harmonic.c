/*
 * harmonic.c
 *
 *  Created on: 2016��8��19��
 *      Author: poiler
 */

#include "main.h"

#pragma DATA_SECTION (vva,".DataInSdram")   // Aͨ����ѹ
int16_t vva[WAVE_POINT];
#pragma DATA_SECTION (vvb,".DataInSdram")   // Bͨ����ѹ
int16_t vvb[WAVE_POINT];
#pragma DATA_SECTION (vvc,".DataInSdram")   // Cͨ����ѹ
int16_t vvc[WAVE_POINT];
#pragma DATA_SECTION (cca,".DataInSdram")   // Aͨ������
int16_t cca[WAVE_POINT];
#pragma DATA_SECTION (ccb,".DataInSdram")   // Bͨ������
int16_t ccb[WAVE_POINT];
#pragma DATA_SECTION (ccc,".DataInSdram")   // Cͨ������
int16_t ccc[WAVE_POINT];
