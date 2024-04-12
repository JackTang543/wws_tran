#include "stm32l0xx_hal.h"

#ifndef __SBSP_ADC_H__
#define __SBSP_ADC_H__

#ifdef __cplusplus
extern "C"{
#endif

void sBSP_ADC_Init();
uint16_t sBSP_ADC_GetChannel(uint32_t channel);


#ifdef __cplusplus
}
#endif
#endif
