/*
 * harmonic.c
 *
 *  Created on: 2016��8��19��
 *      Author: poiler
 */

#include "main.h"

#pragma DATA_SECTION (vva,".DataInSdram")   // Aͨ����ѹ
int16_t vva[CYCLE_SIZE];
#pragma DATA_SECTION (vvb,".DataInSdram")   // Bͨ����ѹ
int16_t vvb[CYCLE_SIZE];
#pragma DATA_SECTION (vvc,".DataInSdram")   // Cͨ����ѹ
int16_t vvc[CYCLE_SIZE];
#pragma DATA_SECTION (cca,".DataInSdram")   // Aͨ������
int16_t cca[CYCLE_SIZE];
#pragma DATA_SECTION (ccb,".DataInSdram")   // Bͨ������
int16_t ccb[CYCLE_SIZE];
#pragma DATA_SECTION (ccc,".DataInSdram")   // Cͨ������
int16_t ccc[CYCLE_SIZE];